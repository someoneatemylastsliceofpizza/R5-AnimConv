#include <iostream>  
#include <fstream>
#include <vector>
#include <filesystem>
#include <mdl/studio.h>
#include <utils/stringtable.cpp>
#include <utils/print.h>

void ProcessAnimValue(int* read_offset, int* write_offset, char* mdlAnimRle, r5r::studioanimvalue_ptr_t* pRseqValue, int numframe, std::vector<int>& idx_offset, float newScale, float oldScale, float shift, bool isZero) {
	r5r::mstudioanimvalue_t* rseqanimvalue = PTR_FROM_IDX(r5r::mstudioanimvalue_t, g_model.pData, *write_offset);
	idx_offset.push_back(rseqanimvalue - (r5r::mstudioanimvalue_t*)pRseqValue);

	rseqanimvalue[0].meta.valid = 0x0;
	rseqanimvalue[0].meta.total = numframe;
	*write_offset += sizeof(short);

	std::vector<int> read_anim_value;
	if (!isZero) {
		int total_frame = 0;
		while (total_frame < numframe) {
			r5r::mstudioanimvalue_t* animvalue = PTR_FROM_IDX(r5r::mstudioanimvalue_t, mdlAnimRle, *read_offset);
			for (int j = 0; j < animvalue->meta.valid; j++)
				read_anim_value.push_back(animvalue[j + 1].value);
			for (int j = 0; j < animvalue->meta.total - animvalue->meta.valid; j++)
				read_anim_value.push_back(animvalue[animvalue->meta.valid].value);
			*read_offset += animvalue->meta.valid * 2 + 2;
			total_frame += animvalue->meta.total;
		}
	}
	else {
		read_anim_value.resize(numframe, 0);
	}

	for (int j = 0; j < numframe; j++) {
		if (!isZero) {
			rseqanimvalue[j + 1].value = ((double)read_anim_value.at(j) * oldScale + shift) / newScale;
		}
		else {
			rseqanimvalue[j + 1].value = shift / newScale;
		}
		*write_offset += sizeof(short);
	}
}

std::vector<int> getAnimArray(int* read_offset, char* mdlAnimRle, int numframe, bool isZero) {
	std::vector<int> read_anim_value;
	if (!isZero) {
		int total_frame = 0;
		while (total_frame < numframe) {
			r5r::mstudioanimvalue_t* animvalue = PTR_FROM_IDX(r5r::mstudioanimvalue_t, mdlAnimRle, *read_offset);
			for (int j = 0; j < animvalue->meta.valid; j++)
				read_anim_value.push_back(animvalue[j + 1].value);
			for (int j = 0; j < animvalue->meta.total - animvalue->meta.valid; j++)
				read_anim_value.push_back(animvalue[animvalue->meta.valid].value);
			*read_offset += animvalue->meta.valid * 2 + 2;
			total_frame += animvalue->meta.total;
		}
	}
	else {
		read_anim_value.resize(numframe, 0);
	}
	return read_anim_value;
}

void ProcessArrayAnimValue(std::vector<int>& anim_array, size_t* write_offset, r5r::studioanimvalue_ptr_t* pRseqValue, int numframes, std::vector<int>& idx_offset, float newScale, float oldScale, float shift, bool isZero) {
	int totalFrames = numframes;
	bool isLastSection = false;
	bool gotIndexOffset = false;
	size_t frameCounter = 0;
	int n = totalFrames / 255 + 1; // max 0xFF frames
	for (int i = 0; i < n; i++) {
		if (n > 1 && ((i + 1) < n)) {
				numframes = 255;
		}
		if ((i + 1) == n) {
			numframes = totalFrames - i * 255;
			isLastSection = true;
		}

		r5r::mstudioanimvalue_t* rseqanimvalue = PTR_FROM_IDX(r5r::mstudioanimvalue_t, g_model.pData, *write_offset);
		if(!gotIndexOffset){
			idx_offset.push_back(rseqanimvalue - (r5r::mstudioanimvalue_t*)pRseqValue);
			gotIndexOffset = true;
		}

		rseqanimvalue[0].meta.valid = 0x0;
		rseqanimvalue[0].meta.total = numframes + isLastSection;
		*write_offset += sizeof(short);

		for (int j = 0; j < numframes; j++) {
			if (!isZero) {
				rseqanimvalue[j + 1].value = ((double)anim_array.at(j) * oldScale + shift) / newScale;
			} else {
				rseqanimvalue[j + 1].value = shift / newScale;
			}
			*write_offset += sizeof(short);
		}
		frameCounter += numframes;
		if (isLastSection) {
			rseqanimvalue[numframes + 1].value = ((double)anim_array.at(frameCounter - 1) * oldScale + shift) / newScale;
			*write_offset += sizeof(short);
			frameCounter += 1;
		}
	}
	anim_array.erase(anim_array.begin(), anim_array.begin() + totalFrames);
}

