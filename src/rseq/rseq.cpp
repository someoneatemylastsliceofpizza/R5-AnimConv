#include <pch.h>
#include <rseq/rseq.h>
using namespace r5;

void ParseRSEQ_v10(std::string in_dir, temp::rig_t& rig) {
	ProgressBar bar(rig.rseqpaths.size());
	std::vector<std::future<void>> tasks;
	std::mutex mutex;

	if (!_enable_verbose && !rig.rseqpaths.empty()) bar.Print();
	for (const auto& file : rig.rseqpaths) {
		tasks.push_back(std::async(std::launch::async, [&, file]() {
			std::string path = in_dir + "\\" + file;
			std::filesystem::path relative_path = std::filesystem::relative(path, in_dir);
			std::string out_dir = path + "_conv";

			if (!std::filesystem::is_regular_file(path)) {
				printf("[!] Error: rseq not found for %s\n", relative_path.string().c_str());
				return; //continue
			}

			size_t inputFileSize = std::filesystem::file_size(path);
			std::vector<char> buffer(inputFileSize, 0);
			{
				std::lock_guard<std::mutex> lock(mutex);
				std::ifstream rseq_stream(path, std::ios::binary);
				rseq_stream.read(buffer.data(), inputFileSize);
				rseq_stream.close();
			}

			// out path
			auto first_dir = relative_path.begin();
			std::filesystem::path newPath = first_dir->string();
			++first_dir;
			for (; first_dir != relative_path.end(); ++first_dir) {
				newPath /= *first_dir;
			}
			out_dir = in_dir + "\\conv\\" + newPath.string();

			// bad size
			if (inputFileSize <= sizeof(anim::v10::mstudioseqdesc_t)) {
				printf("[!] Skipping %s (%zu byte)\n", std::filesystem::path(file).stem().string().c_str(), inputFileSize);
				return; //continue
			}

			auto* pSeqDesc = reinterpret_cast<anim::v10::mstudioseqdesc_t*>(buffer.data());

			std::string seqname = STRING_FROM_IDX(pSeqDesc, pSeqDesc->szlabelindex);
			std::string activityname = STRING_FROM_IDX(pSeqDesc, pSeqDesc->szactivitynameindex);
			int numanims = pSeqDesc->groupsize[0] * pSeqDesc->groupsize[1];
			int numbones = (pSeqDesc->activitymodifierindex - pSeqDesc->weightlistindex) / 4;

			temp::Sequence seq(seqname, numbones);
			seq.anims.reserve(24);
			seq.path = out_dir;
			seq.flags = pSeqDesc->flags;
			seq.activityname = activityname;
			seq.activity = pSeqDesc->activity;
			seq.actweight = pSeqDesc->actweight;
			seq.bbmin = pSeqDesc->bbmin;
			seq.bbmax = pSeqDesc->bbmax;
			seq.groupsize[0] = pSeqDesc->groupsize[0];
			seq.groupsize[1] = pSeqDesc->groupsize[1];
			seq.paramindex[0] = pSeqDesc->paramindex[0];
			seq.paramindex[1] = pSeqDesc->paramindex[1];
			seq.paramstart[0] = pSeqDesc->paramstart[0];
			seq.paramstart[1] = pSeqDesc->paramstart[1];
			seq.paramend[0] = pSeqDesc->paramend[0];
			seq.paramend[1] = pSeqDesc->paramend[1];
			seq.paramparent = pSeqDesc->paramparent;
			seq.fadeintime = pSeqDesc->fadeintime;
			seq.fadeouttime = pSeqDesc->fadeouttime;
			seq.localentrynode = pSeqDesc->localentrynode;
			seq.localexitnode = pSeqDesc->localexitnode;
			seq.nodeflags = pSeqDesc->nodeflags;
			seq.entryphase = pSeqDesc->entryphase;
			seq.exitphase = pSeqDesc->exitphase;
			seq.lastframe = pSeqDesc->lastframe;
			seq.nextseq = pSeqDesc->nextseq;
			seq.pose = pSeqDesc->pose;
			seq.ikResetMask = pSeqDesc->ikResetMask;
			seq.unk1 = pSeqDesc->unk1;

			verbose("%s\n", seqname.c_str());

			ParsePoseKey(pSeqDesc, seq);
			ParseEvent_v10(pSeqDesc, seq);
			ParseAutoLayer(pSeqDesc, seq);
			ParseWeightList(pSeqDesc, seq);
			ParseActMod_v10(pSeqDesc, seq);

			// blends
			auto* pBlends = PTR_FROM_IDX(int, pSeqDesc, pSeqDesc->animindexindex);
			std::vector<int32_t> animIndexes = GetAnimIndexes(pBlends, seq, numanims);

			for (int anim_iter = 0; anim_iter < seq.numuniqueblends; anim_iter++) {
				// anim 
				auto* pAnimDesc = PTR_FROM_IDX(anim::v10::mstudioanimdesc_t, pSeqDesc, animIndexes[anim_iter]);
				temp::animdesc_t anim{};
				anim.name = STRING_FROM_IDX(pAnimDesc, pAnimDesc->sznameindex);
				anim.fps = pAnimDesc->fps;
				anim.flags = pAnimDesc->flags;
				anim.numframes = pAnimDesc->numframes;
				anim.InitData(seq.numbones, rig, seq.IsAdditive());

				if (!(anim.flags & ANIM_VALID)) {
					seq.anims.push_back(anim);
					continue;
				}

				// anim sections
				uint32_t num_sections = 1;
				anim::mstudioanimsections_t* animsections{};
				if (pAnimDesc->sectionindex) {
					num_sections = GetSectionCount(*pAnimDesc);
					animsections = PTR_FROM_IDX(anim::mstudioanimsections_t, pAnimDesc, pAnimDesc->sectionindex);
				}
				uint32_t bfa_size = ((numbones + 3) / 2) & ~1;

				// external data
				std::vector<char> extn_buffer;
				std::string extn_file = path + "_extn";
				if (std::filesystem::exists(extn_file)) {
					std::ifstream extn_stream(extn_file, std::ios::binary); 
					extn_buffer.resize(std::filesystem::file_size(extn_file));
					extn_stream.read(extn_buffer.data(), std::filesystem::file_size(extn_file));
					extn_stream.close();
				}

				uint32_t sectionbaseframe = 0;
				for (uint32_t section = 0; section < num_sections; section++) {
					uint32_t sectionframes = GetSectionLength(*pAnimDesc, section, num_sections);
					char* pBoneFlagArray = PTR_FROM_IDX(char, pAnimDesc, pAnimDesc->animindex);
					
					if (pAnimDesc->sectionindex) {
						if (animsections[section].isExternal) {
							if (animsections[section].animidx >= extn_buffer.size()) PRINTANDTHROW(extn_file, "Out of rseq_extn range.");
							pBoneFlagArray = PTR_FROM_IDX(char, extn_buffer.data(), animsections[section].animidx);
						}
						else {
							pBoneFlagArray = PTR_FROM_IDX(char, pAnimDesc, animsections[section].animidx);
						}
					}

					for (uint32_t localframe = 0; localframe < sectionframes; localframe++) {
						uint32_t frame = sectionbaseframe + localframe;
						auto* pTrack = PTR_FROM_IDX(anim::mstudio_rle_anim_t, pBoneFlagArray, bfa_size);
						for (int bone = 0; bone < numbones; bone++) {
							uint8_t boneFlags = pBoneFlagArray[bone / 2] >> (4 * (bone % 2));

							Vector3& trackpos = anim.animdata[bone].pos[frame];
							Vector3& trackrot = anim.animdata[bone].rot[frame];
							Vector3& trackscl = anim.animdata[bone].scl[frame];
							if (boneFlags & RLE::BONEDATA) {
								auto* pTrackData = PTR_FROM_IDX(uint16_t, pTrack, sizeof(anim::mstudio_rle_anim_t));
								if (boneFlags & RLE::BONEPOS) RLE::CalcBonePosition(*pTrack, &pTrackData, trackpos, localframe);
								if (boneFlags & RLE::BONEROT) RLE::CalcBoneQuaternion(*pTrack, &pTrackData, trackrot, localframe);
								if (boneFlags & RLE::BONESCALE) RLE::CalcBoneScale(*pTrack, &pTrackData, trackscl, localframe);
								pTrack = (anim::mstudio_rle_anim_t*)((char*)pTrack + pTrack->size);
							}
						}
					}
					sectionbaseframe += sectionframes;
				}
				RLE::ParseIkrules_v10(pAnimDesc, anim);
				RLE::ParseFrameMovements_v10(pAnimDesc, anim);
				seq.anims.push_back(anim);
			}
			{
				std::lock_guard<std::mutex> lock(mutex);
				rig.sequences.push_back(std::move(seq));
			}
			std::vector<char>().swap(buffer);
			}));
		if (!_enable_verbose) bar.AddAndPrint();
	}
	for (auto& t : tasks) t.get();
	printf("\n");
}


void ParseRSEQ_v11(std::string in_dir, temp::rig_t& rig) {
	ProgressBar bar(rig.rseqpaths.size());
	std::vector<std::future<void>> tasks;
	std::mutex mutex;

	if (!_enable_verbose && !rig.rseqpaths.empty()) bar.Print();
	for (const auto& file : rig.rseqpaths) {
		tasks.push_back(std::async(std::launch::async, [&, file]() {
		std::string path = in_dir + "\\" + file;
		std::filesystem::path relative_path = std::filesystem::relative(path, in_dir);
		std::string out_dir = path + "_conv";

		if (!std::filesystem::is_regular_file(path)) {
			printf("[!] Error: rseq not found for %s\n", relative_path.string().c_str());
			return; //continue
		}

		size_t inputFileSize = std::filesystem::file_size(path);
		std::vector<char> buffer(inputFileSize, 0);

		{
			std::lock_guard<std::mutex> lock(mutex);
			std::ifstream rseq_stream(path, std::ios::binary);
			rseq_stream.read(buffer.data(), inputFileSize);
			rseq_stream.close();
		}

		// out path
		auto first_dir = relative_path.begin();
		std::filesystem::path newPath = first_dir->string();
		++first_dir;
		for (; first_dir != relative_path.end(); ++first_dir) {
			newPath /= *first_dir;
		}
		out_dir = in_dir + "\\conv\\" + newPath.string();

		// bad size
		if (inputFileSize <= sizeof(anim::v11::mstudioseqdesc_t)) {
			printf("[!] Skipping %s (%zu byte)\n", std::filesystem::path(file).stem().string().c_str(), inputFileSize);
			return; //continue
		}

		auto* pSeqDesc = reinterpret_cast<anim::v11::mstudioseqdesc_t*>(buffer.data());

		std::string seqname = STRING_FROM_IDX(pSeqDesc, OFFSET(pSeqDesc->szlabelindex));
		std::string activityname = STRING_FROM_IDX(pSeqDesc, OFFSET(pSeqDesc->szactivitynameindex));
		int numanims = pSeqDesc->groupsize[0] * pSeqDesc->groupsize[1];
		int numbones = (pSeqDesc->activitymodifierindex - pSeqDesc->weightlistindex) / 4;

		temp::Sequence seq(seqname, numbones);
		seq.anims.reserve(24);
		seq.path = out_dir;
		seq.flags = pSeqDesc->flags;
		seq.activityname = activityname;
		seq.activity = pSeqDesc->activity;
		seq.actweight = pSeqDesc->actweight;
		seq.bbmin = pSeqDesc->bbmin;
		seq.bbmax = pSeqDesc->bbmax;
		seq.groupsize[0] = pSeqDesc->groupsize[0];
		seq.groupsize[1] = pSeqDesc->groupsize[1];
		seq.paramindex[0] = pSeqDesc->paramindex[0];
		seq.paramindex[1] = pSeqDesc->paramindex[1];
		seq.paramstart[0] = pSeqDesc->paramstart[0];
		seq.paramstart[1] = pSeqDesc->paramstart[1];
		seq.paramend[0] = pSeqDesc->paramend[0];
		seq.paramend[1] = pSeqDesc->paramend[1];
		seq.fadeintime = pSeqDesc->fadeintime;
		seq.fadeouttime = pSeqDesc->fadeouttime;
		seq.localentrynode = pSeqDesc->localentrynode;
		seq.localexitnode = pSeqDesc->localexitnode;
		seq.ikResetMask = pSeqDesc->ikResetMask;
		seq.unk1 = pSeqDesc->unk1;

		verbose("%s\n", seqname.c_str());

		ParsePoseKey(pSeqDesc, seq);
		ParseEvent_v11(pSeqDesc, seq);
		ParseAutoLayer(pSeqDesc, seq);
		ParseWeightList(pSeqDesc, seq);
		ParseActMod_v11(pSeqDesc, seq);

		// blends
		auto* pBlends = PTR_FROM_IDX(uint16_t, pSeqDesc, OFFSET(pSeqDesc->animindexindex));
		std::vector<int32_t> animIndexes = GetAnimIndexes(pBlends, seq, numanims);
		for (int anim_iter = 0; anim_iter < seq.numuniqueblends; anim_iter++) {
			// anim 
			auto* pAnimDesc = PTR_FROM_IDX(anim::v11::mstudioanimdesc_t, pSeqDesc, animIndexes[anim_iter]);
			temp::animdesc_t anim{};
			anim.name = STRING_FROM_IDX(pAnimDesc, OFFSET(pAnimDesc->sznameindex));
			anim.fps = pAnimDesc->fps;
			anim.flags = pAnimDesc->flags;
			anim.numframes = pAnimDesc->numframes;
			anim.InitData(seq.numbones, rig, seq.IsAdditive());

			if (!(anim.flags & ANIM_VALID)) {
				seq.anims.push_back(anim);
				continue;
			}

			// anim sections
			uint32_t num_sections = 1;
			int32_t* animsections{};
			if (pAnimDesc->sectionindex) {
				num_sections = GetSectionCount(*pAnimDesc);
				animsections = reinterpret_cast<int32_t*>((char*)pAnimDesc + OFFSET(pAnimDesc->sectionindex));
			}
			uint32_t bfa_size = ((numbones + 3) / 2) & ~1;

			// external data
			std::vector<char> extn_buffer;
			std::string extn_file = path + "_extn";
			if (std::filesystem::exists(extn_file)) {
				std::ifstream extn_stream(extn_file, std::ios::binary);
				extn_buffer.resize(std::filesystem::file_size(extn_file));
				extn_stream.read(extn_buffer.data(), std::filesystem::file_size(extn_file));
				extn_stream.close();
			}

			uint32_t sectionbaseframe = 0;
			for (uint32_t section = 0; section < num_sections; section++) {
				uint32_t sectionframes = GetSectionLength(*pAnimDesc, section, num_sections);

				char* pBoneFlagArray = PTR_FROM_IDX(char, pAnimDesc, OFFSET(pAnimDesc->animindex));
				if (pAnimDesc->sectionindex) {
					if (animsections[section] < 0) {
						int32_t offset = -1 - animsections[section];
						if (offset >= extn_buffer.size()) {
							printf("\n[!] Error: Out of %s range.\n", extn_file.c_str());
							throw std::runtime_error("Out of rseq_extn range.");
						}
						pBoneFlagArray = PTR_FROM_IDX(char, extn_buffer.data(), offset);
					}
					else {
						int32_t offset = animsections[section];
						if (offset >= inputFileSize) {
							printf("\n[!] Error: Out of %s range.\n", seqname.c_str());
							throw std::runtime_error("Out of rseq range.");
						}
						pBoneFlagArray = PTR_FROM_IDX(char, pAnimDesc, offset);
					}
				}

				for (uint32_t localframe = 0; localframe < sectionframes; localframe++) {
					uint32_t frame = sectionbaseframe + localframe;
					auto* pTrack = PTR_FROM_IDX(anim::mstudio_rle_anim_t, pBoneFlagArray, bfa_size);
					for (int bone = 0; bone < numbones; bone++) {
						uint8_t boneFlags = pBoneFlagArray[bone / 2] >> (4 * (bone % 2));

						Vector3& trackpos = anim.animdata[bone].pos[frame];
						Vector3& trackrot = anim.animdata[bone].rot[frame];
						Vector3& trackscl = anim.animdata[bone].scl[frame];
						if (boneFlags & RLE::BONEDATA) {
							auto* pTrackData = PTR_FROM_IDX(uint16_t, pTrack, sizeof(anim::mstudio_rle_anim_t));
							if (boneFlags & RLE::BONEPOS) RLE::CalcBonePosition(*pTrack, &pTrackData, trackpos, localframe);
							if (boneFlags & RLE::BONEROT) RLE::CalcBoneQuaternion(*pTrack, &pTrackData, trackrot, localframe);
							if (boneFlags & RLE::BONESCALE) RLE::CalcBoneScale(*pTrack, &pTrackData, trackscl, localframe);
							pTrack = (anim::mstudio_rle_anim_t*)((char*)pTrack + pTrack->size);
						}
					}
				}
				sectionbaseframe += sectionframes;
			}
			RLE::ParseIkrules_v12(pAnimDesc, anim);
			RLE::ParseFrameMovements_v12(pAnimDesc, anim);
			seq.anims.push_back(anim);
		}
		{
			std::lock_guard<std::mutex> lock(mutex);
			rig.sequences.push_back(std::move(seq));
		}
		std::vector<char>().swap(buffer);
		}));
		if (!_enable_verbose) bar.AddAndPrint();
	}
	for (auto& t : tasks) t.get();
	printf("\n");
}