std::vector<std::string> ConvertMDL_49_RSEQ(char* mdl_buffer, std::string output_dir, std::string filename, std::string override_path, std::vector<std::pair<std::pair<int, int>, int>>& nodedata) {
	pt2::studiohdr_t* pV49MdlHdr = reinterpret_cast<pt2::studiohdr_t*>(mdl_buffer);
	pt2::mstudioseqdesc_t* pStudioSeqDesc = reinterpret_cast<pt2::mstudioseqdesc_t*>((mdl_buffer + pV49MdlHdr->localseqindex));
	pt2::mstudioanimdesc_t* pV49AnimDesc = reinterpret_cast<pt2::mstudioanimdesc_t*>((mdl_buffer + pV49MdlHdr->localanimindex));

	pt2::mstudiolinearbone_t* linearboneindexes = PTR_FROM_IDX(pt2::mstudiolinearbone_t, mdl_buffer, pV49MdlHdr->srcbonetransformindex);
	Vector3* studioPosScale = PTR_FROM_IDX(Vector3, linearboneindexes, linearboneindexes->posscaleindex);
	Vector3* studioRotScale = PTR_FROM_IDX(Vector3, linearboneindexes, linearboneindexes->rotscaleindex);
	Vector3* studioRot = PTR_FROM_IDX(Vector3, linearboneindexes, linearboneindexes->rotindex);

	std::vector<std::string> sequence_names;

	for (int seq_idx = 0; seq_idx < pV49MdlHdr->numlocalseq; seq_idx++) {
		print("[%3d/%3d] ", seq_idx + 1, pV49MdlHdr->numlocalseq);

		int base_ptr = pStudioSeqDesc[seq_idx].baseptr;

		std::string seq_dir, seqdescname;
		std::string raw_seq_name = STRING_FROM_IDX(reinterpret_cast<char*>(mdl_buffer), -base_ptr + pStudioSeqDesc[seq_idx].sznameindex);
		std::replace(raw_seq_name.begin(), raw_seq_name.end(), '\\', '/');
		std::replace(override_path.begin(), override_path.end(), '/', '\\');
		std::string seq_name = (raw_seq_name.rfind('/') != std::string::npos) ? raw_seq_name.substr(raw_seq_name.rfind('/') + 1, raw_seq_name.size()) + ".rseq" : raw_seq_name + ".rseq";

		// override path
		if (!override_path.empty()) {
			seq_dir = override_path;
			seqdescname = override_path + (override_path.back() == '\\' ? "" : "\\") + seq_name;
		}
		else {
			seq_dir = (raw_seq_name.rfind('/') != std::string::npos) ? raw_seq_name.substr(0, raw_seq_name.rfind('/')) + '/' : "";
			seqdescname = seq_dir + seq_name;
		}

		// create directory
		std::replace(seq_dir.begin(), seq_dir.end(), '/', '\\');
		std::filesystem::create_directories(output_dir + "\\" + seq_dir);

		// write rseq file
		std::string output_path = output_dir + "/" + seqdescname;
		std::replace(output_path.begin(), output_path.end(), '/', '\\');
		std::ofstream outRseq(output_path, std::ios::out | std::ios::binary);

		std::replace(seqdescname.begin(), seqdescname.end(), '\\', '/');

#ifdef _DEBUG
		print("\n    ->%s\n", seqdescname.c_str());
#else
		print(" %s", seqdescname.c_str());
		print("...  ");
#endif

		if (seqdescname != "ref.rseq")
			sequence_names.push_back(seqdescname);

		if (pStudioSeqDesc[seq_idx].localentrynode != pStudioSeqDesc[seq_idx].localexitnode)
			nodedata.push_back({ {pStudioSeqDesc[seq_idx].localentrynode , pStudioSeqDesc[seq_idx].localexitnode}, seq_idx - 1 /*doesn't include ref.rseq*/ });

		//header
		g_model.pBase = new char[32 * 1024 * 1024] {};
		g_model.pData = g_model.pBase;
		r5r::mstudioseqdesc_t* pV7RseqDesc = reinterpret_cast<r5r::mstudioseqdesc_t*>(g_model.pData);
		pV7RseqDesc->flags = pStudioSeqDesc[seq_idx].flags;
		pV7RseqDesc->activity = pStudioSeqDesc[seq_idx].activity;
		pV7RseqDesc->actweight = pStudioSeqDesc[seq_idx].actweight;
		pV7RseqDesc->numevents = pStudioSeqDesc[seq_idx].numevents;
		pV7RseqDesc->bbmin = pStudioSeqDesc[seq_idx].bbmin;
		pV7RseqDesc->bbmax = pStudioSeqDesc[seq_idx].bbmax;
		pV7RseqDesc->numblends = pStudioSeqDesc[seq_idx].numblends;
		pV7RseqDesc->groupsize[0] = pStudioSeqDesc[seq_idx].groupsize[0];
		pV7RseqDesc->groupsize[1] = pStudioSeqDesc[seq_idx].groupsize[1];
		pV7RseqDesc->paramindex[0] = pStudioSeqDesc[seq_idx].paramindex[0];
		pV7RseqDesc->paramindex[1] = pStudioSeqDesc[seq_idx].paramindex[1];
		pV7RseqDesc->paramstart[0] = pStudioSeqDesc[seq_idx].paramstart[0];
		pV7RseqDesc->paramstart[1] = pStudioSeqDesc[seq_idx].paramstart[1];
		pV7RseqDesc->paramend[0] = pStudioSeqDesc[seq_idx].paramend[0] ? pStudioSeqDesc[seq_idx].paramend[0] : 1;
		pV7RseqDesc->paramend[1] = pStudioSeqDesc[seq_idx].paramend[1] ? pStudioSeqDesc[seq_idx].paramend[1] : 1;
		pV7RseqDesc->paramparent = pStudioSeqDesc[seq_idx].paramparent;
		pV7RseqDesc->fadeintime = pStudioSeqDesc[seq_idx].fadeintime;
		pV7RseqDesc->fadeouttime = pStudioSeqDesc[seq_idx].fadeouttime;
		pV7RseqDesc->localentrynode = pStudioSeqDesc[seq_idx].localentrynode;
		pV7RseqDesc->localexitnode = pStudioSeqDesc[seq_idx].localexitnode;
		pV7RseqDesc->entryphase = pStudioSeqDesc[seq_idx].entryphase;
		pV7RseqDesc->exitphase = pStudioSeqDesc[seq_idx].exitphase;
		pV7RseqDesc->numikrules = 1;
		g_model.pHdr = pV7RseqDesc;
		g_model.pData += sizeof(r5r::mstudioseqdesc_t);
		BeginStringTable();

		AddToStringTable((char*)pV7RseqDesc, &pV7RseqDesc->szlabelindex, seqdescname.c_str());
		AddToStringTable((char*)pV7RseqDesc, &pV7RseqDesc->szactivitynameindex, STRING_FROM_IDX(reinterpret_cast<char*>(mdl_buffer), pStudioSeqDesc[seq_idx].szactivitynameindex - base_ptr));

		//posekey 
		if (pStudioSeqDesc[seq_idx].posekeyindex) {
			int numposekey = pV7RseqDesc->groupsize[0] + pV7RseqDesc->groupsize[1];
			pV7RseqDesc->posekeyindex = g_model.pData - g_model.pBase;

			float* studioPoseKey = PTR_FROM_IDX(float, &pStudioSeqDesc[seq_idx], pStudioSeqDesc[seq_idx].posekeyindex);
			float* posekey = reinterpret_cast<float*>(g_model.pData);
			for (int i = 0; i < numposekey; i++) {
				posekey[i] = studioPoseKey[i];
			}
			g_model.pData += sizeof(float) * (numposekey);
		}

		//events
		pV7RseqDesc->eventindex = g_model.pData - g_model.pBase;
		r5r::mstudioevent_t* rseqEvent = reinterpret_cast<r5r::mstudioevent_t*>(g_model.pData);
		pt2::mstudioevent_t* studioEvent = reinterpret_cast<pt2::mstudioevent_t*>(mdl_buffer - base_ptr + pStudioSeqDesc[seq_idx].eventindex);
		for (int i = 0; i < pV7RseqDesc->numevents; i++) {
			rseqEvent[i].cycle = studioEvent[i].cycle;
			rseqEvent[i].event = studioEvent[i].event;
			rseqEvent[i].type = studioEvent[i].type;
			memcpy_s(rseqEvent[i].options, 256, studioEvent[i].options, 64);
			AddToStringTable((char*)&rseqEvent[i], &rseqEvent[i].szeventindex, STRING_FROM_IDX(&studioEvent[i], studioEvent[i].szeventindex));
		}
		g_model.pData += sizeof(r5r::mstudioevent_t) * pV7RseqDesc->numevents;

		//autolayer
		if (pStudioSeqDesc[seq_idx].numautolayers) {
			pV7RseqDesc->numautolayers = pStudioSeqDesc[seq_idx].numautolayers;
			pV7RseqDesc->autolayerindex = g_model.pData - g_model.pBase;
			pt2::mstudioautolayer_t* v49AutoLayer = reinterpret_cast<pt2::mstudioautolayer_t*>(mdl_buffer - base_ptr + pStudioSeqDesc[seq_idx].autolayerindex);
			r5r::mstudioautolayer_t* v54AutoLayer = reinterpret_cast<r5r::mstudioautolayer_t*>(g_model.pData);
			for (int i = 0; i < pV7RseqDesc->numautolayers; i++) {
				std::string n = std::string(STRING_FROM_IDX(reinterpret_cast<char*>(mdl_buffer), -pStudioSeqDesc[v49AutoLayer[i].iSequence].baseptr + pStudioSeqDesc[v49AutoLayer[i].iSequence].sznameindex)) + ".rseq";
				std::replace(n.begin(), n.end(), '\\', '/');
				n = n.substr(n.find_last_of('/') + 1);
				if (!override_path.empty()) {
					n = override_path + (override_path.back() == '\\' ? "" : "\\") + n;
				}
				std::replace(n.begin(), n.end(), '\\', '/');
				v54AutoLayer[i].assetSequence = StringToGuid(n.c_str());
				v54AutoLayer[i].iSequence = v49AutoLayer[i].iSequence + sequence_names.size() - seq_idx - 1; //
				v54AutoLayer[i].iPose = v49AutoLayer[i].iPose;
				v54AutoLayer[i].flags = v49AutoLayer[i].flags;
				v54AutoLayer[i].start = v49AutoLayer[i].start;
				v54AutoLayer[i].peak = v49AutoLayer[i].peak;
				v54AutoLayer[i].tail = v49AutoLayer[i].tail;
				v54AutoLayer[i].end = v49AutoLayer[i].end;
			}
			g_model.pData += sizeof(r5r::mstudioautolayer_t) * pV7RseqDesc->numautolayers;
		}

		//weight list
		pV7RseqDesc->weightlistindex = g_model.pData - g_model.pBase;
		float* mdl_weight = reinterpret_cast<float*>(mdl_buffer - base_ptr + pStudioSeqDesc[seq_idx].weightlistindex);
		float* rseq_weight = reinterpret_cast<float*>(g_model.pData);
		for (int i = 0; i < pV49MdlHdr->numbones; i++) {
			rseq_weight[i] = mdl_weight[i];
		}
		g_model.pData += sizeof(int) * pV49MdlHdr->numbones;

		//activity modifiers
		pV7RseqDesc->activitymodifierindex = g_model.pData - g_model.pBase;
		pV7RseqDesc->numactivitymodifiers = pStudioSeqDesc[seq_idx].numactivitymodifiers;
		int* mstudioActivityModifier = PTR_FROM_IDX(int, &pStudioSeqDesc[seq_idx], pStudioSeqDesc[seq_idx].activitymodifierindex);
		r5r::mstudioactivitymodifier_t* rseqActivityModifier = reinterpret_cast<r5r::mstudioactivitymodifier_t*>(g_model.pData);
		std::vector<std::string> activity_name{};
		for (int i = 0; i < pV7RseqDesc->numactivitymodifiers; i++) {
			std::string tmp = STRING_FROM_IDX(&mstudioActivityModifier[i], mstudioActivityModifier[i]); //
			if (tmp.size() >= 2 && tmp.compare(tmp.size() - 2, 2, "_1") == 0) {						    // if activitymodifier is negate add _1 at the end of the name
				tmp.erase(tmp.size() - 2, 2);															// eg. activitymodifier crouch_1
				rseqActivityModifier[i].negate = 1;														//
			}
			else {
				rseqActivityModifier[i].negate = 0;
			}
			activity_name.push_back(tmp);
		}
		for (int i = 0; i < pV7RseqDesc->numactivitymodifiers; i++) {
			AddToStringTable((char*)&rseqActivityModifier[i], &rseqActivityModifier[i].sznameindex, activity_name.at(i).c_str());
		}
		g_model.pData += sizeof(r5r::mstudioactivitymodifier_t) * pV7RseqDesc->numactivitymodifiers;


		//blends
		pV7RseqDesc->animindexindex = g_model.pData - g_model.pBase;
		short* studio_blends = PTR_FROM_IDX(short, &pStudioSeqDesc[seq_idx], pStudioSeqDesc[seq_idx].animindexindex);
		int* rseq_blends = reinterpret_cast<int*>(g_model.pData);
		g_model.pData += sizeof(int) * pV7RseqDesc->numblends;

		for (int blend_idx = 0; blend_idx < pV7RseqDesc->numblends; blend_idx++) {
			//animdesc 
			short anim_idx = studio_blends[blend_idx];
			r5r::mstudioanimdesc_t* rAnimDesc = reinterpret_cast<r5r::mstudioanimdesc_t*>(g_model.pData);
			char* anim_name = (char*)STRING_FROM_IDX(mdl_buffer, -pV49AnimDesc[anim_idx].baseptr + pV49AnimDesc[anim_idx].sznameindex);
			rseq_blends[blend_idx] = g_model.pData - g_model.pBase;
			rAnimDesc->fps = pV49AnimDesc[anim_idx].fps;
			rAnimDesc->flags = (pV49AnimDesc[anim_idx].flags & 0x20 ? 0x0 : 0x20000) | pV49AnimDesc[anim_idx].flags;
			rAnimDesc->numframes = pV49AnimDesc[anim_idx].numframes;
			AddToStringTable(reinterpret_cast<char*>(rAnimDesc), &rAnimDesc->sznameindex, anim_name);
			g_model.pData += sizeof(r5r::mstudioanimdesc_t);
			ALIGN16(g_model.pData);

			//anims
			pt2::mstudioanimsections_t* animsections = nullptr;
			unsigned int* sections_index{};
			int animbase_ptr = pV49AnimDesc[anim_idx].baseptr;
			int num_frames = pV49AnimDesc[anim_idx].numframes;
			bool hasSections = false;

#ifdef _DEBUG
			print("      L-> %s (%d)\n", anim_name, pV49AnimDesc[anim_idx].sectionframes > 0 ? (pV49AnimDesc[anim_idx].numframes / pV49AnimDesc[anim_idx].sectionframes) + 1 : 0);
#endif

			if (rAnimDesc->flags & 0x20) {
				rAnimDesc->animindex = g_model.pData - (char*)rAnimDesc;
				continue;
			}

			size_t num_sections = 1;
			if (pV49AnimDesc[anim_idx].sectionindex) {
				hasSections = true;
				num_sections = (pV49AnimDesc[anim_idx].numframes / pV49AnimDesc[anim_idx].sectionframes) + 2;
				animsections = reinterpret_cast<pt2::mstudioanimsections_t*>(mdl_buffer - animbase_ptr + pV49AnimDesc[anim_idx].sectionindex);
				sections_index = reinterpret_cast<unsigned int*>(g_model.pData);
				g_model.pData += 4 * num_sections;
			}


			rAnimDesc->animindex = g_model.pData - (char*)rAnimDesc;
			rAnimDesc->sectionindex = hasSections ? rAnimDesc->animindex - ((num_sections) * 4) : 0;
			rAnimDesc->sectionframes = pV49AnimDesc[anim_idx].sectionframes;

			for (size_t section = 0; section < num_sections; section++) {
				if (hasSections) {
					sections_index[section] = g_model.pData - (char*)rAnimDesc;
					num_frames = pV49AnimDesc[anim_idx].sectionframes + 1;
					if (section == num_sections - 1) {
						num_frames = pV49AnimDesc[anim_idx].numframes - ((num_sections - 2) * pV49AnimDesc[anim_idx].sectionframes + 1);
					}
				}

				//boneflagarray (allocate)
				char* boneflagarray = reinterpret_cast<char*>(g_model.pData);
				std::vector<unsigned int> flaggedbones(pV49MdlHdr->numbones + 1, 0);
				g_model.pData += ((pV49MdlHdr->numbones + 3) / 2) % 2 == 1 ? (pV49MdlHdr->numbones + 3) / 2 - 1 : (pV49MdlHdr->numbones + 3) / 2;

				//animvalue
				int anim_block_offset = 0;
				for (int i = 0; i < pV49MdlHdr->numbones; i++) {
					pt2::mstudio_rle_anim_t* mdlAnimRle{};

					if (hasSections) {
						mdlAnimRle = reinterpret_cast<pt2::mstudio_rle_anim_t*>(mdl_buffer - animbase_ptr + animsections[section].animindex + anim_block_offset);
					}
					else {
						mdlAnimRle = reinterpret_cast<pt2::mstudio_rle_anim_t*>(mdl_buffer - animbase_ptr + pV49AnimDesc[anim_idx].animindex + anim_block_offset);
					}

					r5r::mstudio_rle_anim_t* rseqAnimRle = reinterpret_cast<r5r::mstudio_rle_anim_t*>(g_model.pData);
					rseqAnimRle->flags = 0;
					int read_offset = sizeof(pt2::mstudio_rle_anim_t);
					int write_offset = sizeof(r5r::mstudio_rle_anim_t);
					int flags = 0;
					int tmp_r, tmp_p;

					// skipped the broken data
					if (mdlAnimRle->bone > pV49MdlHdr->numbones) {
						rAnimDesc->flags = 0x20;
						g_model.pData = g_model.pData - rAnimDesc->animindex;
						break;
					}

					r5r::studioanimvalue_ptr_t* pRseqRotV{};
					r5r::studioanimvalue_ptr_t* pRseqPosV{};
					pt2::studioanimvalue_ptr_t* pMdlRotV{};
					pt2::studioanimvalue_ptr_t* pMdlPosV{};

					//posscale
					if (mdlAnimRle->flags & R5_ANIM_ANIMPOS) {
						float* posscale = PTR_FROM_IDX(float, g_model.pData, write_offset);
						*posscale = studioPosScale[mdlAnimRle->bone].Min();
						write_offset += 4;
					}
					//raw data
					if (mdlAnimRle->flags & R5_ANIM_RAWPOS) {
						Vector48* rseqRawpos = PTR_FROM_IDX(Vector48, g_model.pData, write_offset);
						Vector48* mdlRawpos = PTR_FROM_IDX(Vector48, mdlAnimRle, ((mdlAnimRle->flags & R5_ANIM_RAWROT) ? 12 : read_offset));
						rseqRawpos->x = mdlRawpos->x;
						rseqRawpos->y = mdlRawpos->y;
						rseqRawpos->z = mdlRawpos->z;
						flags |= 0x1;
						write_offset += 6;
					}

					if (mdlAnimRle->flags & R5_ANIM_RAWROT) {
						Quaternion64* rseqRawrot = PTR_FROM_IDX(Quaternion64, g_model.pData, write_offset);
						Quaternion64* mdlRawrot = PTR_FROM_IDX(Quaternion64, mdlAnimRle, read_offset);
						rseqRawrot->x = mdlRawrot->x;
						rseqRawrot->y = mdlRawrot->y;
						rseqRawrot->z = mdlRawrot->z;
						rseqRawrot->wneg = mdlRawrot->wneg;
						flags |= 0x2;
						write_offset += 8;
					}

					//animvalue_ptr
					if (mdlAnimRle->flags & R5_ANIM_ANIMROT) {
						pMdlRotV = PTR_FROM_IDX(pt2::studioanimvalue_ptr_t, mdlAnimRle, read_offset);
						tmp_r = read_offset;
						read_offset += sizeof(pt2::studioanimvalue_ptr_t);
					}
					if (mdlAnimRle->flags & R5_ANIM_ANIMPOS) {
						pMdlPosV = PTR_FROM_IDX(pt2::studioanimvalue_ptr_t, mdlAnimRle, read_offset);
						tmp_p = read_offset;
						read_offset += sizeof(pt2::studioanimvalue_ptr_t);
					}


					if (mdlAnimRle->flags & R5_ANIM_ANIMPOS && !pMdlPosV->IsAllZero()) {
						pRseqPosV = PTR_FROM_IDX(r5r::studioanimvalue_ptr_t, g_model.pData, write_offset);
						write_offset += sizeof(r5r::studioanimvalue_ptr_t);
						flags |= 0x1;
					}

					if (mdlAnimRle->flags & R5_ANIM_ANIMROT && !pMdlRotV->IsAllZero()) {
						pRseqRotV = PTR_FROM_IDX(r5r::studioanimvalue_ptr_t, g_model.pData, write_offset);
						write_offset += sizeof(r5r::studioanimvalue_ptr_t);
						flags |= 0x2;
					}

					//write animvalue
					//animpos
					std::vector<int> idx_offset;
					if (mdlAnimRle->flags & R5_ANIM_ANIMPOS && !pMdlPosV->IsAllZero()) {
						rseqAnimRle->flags |= 0x4;
						//pos x
						pRseqPosV->flags |= 4;
						read_offset = tmp_p + pMdlPosV->x;
						ProcessAnimValue(&read_offset, &write_offset, (char*)mdlAnimRle, pRseqPosV, num_frames, idx_offset, studioPosScale[mdlAnimRle->bone].Min(), studioPosScale[mdlAnimRle->bone].x, 0, !pMdlPosV->x);
						//pos y
						pRseqPosV->flags |= 2;
						read_offset = tmp_p + pMdlPosV->y;
						ProcessAnimValue(&read_offset, &write_offset, (char*)mdlAnimRle, pRseqPosV, num_frames, idx_offset, studioPosScale[mdlAnimRle->bone].Min(), studioPosScale[mdlAnimRle->bone].y, 0, !pMdlPosV->y);
						//pos z
						pRseqPosV->flags |= 1;
						read_offset = tmp_p + pMdlPosV->z;
						ProcessAnimValue(&read_offset, &write_offset, (char*)mdlAnimRle, pRseqPosV, num_frames, idx_offset, studioPosScale[mdlAnimRle->bone].Min(), studioPosScale[mdlAnimRle->bone].z, 0, !pMdlPosV->z);

						pRseqPosV->offset = idx_offset.at(0) * 2;
						pRseqPosV->idx1 = MAX(idx_offset.at(1) - idx_offset.at(0), 0);
						pRseqPosV->idx2 = MAX(idx_offset.at(2) - idx_offset.at(0), 0);

					}

					//animrot
					idx_offset.clear();
					if (mdlAnimRle->flags & R5_ANIM_ANIMROT && !pMdlRotV->IsAllZero()) {
						rseqAnimRle->flags |= 0x2;
						//rot x
						pRseqRotV->flags |= 4;
						read_offset = tmp_r + pMdlRotV->x;
						ProcessAnimValue(&read_offset, &write_offset, (char*)mdlAnimRle, pRseqRotV, num_frames, idx_offset, 0.00019175345f, studioRotScale[mdlAnimRle->bone].x, rAnimDesc->flags & 0x4 ? 0 : studioRot[mdlAnimRle->bone].x, !pMdlRotV->x);
						//rot y
						pRseqRotV->flags |= 2;
						read_offset = tmp_r + pMdlRotV->y;
						ProcessAnimValue(&read_offset, &write_offset, (char*)mdlAnimRle, pRseqRotV, num_frames, idx_offset, 0.00019175345f, studioRotScale[mdlAnimRle->bone].y, rAnimDesc->flags & 0x4 ? 0 : studioRot[mdlAnimRle->bone].y, !pMdlRotV->y);
						//rot z
						pRseqRotV->flags |= 1;
						read_offset = tmp_r + pMdlRotV->z;
						ProcessAnimValue(&read_offset, &write_offset, (char*)mdlAnimRle, pRseqRotV, num_frames, idx_offset, 0.00019175345f, studioRotScale[mdlAnimRle->bone].z, rAnimDesc->flags & 0x4 ? 0 : studioRot[mdlAnimRle->bone].z, !pMdlRotV->z);

						pRseqRotV->offset = idx_offset.at(0) * 2;
						pRseqRotV->idx1 = MAX(idx_offset.at(1) - idx_offset.at(0), 0);
						pRseqRotV->idx2 = MAX(idx_offset.at(2) - idx_offset.at(0), 0);
					}

					flaggedbones.at(mdlAnimRle->bone) = flags;
					rseqAnimRle->size = write_offset;
					g_model.pData += write_offset;
					anim_block_offset += mdlAnimRle->nextoffset;

					if (mdlAnimRle->nextoffset == 0) break;
				}

				//boneflagarray (write)
				for (int i = 0; i < (flaggedbones.size()) / 2; i++) {
					boneflagarray[i] = flaggedbones.at(i * 2);
					boneflagarray[i] |= flaggedbones.at(i * 2 + 1) << 4;
				}
			}

			//TODO:
			//ikrules
			rAnimDesc->numikrules = pV49AnimDesc[anim_idx].numikrules;
			rAnimDesc->ikruleindex = g_model.pData - g_model.pBase - rseq_blends[blend_idx];
			pt2::mstudioikrule_t* v49ikrule = reinterpret_cast<pt2::mstudioikrule_t*>(mdl_buffer - animbase_ptr + pV49AnimDesc[anim_idx].ikruleindex);
			r5r::mstudioikrule_t* ikrule = reinterpret_cast<r5r::mstudioikrule_t*>(g_model.pData);
			for (int i = 0; i < rAnimDesc->numikrules; i++) {
				ikrule[i].chain = v49ikrule[i].chain;
				ikrule[i].type = 4; //v53ikrule[i].type;
				ikrule[i].bone = v49ikrule[i].bone;
				ikrule[i].slot = v49ikrule[i].slot;
				ikrule[i].height = v49ikrule[i].height;
				ikrule[i].radius = v49ikrule[i].radius;
				ikrule[i].floor = v49ikrule[i].floor;
				ikrule[i].pos = v49ikrule[i].pos;
				ikrule[i].q = v49ikrule[i].q;
				ikrule[i].start = v49ikrule[i].start;
				ikrule[i].peak = v49ikrule[i].peak;
				ikrule[i].tail = v49ikrule[i].tail;
				ikrule[i].end = v49ikrule[i].end;
				ikrule[i].contact = v49ikrule[i].contact;
				ikrule[i].drop = v49ikrule[i].drop;
				ikrule[i].szattachmentindex = 0; //
				//ikrule[i].endHeight = v49ikrule[i].;
			}
			g_model.pData += sizeof(r5r::mstudioikrule_t) * rAnimDesc->numikrules;

			//TODO:
			for (int i = 0; i < rAnimDesc->numikrules; i++) {
				//ikrule[i].compressedikerrorindex = g_model.pData - (char*)&ikrule[i];
				//ikrule frames...
			}

			// movement
			//r5r::mstudiomovement_t* movementdata = reinterpret_cast<r5r::mstudiomovement_t*>(rAnimDesc->movementindex);
			//g_model.pData += sizeof(r5r::mstudiomovement_t) * Desc->;
		}

		//unk
		pV7RseqDesc->unkOffset = g_model.pData - g_model.pBase;

		//write all data
		g_model.pData = WriteStringTable(g_model.pData);
		ALIGN4(g_model.pData);

		outRseq.write(g_model.pBase, g_model.pData - g_model.pBase);
		g_model.stringTable.clear();
		delete[] g_model.pBase;

		print("Done!\n");
	}
	return sequence_names;
}