void WriteRSEQ_v7(temp::rig_t& rig, bool bSkipEvents) {
	ProgressBar bar(rig.sequences.size());
	std::vector<std::future<void>> tasks;
	std::mutex mutex;

	if (!_enable_verbose && !rig.sequences.empty()) bar.Print();
	for (auto& seq : rig.sequences) {
		tasks.push_back(std::async(std::launch::async, [&]() {
			std::vector<char> buffer(8 * 1024 * 1024, 0);
			char* pBase = buffer.data();
			char* pData = pBase;
			temp::StringTable stringTables{};
			stringTables.Init();

			std::filesystem::create_directories(std::filesystem::path(seq.path).parent_path());
			std::ofstream outRseq(seq.path, std::ios::out | std::ios::binary);

			auto* v7RseqDesc = reinterpret_cast<anim::v7::mstudioseqdesc_t*>(pData);
			stringTables.Add(v7RseqDesc, &v7RseqDesc->szlabelindex, seq.name);
			stringTables.Add(v7RseqDesc, &v7RseqDesc->szactivitynameindex, seq.activityname);
			v7RseqDesc->flags = seq.flags;
			v7RseqDesc->activity = seq.activity;
			v7RseqDesc->actweight = seq.actweight;
			v7RseqDesc->bbmin = seq.bbmin;
			v7RseqDesc->bbmax = seq.bbmax;
			v7RseqDesc->numblends = static_cast<uint32_t>(seq.blends.size());
			v7RseqDesc->groupsize[0] = seq.groupsize[0];
			v7RseqDesc->groupsize[1] = seq.groupsize[1];
			v7RseqDesc->paramindex[0] = seq.paramindex[0];
			v7RseqDesc->paramindex[1] = seq.paramindex[1];
			v7RseqDesc->paramstart[0] = seq.paramstart[0];
			v7RseqDesc->paramstart[1] = seq.paramstart[1];
			v7RseqDesc->paramend[0] = seq.paramend[0];
			v7RseqDesc->paramend[1] = seq.paramend[1];
			v7RseqDesc->paramparent = seq.paramparent;
			v7RseqDesc->fadeintime = seq.fadeintime;
			v7RseqDesc->fadeouttime = seq.fadeouttime;
			v7RseqDesc->localentrynode = seq.localentrynode == rig.ignorenode ? 0 : seq.localentrynode;
			v7RseqDesc->localexitnode = seq.localexitnode == rig.ignorenode ? 0 : seq.localexitnode;
			v7RseqDesc->nodeflags = seq.nodeflags;
			v7RseqDesc->entryphase = seq.entryphase;
			v7RseqDesc->exitphase = seq.exitphase;
			v7RseqDesc->lastframe = seq.lastframe;
			v7RseqDesc->nextseq = seq.nextseq;
			v7RseqDesc->pose = seq.pose;
			v7RseqDesc->numautolayers = static_cast<uint32_t>(seq.autolayers.size());
			v7RseqDesc->numikrules = 0;//
			v7RseqDesc->numiklocks = 0;//
			v7RseqDesc->keyvaluesize = 0;//
			v7RseqDesc->cycleposeindex = 0;//
			v7RseqDesc->numactivitymodifiers = static_cast<uint32_t>(seq.actmods.size());
			v7RseqDesc->ikResetMask = seq.ikResetMask;
			v7RseqDesc->unk1 = seq.unk1;
			v7RseqDesc->weightFixupCount = 0;//
			pData += sizeof(anim::v7::mstudioseqdesc_t);

			verbose("%s\n", seq.name.c_str());

			// posekeys
			if (seq.posekeys.size()) {
				v7RseqDesc->posekeyindex = static_cast<uint32_t>(pData - pBase);
				auto* posekeys = reinterpret_cast<float*>(pData);
				for (int i = 0; i < seq.posekeys.size(); i++) {
					posekeys[i] = seq.posekeys[i];
				}
				pData += sizeof(float) * seq.posekeys.size();
			}

			// events
			if (bSkipEvents) {
				for (int i = 0; i < seq.events.size(); i++) {
					if (seq.events[i].name == "AE_CL_CREATE_PROP" || (seq.events[i].name == "AE_CL_DESTROY_PROP") || (seq.events[i].name == "AE_CL_SCRIPT_ANIM_WINDOW_BEGIN") || (seq.events[i].name == "AE_CL_SCRIPT_ANIM_WINDOW_END")) {
						seq.events.erase(seq.events.begin() + i);
						i--;
					}
				}
			}

			v7RseqDesc->numevents = static_cast<uint32_t>(seq.events.size());
			v7RseqDesc->eventindex = static_cast<uint32_t>(pData - pBase);
			if (v7RseqDesc->numevents) {
				auto* v7Events = reinterpret_cast<anim::v7::mstudioevent_t*>(pData);
				for (int i = 0; i < v7RseqDesc->numevents; i++) {
					stringTables.Add(&v7Events[i], &v7Events[i].szeventindex, seq.events[i].name);
					v7Events[i].cycle = seq.events[i].cycle;
					v7Events[i].event = seq.events[i].event;
					v7Events[i].type = seq.events[i].type;
					memcpy_s(v7Events[i].options, 256, seq.events[i].options.c_str(), seq.events[i].options.length());
				}
			}
			pData += sizeof(anim::v7::mstudioevent_t) * v7RseqDesc->numevents;

			// autolayer
			v7RseqDesc->autolayerindex = static_cast<int32_t>(pData - pBase);
			auto* v7Autolayer = reinterpret_cast<anim::v7::mstudioautolayer_t*>(pData);
			for (int i = 0; i < seq.autolayers.size(); i++) {
				v7Autolayer[i].guidSequence = seq.autolayers[i].guidSequence;
				v7Autolayer[i].flags = seq.autolayers[i].flags;
				v7Autolayer[i].iPose = seq.autolayers[i].iPose;
				v7Autolayer[i].iSequence = seq.autolayers[i].iSequence;
				v7Autolayer[i].start = seq.autolayers[i].start;
				v7Autolayer[i].end = seq.autolayers[i].end;
				v7Autolayer[i].peak = seq.autolayers[i].peak;
				v7Autolayer[i].tail = seq.autolayers[i].tail;
			}
			pData += v7RseqDesc->numautolayers * sizeof(anim::v7::mstudioautolayer_t);

			// weightlist
			v7RseqDesc->weightlistindex = static_cast<uint32_t>(pData - pBase);
			auto* v7WeightList = reinterpret_cast<float*>(pData);
			for (int i = 0; i < seq.numbones; i++) {
				v7WeightList[i] = seq.weightlist[i];
			}
			pData += sizeof(float) * seq.numbones;

			// TODO:
			v7RseqDesc->iklockindex = static_cast<uint32_t>(pData - pBase);
			v7RseqDesc->keyvalueindex = static_cast<uint32_t>(pData - pBase);

			// activitymodifiers
			v7RseqDesc->activitymodifierindex = static_cast<uint32_t>(pData - pBase);
			auto* v7Actmod = reinterpret_cast<anim::v7::mstudioactivitymodifier_t*>(pData);
			for (int i = 0; i < seq.actmods.size(); i++) {
				stringTables.Add(&v7Actmod[i], &v7Actmod[i].sznameindex, seq.actmods[i].name);
				v7Actmod[i].negate = seq.actmods[i].negate;
			}
			pData += sizeof(anim::v7::mstudioactivitymodifier_t) * v7RseqDesc->numactivitymodifiers;

			// anims/blends
			std::vector<std::pair<int, int>> blends_index_map;
			v7RseqDesc->animindexindex = static_cast<uint32_t>(pData - pBase);
			auto* v7Blends = reinterpret_cast<int*>(pData);
			pData += sizeof(int) * v7RseqDesc->numblends;
			for (int anim_iter = 0; anim_iter < seq.numuniqueblends; anim_iter++) {
				blends_index_map.push_back({ seq.blends[anim_iter] ,pData - pBase });
				temp::animdesc_t anim = seq.anims[anim_iter];
				anim.SubtractBase(seq.numbones, rig, seq.IsAdditive());
				uint32_t targetsectionframes = 61;

				// anim
				auto* animDesc = reinterpret_cast<anim::v7::mstudioanimdesc_t*>(pData);
				stringTables.Add(animDesc, &animDesc->sznameindex, anim.name);
				animDesc->fps = anim.fps;
				animDesc->flags = anim.flags;
				animDesc->numframes = anim.numframes;
				animDesc->nummovements = 0;
				animDesc->framemovementindex = 0;
				animDesc->numikrules = static_cast<uint32_t>(anim.ikrules.size());
				animDesc->sectionframes = anim.numframes > targetsectionframes ? targetsectionframes : 0;

				pData += sizeof(anim::v7::mstudioanimdesc_t);
				animDesc->animindex = static_cast<int32_t>(pData - (char*)animDesc);

				if (!(anim.flags & ANIM_VALID)) {
					continue;
				}

				// anim sections
				uint32_t numsections = 1;
				uint32_t* animsections{};
				if (anim.numframes > targetsectionframes) {
					animDesc->sectionindex = static_cast<int32_t>(pData - (char*)animDesc);
					numsections = GetSectionCount(*animDesc);
					animsections = PTR_FROM_IDX(uint32_t, pData, 0);
					pData += sizeof(uint32_t) * numsections;
				}

				animDesc->animindex = static_cast<int32_t>(pData - (char*)animDesc);

				uint32_t startframe = 0;
				for (uint32_t section = 0; section < numsections; section++) {
					uint32_t sectionframes = GetSectionLength(*animDesc, section, numsections);
					bool bInterpframe = (section + 1 != numsections);
					uint32_t endframe = startframe + sectionframes + bInterpframe;

					if (numsections > 1) {
						animsections[section] = static_cast<int32_t>(pData - (char*)animDesc);
					}

					// animdata
					uint32_t bfa_size = ((seq.numbones + 3) / 2) & ~1;
					static thread_local std::vector<uint8_t> flaggedBones;
					flaggedBones.clear();
					flaggedBones.resize(bfa_size * 2);
					char* boneflagarray = reinterpret_cast<char*>(pData);
					pData += bfa_size;

					for (int bone = 0; bone < seq.numbones; bone++) {
						uint8_t boneFlags = 0u;
						auto& animData = anim.animdata[bone];

						bool bRawpos = allEqualVector(animData.pos, startframe, endframe);
						bool bRawrot = allEqualVector(animData.rot, startframe, endframe);
						bool bRawscl = allEqualVector(animData.scl, startframe, endframe);

						bool bHasPosData = !bRawpos || (bRawpos && !(animData.pos[startframe].approx_equal({ 0,0,0 })));
						bool bHasRotData = !bRawrot || (bRawrot && !(animData.rot[startframe].approx_equal(rig.bones[bone].rot)));
						bool bHasSclData = !bRawscl || (bRawscl && !(animData.scl[startframe].approx_equal({ 0,0,0 })));

						if (!bHasPosData && !bHasRotData && !bHasSclData) {
							continue;
						}

						// rle / ptr
						auto* animRLE = reinterpret_cast<anim::mstudio_rle_anim_t*>(pData);
						pData += sizeof(anim::mstudio_rle_anim_t);
						anim::studioanimvalue_ptr_t* animposptr{};
						anim::studioanimvalue_ptr_t* animrotptr{};
						anim::studioanimvalue_ptr_t* animsclptr{};

						// posscale
						float posscale = 0.003906369f;
						Vector3 maxpos{};
						Vector3 minpos{};
						findMinMaxSIMD(animData.pos, startframe, endframe, minpos, maxpos);
						float v1 = std::max({ std::fabs(maxpos.Max()), std::fabs(minpos.Min()) });
						if (v1 > 127.) posscale = (v1 * 2.f) / 65534.f;

						// raw pos
						if (bHasPosData) {
							boneFlags |= 0x1;
							if (bRawpos) {
								Vector3 val = animData.pos[startframe];
								if (!seq.IsAdditive()) val += rig.bones[bone].pos;
								*(Vector48*)pData = Pack48(val);
								pData += sizeof(Vector48);
							}
							else {
								animRLE->bAnimPosition = true;
								*(float*)pData = posscale;
								pData += sizeof(float);

								animposptr = reinterpret_cast<anim::studioanimvalue_ptr_t*>(pData);
								pData += sizeof(anim::studioanimvalue_ptr_t);
							}
						}

						// raw rot
						if (bHasRotData) {
							boneFlags |= 0x2;
							if (bRawrot) {
								Quaternion q{};
								AngleQuaternion(animData.rot[startframe], q);
								*(Quaternion64*)pData = PackQuat64(q);
								pData += sizeof(Quaternion64);
							}
							else {
								animRLE->bAnimRotation = true;

								animrotptr = reinterpret_cast<anim::studioanimvalue_ptr_t*>(pData);
								pData += sizeof(anim::studioanimvalue_ptr_t);
							}
						}

						// raw scl
						if (bHasSclData) {
							boneFlags |= 0x4;
							if (bRawscl) {
								Vector3 val = animData.scl[startframe];
								if (!seq.IsAdditive()) val += rig.bones[bone].scl;
								*(Vector48*)pData = Pack48(val);
								pData += sizeof(Vector48);
							}
							else {
								animRLE->bAnimScale = true;

								animsclptr = reinterpret_cast<anim::studioanimvalue_ptr_t*>(pData);
								pData += sizeof(anim::studioanimvalue_ptr_t);
							}
						}

						// Animated pos
						if (bHasPosData && !bRawpos) {
							WriteAnim(pData, animposptr, animData.pos, startframe, endframe, posscale);
						}

						// Animated rot
						if (bHasRotData && !bRawrot) {
							WriteAnim(pData, animrotptr, animData.rot, startframe, endframe, 0.00019175345f);
						}

						// Animated scl
						if (bHasSclData && !bRawscl) {
							WriteAnim(pData, animsclptr, animData.scl, startframe, endframe, 0.0030518509f);
						}

						animRLE->size = pData - (char*)animRLE;
						flaggedBones.at(bone) = boneFlags;

						//boneflagarray (write)
						for (int i = 0; i < (flaggedBones.size()) / 2; i++) {
							boneflagarray[i] = flaggedBones.at(i * 2);
							boneflagarray[i] |= flaggedBones.at(i * 2 + 1) << 4;
						}
					}
					ALIGN4(pData);
					startframe += sectionframes;
				}

				// ikrules
				if (anim.ikrules.size()) {
					int ik_idx = 0;
					v7RseqDesc->numikrules = std::max({ v7RseqDesc->numikrules , (int)anim.ikrules.size() });
					animDesc->numikrules = static_cast<uint32_t>(anim.ikrules.size());
					animDesc->ikruleindex = static_cast<int32_t>(pData - (char*)animDesc);
					auto* v7Ikrule = reinterpret_cast<anim::v7::mstudioikrule_t*>(pData);
					for (ik_idx = 0; ik_idx < anim.ikrules.size(); ik_idx++) {
						temp::ikrule_t& ikrule = anim.ikrules[ik_idx];
						v7Ikrule[ik_idx].index = ikrule.index;
						v7Ikrule[ik_idx].type = ikrule.type;
						v7Ikrule[ik_idx].chain = ikrule.chain;
						v7Ikrule[ik_idx].bone = ikrule.bone;
						v7Ikrule[ik_idx].slot = ikrule.slot;
						v7Ikrule[ik_idx].height = ikrule.height;
						v7Ikrule[ik_idx].radius = ikrule.radius;
						v7Ikrule[ik_idx].pos = ikrule.pos;
						v7Ikrule[ik_idx].q = ikrule.q;
						for (int i = 0; i < 6; i++) v7Ikrule[ik_idx].scale[i] = ikrule.scale[i];
						v7Ikrule[ik_idx].sectionframes = ikrule.sectionframes;
						v7Ikrule[ik_idx].iStart = ikrule.iStart;
						v7Ikrule[ik_idx].start = ikrule.start;
						v7Ikrule[ik_idx].peak = ikrule.peak;
						v7Ikrule[ik_idx].tail = ikrule.tail;
						v7Ikrule[ik_idx].end = ikrule.end;
						v7Ikrule[ik_idx].contact = ikrule.contact;
						v7Ikrule[ik_idx].drop = ikrule.drop;
						v7Ikrule[ik_idx].top = ikrule.top;
						if (ikrule.attachmentname.length())
							stringTables.Add(&v7Ikrule[ik_idx], &v7Ikrule[ik_idx].szattachmentindex, ikrule.attachmentname);
						v7Ikrule[ik_idx].endHeight = ikrule.endHeight;
						pData += sizeof(anim::v7::mstudioikrule_t);
					}

					for (ik_idx = 0; ik_idx < anim.ikrules.size(); ik_idx++) {
						v7Ikrule[ik_idx].compressedikerrorindex = static_cast<int32_t>(pData - (char*)&v7Ikrule[ik_idx]);
						temp::ikrule_t& ikrule = anim.ikrules[ik_idx];
						if (ikrule.sectionframes == 0) continue;

						int32_t sectioncount = static_cast<int32_t>((float)(anim.numframes - 1) / (float)ikrule.sectionframes) + 1;
						int32_t* sectionindices = reinterpret_cast<int32_t*>(pData);
						pData += sizeof(int32_t) * sectioncount;

						Vector3 mn{}, mx{};
						Vector3 mn1{}, mx1{};
						findMinMaxSIMD(ikrule.ikruledata.pos, 0, anim.numframes, mn, mx);
						findMinMaxSIMD(ikrule.ikruledata.rot, 0, anim.numframes, mn1, mx1);

						int idx = 0;
						for (idx = 0; idx < 6; idx++) {
							float v1 = 0;
							if (idx < 3)
								v1 = std::max({ std::fabs(mx[idx]), std::fabs(mn[idx]) });
							else
								v1 = std::max({ std::fabs(mx1[idx - 3]), std::fabs(mn1[idx - 3]) });

							if (v1 > 127.) v7Ikrule[ik_idx].scale[idx] = (v1 * 2.f) / 65534.f;
						}

						startframe = 0;
						for (int section = 0; section < sectioncount; section++) {
							sectionindices[section] = static_cast<int32_t>(pData - (char*)&v7Ikrule[ik_idx]);
							int sectionframes = GetSectionLength(anim.numframes, ikrule.sectionframes, section);

							uint32_t framerange = sectionframes + !(section + 1 == sectioncount);
							uint32_t endframe = startframe + framerange;
							auto* offsets = reinterpret_cast<int16_t*>(pData);
							pData += 6 * sizeof(int16_t);

							for (idx = 0; idx < 3; idx++) {
								offsets[idx] = static_cast<int16_t>(pData - (char*)offsets);
								WriteAnimData(pData, ikrule.ikruledata.pos, startframe, endframe, idx, ikrule.scale[idx]);
							}

							for (idx = 0; idx < 3; idx++) {
								offsets[idx + 3] = static_cast<int16_t>(pData - (char*)offsets);
								WriteAnimData(pData, ikrule.ikruledata.rot, startframe, endframe, idx, ikrule.scale[idx + 3]);
							}
							startframe += sectionframes;
						}
					}
				}

				// movement
				if (anim.flags & r5::ANIM_FRAMEMOVEMENT && (anim.movement.sectionframes != 0)) {
					animDesc->framemovementindex = static_cast<int32_t>(pData - (char*)animDesc);
					auto* frameMovement = reinterpret_cast<anim::v7::mstudioframemovement_t*>(pData);
					frameMovement->scale = anim.movement.scale;
					frameMovement->sectionframes = anim.movement.sectionframes;
					pData += sizeof(anim::v7::mstudioframemovement_t);
					auto& movementdata = anim.movement.movementdata;

					uint32_t sectioncount = static_cast<uint32_t>((float)(anim.numframes - 1) / (float)anim.movement.sectionframes) + 1;
					int32_t* sectionindices = reinterpret_cast<int32_t*>(pData);
					pData += sizeof(uint32_t) * sectioncount;

					Vector4 mn{}, mx{};
					findMinMaxSIMD(movementdata, 0, movementdata.size(), mn, mx);

					int idx = 0;
					for (idx = 0; idx < 4; idx++) {
						float v1 = std::max({ std::fabs(mx[idx]), std::fabs(mn[idx]) });
						if (v1 > 127.) frameMovement->scale[idx] = (v1 * 2.f) / 65534.f;
					}

					startframe = 0;
					for (uint32_t section = 0; section < sectioncount; section++) {
						sectionindices[section] = static_cast<int32_t>(pData - (char*)frameMovement);
						int sectionframes = GetSectionLength(anim.numframes, anim.movement.sectionframes, section);

						uint32_t framerange = sectionframes + !(section + 1 == sectioncount);
						uint32_t endframe = startframe + framerange;
						auto* offsets = reinterpret_cast<int16_t*>(pData);
						pData += 4 * sizeof(int16_t);

						for (idx = 0; idx < 4; idx++) {
							bool bIsAllEqual = allEqualVector(movementdata, startframe, endframe, idx, frameMovement->scale[idx]);
							if (bIsAllEqual && movementdata[startframe][idx] == 0) continue;

							offsets[idx] = static_cast<int16_t>(pData - (char*)offsets);
							WriteAnimData(pData, movementdata, startframe, endframe, idx, frameMovement->scale[idx]);
						}
						startframe += sectionframes;
					}
				}
			}

			// write blends
			for (int iter = 0; iter < v7RseqDesc->numblends; iter++) {
				v7Blends[iter] = blends_index_map[seq.blends[iter]].second;
			}

			v7RseqDesc->weightFixupOffset = static_cast<uint32_t>(pData - pBase);

			//write
			pData = stringTables.Write(pData);
			ALIGN4(pData);
			outRseq.write(pBase, pData - pBase);
			stringTables.Init();
			std::vector<char>().swap(buffer);
			}));
		if (!_enable_verbose) bar.AddAndPrint();
	}
	for (auto& t : tasks) t.get();
	printf("\n");
}