std::vector<std::string> ConvertMDL_53_RSEQ(char* mdl_buffer, std::string output_dir, std::string filename, std::string override_path, std::vector<std::pair<std::pair<int, int>, int>>& nodedata) {
	tf2::studiohdr_t* pV53MdlHdr = reinterpret_cast<tf2::studiohdr_t*>(mdl_buffer);
	tf2::mstudioseqdesc_t* pV53SeqDesc = reinterpret_cast<tf2::mstudioseqdesc_t*>((mdl_buffer + pV53MdlHdr->localseqindex));
	tf2::mstudioanimdesc_t* pV53AnimDesc = reinterpret_cast<tf2::mstudioanimdesc_t*>((mdl_buffer + pV53MdlHdr->localanimindex));

	pt2::mstudiolinearbone_t* linearboneindexes = PTR_FROM_IDX(pt2::mstudiolinearbone_t, mdl_buffer, pV53MdlHdr->linearboneindex);
	Vector3* studioRotScale = PTR_FROM_IDX(Vector3, linearboneindexes, linearboneindexes->rotscaleindex);
	Vector3* studioPos = PTR_FROM_IDX(Vector3, linearboneindexes, linearboneindexes->posindex);
	Vector3* studioRot = PTR_FROM_IDX(Vector3, linearboneindexes, linearboneindexes->rotindex);

	std::vector<std::string> sequence_names;

	for (int seq_idx = 0; seq_idx < pV53MdlHdr->numlocalseq; seq_idx++) {
		print("[%3d/%3d] ", seq_idx + 1, pV53MdlHdr->numlocalseq);

		int base_ptr = pV53SeqDesc[seq_idx].baseptr;

		std::string seq_dir, seqdescname;
		std::string raw_seq_name = STRING_FROM_IDX(reinterpret_cast<char*>(mdl_buffer), -base_ptr + pV53SeqDesc[seq_idx].szlabelindex);
		std::replace(raw_seq_name.begin(), raw_seq_name.end(), '\\', '/');
		std::replace(override_path.begin(), override_path.end(), '/', '\\');
		std::string seq_name = (raw_seq_name.rfind('/') != std::string::npos) ? raw_seq_name.substr(raw_seq_name.rfind('/') + 1, raw_seq_name.size()) + ".rseq" : raw_seq_name + ".rseq";


		//n = override_path + (override_path.back() == '\\' ? "" : "\\") + n;

		// override path
		if (!override_path.empty()) {
			seq_dir = override_path;
			seqdescname = override_path + (override_path.back() == '\\' ? "" : "\\") + seq_name;
		}
		else {
			seq_dir = (raw_seq_name.rfind('/') != std::string::npos) ? raw_seq_name.substr(0, raw_seq_name.rfind('/')) + '/' : "";
			seqdescname = seq_dir + seq_name;
		}

		// create directory
		std::replace(seq_dir.begin(), seq_dir.end(), '/', '\\');
		std::filesystem::create_directories(output_dir + "\\" + seq_dir);

		// write rseq file
		std::string output_path = output_dir + "/" + seqdescname;
		std::replace(output_path.begin(), output_path.end(), '/', '\\');
		std::ofstream outRseq(output_path, std::ios::out | std::ios::binary);

		std::replace(seqdescname.begin(), seqdescname.end(), '\\', '/');

#ifdef _DEBUG
		print("\n    ->%s\n", seqdescname.c_str());
#else
		print(" %s", seqdescname.c_str());
		print("...  ");
#endif

		if (seqdescname != "ref.rseq")
			sequence_names.push_back(seqdescname);

		if (pV53SeqDesc[seq_idx].localentrynode != pV53SeqDesc[seq_idx].localexitnode)
			nodedata.push_back({ {pV53SeqDesc[seq_idx].localentrynode , pV53SeqDesc[seq_idx].localexitnode}, seq_idx - 1 /*doesn't include ref.rseq*/ });

		//header
		g_model.pBase = new char[32 * 1024 * 1024] {};
		g_model.pData = g_model.pBase;
		r5r::mstudioseqdesc_t* pV7RseqDesc = reinterpret_cast<r5r::mstudioseqdesc_t*>(g_model.pData);
		pV7RseqDesc->flags = pV53SeqDesc[seq_idx].flags;
		pV7RseqDesc->activity = pV53SeqDesc[seq_idx].activity;
		pV7RseqDesc->actweight = pV53SeqDesc[seq_idx].actweight;
		pV7RseqDesc->numevents = pV53SeqDesc[seq_idx].numevents;
		pV7RseqDesc->bbmin = pV53SeqDesc[seq_idx].bbmin;
		pV7RseqDesc->bbmax = pV53SeqDesc[seq_idx].bbmax;
		pV7RseqDesc->numblends = pV53SeqDesc[seq_idx].numblends;
		pV7RseqDesc->groupsize[0] = pV53SeqDesc[seq_idx].groupsize[0];
		pV7RseqDesc->groupsize[1] = pV53SeqDesc[seq_idx].groupsize[1];
		pV7RseqDesc->paramindex[0] = pV53SeqDesc[seq_idx].paramindex[0];
		pV7RseqDesc->paramindex[1] = pV53SeqDesc[seq_idx].paramindex[1];
		pV7RseqDesc->paramstart[0] = pV53SeqDesc[seq_idx].paramstart[0];
		pV7RseqDesc->paramstart[1] = pV53SeqDesc[seq_idx].paramstart[1];
		pV7RseqDesc->paramend[0] = pV53SeqDesc[seq_idx].paramend[0] ? pV53SeqDesc[seq_idx].paramend[0] : 1;
		pV7RseqDesc->paramend[1] = pV53SeqDesc[seq_idx].paramend[1] ? pV53SeqDesc[seq_idx].paramend[1] : 1;
		pV7RseqDesc->paramparent = pV53SeqDesc[seq_idx].paramparent;
		pV7RseqDesc->fadeintime = pV53SeqDesc[seq_idx].fadeintime;
		pV7RseqDesc->fadeouttime = pV53SeqDesc[seq_idx].fadeouttime;
		pV7RseqDesc->localentrynode = pV53SeqDesc[seq_idx].localentrynode;
		pV7RseqDesc->localexitnode = pV53SeqDesc[seq_idx].localexitnode;
		pV7RseqDesc->entryphase = pV53SeqDesc[seq_idx].entryphase;
		pV7RseqDesc->exitphase = pV53SeqDesc[seq_idx].exitphase;
		pV7RseqDesc->numikrules = 1;
		g_model.pHdr = pV7RseqDesc;
		g_model.pData += sizeof(r5r::mstudioseqdesc_t);
		BeginStringTable();

		AddToStringTable((char*)pV7RseqDesc, &pV7RseqDesc->szlabelindex, seqdescname.c_str());
		AddToStringTable((char*)pV7RseqDesc, &pV7RseqDesc->szactivitynameindex, STRING_FROM_IDX(reinterpret_cast<char*>(mdl_buffer), pV53SeqDesc[seq_idx].szactivitynameindex - base_ptr));

		//posekey 
		if (pV53SeqDesc[seq_idx].posekeyindex) {
			int numposekey = pV7RseqDesc->groupsize[0] + pV7RseqDesc->groupsize[1];
			pV7RseqDesc->posekeyindex = g_model.pData - g_model.pBase;

			float* studioPoseKey = PTR_FROM_IDX(float, &pV53SeqDesc[seq_idx], pV53SeqDesc[seq_idx].posekeyindex);
			float* posekey = reinterpret_cast<float*>(g_model.pData);
			for (int i = 0; i < numposekey; i++) {
				posekey[i] = studioPoseKey[i];
			}
			g_model.pData += sizeof(float) * (numposekey);
		}

		//events
		pV7RseqDesc->eventindex = g_model.pData - g_model.pBase;
		r5r::mstudioevent_t* rseqEvent = reinterpret_cast<r5r::mstudioevent_t*>(g_model.pData);
		pt2::mstudioevent_t* studioEvent = reinterpret_cast<pt2::mstudioevent_t*>(mdl_buffer - base_ptr + pV53SeqDesc[seq_idx].eventindex);
		for (int i = 0; i < pV7RseqDesc->numevents; i++) {
			rseqEvent[i].cycle = studioEvent[i].cycle;
			rseqEvent[i].event = studioEvent[i].event;
			rseqEvent[i].type = studioEvent[i].type;
			memcpy_s(rseqEvent[i].options, 256, studioEvent[i].options, 64);
			AddToStringTable((char*)&rseqEvent[i], &rseqEvent[i].szeventindex, STRING_FROM_IDX(&studioEvent[i], studioEvent[i].szeventindex));
		}
		g_model.pData += sizeof(r5r::mstudioevent_t) * pV7RseqDesc->numevents;

		//autolayer
		if (pV53SeqDesc[seq_idx].numautolayers) {
			pV7RseqDesc->numautolayers = pV53SeqDesc[seq_idx].numautolayers;
			pV7RseqDesc->autolayerindex = g_model.pData - g_model.pBase;
			pt2::mstudioautolayer_t* v53AutoLayer = reinterpret_cast<pt2::mstudioautolayer_t*>(mdl_buffer - base_ptr + pV53SeqDesc[seq_idx].autolayerindex);
			r5r::mstudioautolayer_t* v54AutoLayer = reinterpret_cast<r5r::mstudioautolayer_t*>(g_model.pData);
			for (int i = 0; i < pV7RseqDesc->numautolayers; i++) {
				std::string n = std::string(STRING_FROM_IDX(reinterpret_cast<char*>(mdl_buffer), -pV53SeqDesc[v53AutoLayer[i].iSequence].baseptr + pV53SeqDesc[v53AutoLayer[i].iSequence].szlabelindex)) + ".rseq";
				std::replace(n.begin(), n.end(), '\\', '/');
				n = n.substr(n.find_last_of('/') + 1);
				if (!override_path.empty()) {
					n = override_path + (override_path.back() == '\\' ? "" : "\\") + n;
				}
				std::replace(n.begin(), n.end(), '\\', '/');
				v54AutoLayer[i].assetSequence = StringToGuid(n.c_str());
				v54AutoLayer[i].iSequence = v53AutoLayer[i].iSequence + sequence_names.size() - seq_idx - 1;
				v54AutoLayer[i].iPose = v53AutoLayer[i].iPose;
				v54AutoLayer[i].flags = v53AutoLayer[i].flags;
				v54AutoLayer[i].start = v53AutoLayer[i].start;
				v54AutoLayer[i].peak = v53AutoLayer[i].peak;
				v54AutoLayer[i].tail = v53AutoLayer[i].tail;
				v54AutoLayer[i].end = v53AutoLayer[i].end;
			}
			g_model.pData += sizeof(r5r::mstudioautolayer_t) * pV7RseqDesc->numautolayers;
		}

		//weight list
		pV7RseqDesc->weightlistindex = g_model.pData - g_model.pBase;
		float* mdl_weight = reinterpret_cast<float*>(mdl_buffer - base_ptr + pV53SeqDesc[seq_idx].weightlistindex);
		float* rseq_weight = reinterpret_cast<float*>(g_model.pData);
		for (int i = 0; i < pV53MdlHdr->numbones; i++) {
			rseq_weight[i] = mdl_weight[i];
		}
		g_model.pData += sizeof(int) * pV53MdlHdr->numbones;

		//activity modifiers
		pV7RseqDesc->activitymodifierindex = g_model.pData - g_model.pBase;
		pV7RseqDesc->numactivitymodifiers = pV53SeqDesc[seq_idx].numactivitymodifiers;
		r5r::mstudioactivitymodifier_t* v53ActivityModifier = PTR_FROM_IDX(r5r::mstudioactivitymodifier_t, &pV53SeqDesc[seq_idx], pV53SeqDesc[seq_idx].activitymodifierindex);
		r5r::mstudioactivitymodifier_t* rseqActivityModifier = reinterpret_cast<r5r::mstudioactivitymodifier_t*>(g_model.pData);
		for (int i = 0; i < pV7RseqDesc->numactivitymodifiers; i++) {
			AddToStringTable((char*)&rseqActivityModifier[i], &rseqActivityModifier[i].sznameindex, STRING_FROM_IDX(&v53ActivityModifier[i], v53ActivityModifier[i].sznameindex));
			rseqActivityModifier[i].negate = v53ActivityModifier[i].negate;
		}
		g_model.pData += sizeof(r5r::mstudioactivitymodifier_t) * pV7RseqDesc->numactivitymodifiers;


		//blends
		pV7RseqDesc->animindexindex = g_model.pData - g_model.pBase;
		short* studio_blends = PTR_FROM_IDX(short, &pV53SeqDesc[seq_idx], pV53SeqDesc[seq_idx].animindexindex);
		int* rseq_blends = reinterpret_cast<int*>(g_model.pData);
		g_model.pData += sizeof(int) * pV7RseqDesc->numblends;

		for (int blend_idx = 0; blend_idx < pV7RseqDesc->numblends; blend_idx++) {
			//animdesc 
			short anim_idx = studio_blends[blend_idx];
			r5r::mstudioanimdesc_t* rAnimDesc = reinterpret_cast<r5r::mstudioanimdesc_t*>(g_model.pData);
			char* anim_name = (char*)STRING_FROM_IDX(mdl_buffer, -pV53AnimDesc[anim_idx].baseptr + pV53AnimDesc[anim_idx].sznameindex);
			rseq_blends[blend_idx] = g_model.pData - g_model.pBase;
			rAnimDesc->fps = pV53AnimDesc[anim_idx].fps;
			rAnimDesc->flags = (pV53AnimDesc[anim_idx].flags & 0x20 ? 0x0 : 0x20000) | pV53AnimDesc[anim_idx].flags;
			rAnimDesc->numframes = pV53AnimDesc[anim_idx].numframes;
			AddToStringTable(reinterpret_cast<char*>(rAnimDesc), &rAnimDesc->sznameindex, anim_name);
			g_model.pData += sizeof(r5r::mstudioanimdesc_t);
			ALIGN16(g_model.pData);

			//anims
			uint32_t* animsections = nullptr;
			unsigned int* sections_index{};
			int animbase_ptr = pV53AnimDesc[anim_idx].baseptr;
			int num_frames = pV53AnimDesc[anim_idx].numframes;
			bool hasSections = false;

#ifdef _DEBUG
			print("      L-> %s (%d)\n", anim_name, pV53AnimDesc[anim_idx].sectionframes > 0 ? (pV53AnimDesc[anim_idx].numframes / pV53AnimDesc[anim_idx].sectionframes) + 1 : 0);
#endif

			if (rAnimDesc->flags & 0x20) {
				rAnimDesc->animindex = g_model.pData - (char*)rAnimDesc;
				continue;
			}

			size_t num_sections = 1;
			if (pV53AnimDesc[anim_idx].sectionindex) {
				hasSections = true;
				num_sections = (pV53AnimDesc[anim_idx].numframes / pV53AnimDesc[anim_idx].sectionframes) + 2;
				animsections = reinterpret_cast<uint32_t*>(mdl_buffer - animbase_ptr + pV53AnimDesc[anim_idx].sectionindex);
				sections_index = reinterpret_cast<unsigned int*>(g_model.pData);
				g_model.pData += 4 * num_sections;
			}


			rAnimDesc->animindex = g_model.pData - (char*)rAnimDesc;
			rAnimDesc->sectionindex = hasSections ? rAnimDesc->animindex - ((num_sections) * 4) : 0;
			rAnimDesc->sectionframes = pV53AnimDesc[anim_idx].sectionframes;

			for (size_t section = 0; section < num_sections; section++) {
				if (hasSections) {
					sections_index[section] = g_model.pData - (char*)rAnimDesc;
					num_frames = pV53AnimDesc[anim_idx].sectionframes + 1;
					if (section == num_sections - 1) {
						num_frames = pV53AnimDesc[anim_idx].numframes - ((num_sections - 2) * pV53AnimDesc[anim_idx].sectionframes + 1);
					}
				}

				//boneflagarray (allocate)
				char* boneflagarray = reinterpret_cast<char*>(g_model.pData);
				std::vector<unsigned int> flaggedbones(pV53MdlHdr->numbones + 1, 0);
				g_model.pData += ((pV53MdlHdr->numbones + 3) / 2) % 2 == 1 ? (pV53MdlHdr->numbones + 3) / 2 - 1 : (pV53MdlHdr->numbones + 3) / 2;

				//animvalue
				int anim_block_offset = 0;
				for (int i = 0; i < pV53MdlHdr->numbones; i++) {
					tf2::mstudio_rle_anim_t* v53AnimRle{};

					if (hasSections) {
						v53AnimRle = reinterpret_cast<tf2::mstudio_rle_anim_t*>(mdl_buffer - animbase_ptr + animsections[section] + anim_block_offset);
					}
					else {
						v53AnimRle = reinterpret_cast<tf2::mstudio_rle_anim_t*>(mdl_buffer - animbase_ptr + pV53AnimDesc[anim_idx].animindex + anim_block_offset);
					}

					r5r::mstudio_rle_anim_t* rseqAnimRle = reinterpret_cast<r5r::mstudio_rle_anim_t*>(g_model.pData);
					rseqAnimRle->flags = 0;
					int read_offset = 8;
					int write_offset = sizeof(r5r::mstudio_rle_anim_t);
					int flags = 0;
					int tmp_r, tmp_p;

					r5r::studioanimvalue_ptr_t* pRseqRotV{};
					r5r::studioanimvalue_ptr_t* pRseqPosV{};
					tf2::animvalue_ptr_t* pMdlRotV{};
					tf2::animvalue_ptr_t* pMdlPosV{};

					//posscale
					float* posscale = PTR_FROM_IDX(float, g_model.pData, write_offset);
					if (!(v53AnimRle->flags & TF2_ANIM_RAWPOS)) { //ANIMPOS
						*posscale = v53AnimRle->posscale;
						write_offset += 4;
					}

					//raw data
					if (v53AnimRle->flags & TF2_ANIM_RAWPOS) {
						Vector48* rseqRawpos = PTR_FROM_IDX(Vector48, g_model.pData, write_offset); 
						rseqRawpos->x = v53AnimRle->pos.x;
						rseqRawpos->y = v53AnimRle->pos.y;
						rseqRawpos->z = v53AnimRle->pos.z;
						flags |= 0x1;
						write_offset += 6;
					}

					if (v53AnimRle->flags & TF2_ANIM_RAWROT) {
						Quaternion64* rseqRawrot = PTR_FROM_IDX(Quaternion64, g_model.pData, write_offset);
						rseqRawrot->x = v53AnimRle->rot.x;
						rseqRawrot->y = v53AnimRle->rot.y;
						rseqRawrot->z = v53AnimRle->rot.z;
						rseqRawrot->wneg = v53AnimRle->rot.wneg;
						flags |= 0x2;
						write_offset += 8;
					}

					//animvalue_ptr
					if (!(v53AnimRle->flags & TF2_ANIM_RAWROT)) { //ANIMROT
						pMdlRotV = PTR_FROM_IDX(tf2::animvalue_ptr_t, &v53AnimRle->rot, 0);
					}

					if (!(v53AnimRle->flags & TF2_ANIM_RAWPOS)) { //ANIMPOS
						pMdlPosV = PTR_FROM_IDX(tf2::animvalue_ptr_t, &v53AnimRle->pos, 0);
					}


					if (!(v53AnimRle->flags & TF2_ANIM_RAWPOS) && !pMdlPosV->IsAllZero()) { //ANIMPOS
						pRseqPosV = PTR_FROM_IDX(r5r::studioanimvalue_ptr_t, g_model.pData, write_offset);
						write_offset += sizeof(r5r::studioanimvalue_ptr_t);
						flags |= 0x1;
					}

					if (!(v53AnimRle->flags & TF2_ANIM_RAWROT) && !pMdlRotV->IsAllZero()) { //ANIMROT
						pRseqRotV = PTR_FROM_IDX(r5r::studioanimvalue_ptr_t, g_model.pData, write_offset);
						write_offset += sizeof(r5r::studioanimvalue_ptr_t);
						flags |= 0x2;
					}

					//write animvalue
					//animpos
					std::vector<int> idx_offset;
					if (!(v53AnimRle->flags & TF2_ANIM_RAWPOS) && !pMdlPosV->IsAllZero()) {
						rseqAnimRle->flags |= 0x4;
						//pos x
						pRseqPosV->flags |= 4;
						read_offset = pMdlPosV->x;
						ProcessAnimValue(&read_offset, &write_offset, (char*)&v53AnimRle->pos, pRseqPosV, num_frames, idx_offset, *posscale, *posscale, 0, !pMdlPosV->x);
						//pos y
						pRseqPosV->flags |= 2;
						read_offset = pMdlPosV->y;
						ProcessAnimValue(&read_offset, &write_offset, (char*)&v53AnimRle->pos, pRseqPosV, num_frames, idx_offset, *posscale, *posscale, 0, !pMdlPosV->y);
						//pos z
						pRseqPosV->flags |= 1;
						read_offset = pMdlPosV->z;
						ProcessAnimValue(&read_offset, &write_offset, (char*)&v53AnimRle->pos, pRseqPosV, num_frames, idx_offset, *posscale, *posscale, 0, !pMdlPosV->z);

						pRseqPosV->offset = idx_offset.at(0) * 2;
						pRseqPosV->idx1 = MAX(idx_offset.at(1) - idx_offset.at(0), 0);
						pRseqPosV->idx2 = MAX(idx_offset.at(2) - idx_offset.at(0), 0);
					}

					//animrot
					idx_offset.clear();
					if (!(v53AnimRle->flags & TF2_ANIM_RAWROT) && !pMdlRotV->IsAllZero()) {
						rseqAnimRle->flags |= 0x2;
						//rot x
						pRseqRotV->flags |= 4;
						read_offset = pMdlRotV->x;
						ProcessAnimValue(&read_offset, &write_offset, (char*)&v53AnimRle->rot, pRseqRotV, num_frames, idx_offset, 0.00019175345f, studioRotScale[v53AnimRle->bone].x, rAnimDesc->flags & 0x4 ? 0 : studioRot[v53AnimRle->bone].x, !pMdlRotV->x);
						//rot y
						pRseqRotV->flags |= 2;
						read_offset = pMdlRotV->y;
						ProcessAnimValue(&read_offset, &write_offset, (char*)&v53AnimRle->rot, pRseqRotV, num_frames, idx_offset, 0.00019175345f, studioRotScale[v53AnimRle->bone].y, rAnimDesc->flags & 0x4 ? 0 : studioRot[v53AnimRle->bone].y, !pMdlRotV->y);
						//rot z
						pRseqRotV->flags |= 1;
						read_offset = pMdlRotV->z;
						ProcessAnimValue(&read_offset, &write_offset, (char*)&v53AnimRle->rot, pRseqRotV, num_frames, idx_offset, 0.00019175345f, studioRotScale[v53AnimRle->bone].z, rAnimDesc->flags & 0x4 ? 0 : studioRot[v53AnimRle->bone].z, !pMdlRotV->z);

						pRseqRotV->offset = idx_offset.at(0) * 2;
						pRseqRotV->idx1 = MAX(idx_offset.at(1) - idx_offset.at(0), 0);
						pRseqRotV->idx2 = MAX(idx_offset.at(2) - idx_offset.at(0), 0);
					}

					flaggedbones.at(v53AnimRle->bone) = flags;
					rseqAnimRle->size = write_offset;
					g_model.pData += write_offset;
					anim_block_offset += v53AnimRle->nextoffset;

					if (v53AnimRle->nextoffset == 0) break;
				}

				//boneflagarray (write)
				for (int i = 0; i < (flaggedbones.size()) / 2; i++) {
					boneflagarray[i] = flaggedbones.at(i * 2);
					boneflagarray[i] |= flaggedbones.at(i * 2 + 1) << 4;
				}
			}

			//TODO:
			//ikrules
			rAnimDesc->numikrules = pV53AnimDesc[anim_idx].numikrules;
			rAnimDesc->ikruleindex = g_model.pData - g_model.pBase - rseq_blends[blend_idx];
			tf2::mstudioikrule_t* v53ikrule = reinterpret_cast<tf2::mstudioikrule_t*>(mdl_buffer - animbase_ptr + pV53AnimDesc[anim_idx].ikruleindex);
			r5r::mstudioikrule_t* v54ikrule = reinterpret_cast<r5r::mstudioikrule_t*>(g_model.pData);
			for (int i = 0; i < rAnimDesc->numikrules; i++) {
				v54ikrule[i].index = v53ikrule[i].index;
				v54ikrule[i].chain = v53ikrule[i].chain;
				v54ikrule[i].type = 4;// v53ikrule[i].type;
				v54ikrule[i].bone = v53ikrule[i].bone;
				v54ikrule[i].slot = v53ikrule[i].slot;
				v54ikrule[i].height = v53ikrule[i].height;
				v54ikrule[i].radius = v53ikrule[i].radius;
				v54ikrule[i].floor = v53ikrule[i].floor;
				v54ikrule[i].pos = v53ikrule[i].pos;
				v54ikrule[i].q = v53ikrule[i].q;
				v54ikrule[i].start = v53ikrule[i].start;
				v54ikrule[i].peak = v53ikrule[i].peak;
				v54ikrule[i].tail = v53ikrule[i].tail;
				v54ikrule[i].end = v53ikrule[i].end;
				v54ikrule[i].contact = v53ikrule[i].contact;
				v54ikrule[i].drop = v53ikrule[i].drop;
				v54ikrule[i].szattachmentindex = 0; //
				v54ikrule[i].endHeight = v53ikrule[i].endHeight;
			}
			g_model.pData += sizeof(r5r::mstudioikrule_t) * rAnimDesc->numikrules;

			//TODO:
			for (int i = 0; i < rAnimDesc->numikrules; i++) {
				if (v53ikrule[i].compressedikerrorindex) {
					//v54ikrule[i].compressedikerrorindex = g_model.pData - (char*)&v54ikrule[i];

					//ikrule frames...
					//tf2::mstudiocompressedikerror_t* v53ikerror = PTR_FROM_IDX(tf2::mstudiocompressedikerror_t, &v53ikrule[i], v53ikrule[i].compressedikerrorindex);
					//v54ikrule[i].compressedikerror.posscale = v53ikerror->posscale;
					//v54ikrule[i].compressedikerror.rotscale = v53ikerror->rotscale;
					//v54ikrule[i].compressedikerror.sectionframes = 0; //
					//int size = 0;
					//std::vector<int> tmp{};
					//for (int j = 0; j < 6; j++) {
					//	int read = v53ikerror->offset[j];
					//	ProcessAnimValue(&read, &size, (char*)v53ikerror, nullptr, 17, tmp);
					//}
					//g_model.pData += size;
				}
			}

			// movement
			if (pV53AnimDesc[anim_idx].framemovementindex) {
				int mmSectionFrames = 254;

				tf2::framemovement_t* v53Movement = PTR_FROM_IDX(tf2::framemovement_t, &pV53AnimDesc[anim_idx], pV53AnimDesc[anim_idx].framemovementindex);
				r5r::mstudioframemovement_t* v54Movement = reinterpret_cast<r5r::mstudioframemovement_t*>(g_model.pData);
				rAnimDesc->framemovementindex = (char*)v54Movement - (char*)rAnimDesc;
				v54Movement->scale.x = v53Movement->scale.x;
				v54Movement->scale.y = v53Movement->scale.y;
				v54Movement->scale.z = v53Movement->scale.z;
				v54Movement->scale.w = v53Movement->scale.w;
				v54Movement->sectionFrames = (pV53AnimDesc[anim_idx].numframes < mmSectionFrames) ? pV53AnimDesc[anim_idx].numframes : mmSectionFrames;
				g_model.pData += sizeof(r5r::mstudioframemovement_t);

				uint32_t mmNumSections = (pV53AnimDesc[anim_idx].numframes - 1) / mmSectionFrames + 1;
				uint32_t* mmSectionIndexes = reinterpret_cast<uint32_t*>(g_model.pData);
				g_model.pData += mmNumSections * sizeof(uint32_t);

				// aquiring movement values
				std::vector<int> animArray_x{};
				std::vector<int> animArray_y{};
				std::vector<int> animArray_z{};
				std::vector<int> animArray_yaw{};

				for (int i = 0; i < 4; i++) {
					int read = v53Movement->offset[i];
					switch (i) {
					case 0:
						animArray_x = getAnimArray(&read, (char*)v53Movement, pV53AnimDesc[anim_idx].numframes, false);
						break;
					case 1:
						animArray_y = getAnimArray(&read, (char*)v53Movement, pV53AnimDesc[anim_idx].numframes, false);
						break;
					case 2:
						animArray_z = getAnimArray(&read, (char*)v53Movement, pV53AnimDesc[anim_idx].numframes, false);
						break;
					case 3:
						animArray_yaw = getAnimArray(&read, (char*)v53Movement, pV53AnimDesc[anim_idx].numframes, false);
						break;
					}
				}

				std::vector<std::vector<int>> animArray = { animArray_x , animArray_y, animArray_z, animArray_yaw };

				// write movement values
				for (int i = 0; i < mmNumSections; i++) {
					std::vector<int> mmOffsets_tmp{};
					mmSectionIndexes[i] = g_model.pData - (char*)v54Movement;

					uint16_t* mmOffsets = reinterpret_cast<uint16_t*>(g_model.pData);
					g_model.pData += 4 * sizeof(short);

					int mmNumFrames = v54Movement->sectionFrames;
					if (i + 1 == mmNumSections) mmNumFrames = pV53AnimDesc[anim_idx].numframes - (i * mmSectionFrames);

					size_t size = 0;

					for (int j = 0; j < 4; j++) {
						if (v53Movement->offset[j]) {
							ProcessArrayAnimValue(animArray[j], &size, (r5r::studioanimvalue_ptr_t*)mmOffsets, mmNumFrames, mmOffsets_tmp, 1.f, 1.f, 0.0f, false);
						}
						else {
							mmOffsets_tmp.push_back(0);
						}
					}
					for (int j = 0; j < 4; j++) mmOffsets[j] = mmOffsets_tmp.at(j) * 2;
					g_model.pData += size;
				}
			}
		}

		//unk
		pV7RseqDesc->unkOffset = g_model.pData - g_model.pBase;

		//write all data
		g_model.pData = WriteStringTable(g_model.pData);
		ALIGN4(g_model.pData);

		outRseq.write(g_model.pBase, g_model.pData - g_model.pBase);
		g_model.stringTable.clear();
		delete[] g_model.pBase;

		print("Done!\n");
	}
	return sequence_names;
}