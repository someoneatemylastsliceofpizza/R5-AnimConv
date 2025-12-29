#include <pch.h>
#include <rseq/rseq.h>
#include <rrig/rrig.h>

void ParseMDL_v49(char* buffer, temp::rig_t& rig, std::string output_dir, std::string override_rrig_path, std::string override_rseq_path) {
	p2::studiohdr_t* pMdlHdr = reinterpret_cast<p2::studiohdr_t*>(buffer);
	p2::mstudioseqdesc_t* pStudioSeqDesc = reinterpret_cast<p2::mstudioseqdesc_t*>((buffer + pMdlHdr->localseqindex));
	p2::mstudioanimdesc_t* pStudioAnimDesc = reinterpret_cast<p2::mstudioanimdesc_t*>((buffer + pMdlHdr->localanimindex));

	p2::mstudiolinearbone_t* linearboneindexes = PTR_FROM_IDX(p2::mstudiolinearbone_t, buffer, pMdlHdr->linearboneindex + 0x198);
	Vector3* studioPosScale = PTR_FROM_IDX(Vector3, linearboneindexes, linearboneindexes->posscaleindex);
	Vector3* studioRotScale = PTR_FROM_IDX(Vector3, linearboneindexes, linearboneindexes->rotscaleindex);

	// rig header
	std::string raw = pMdlHdr->name;
	std::replace(raw.begin(), raw.end(), '/', '\\');

	if (raw.rfind("mdl", 0) == 0) raw.replace(0, 3, "animrig");
	std::string name = std::filesystem::path(raw).stem().string();
	std::string dir = override_rrig_path.empty() ? raw.substr(0, raw.rfind('\\') + 1) : override_rrig_path + (override_rrig_path.back() == '\\' ? "" : "\\");
	std::replace(dir.begin(), dir.end(), '/', '\\');
	std::replace(name.begin(), name.end(), '/', '\\');

	rig.name = dir + name.substr(0, name.rfind('.')) + ".rrig";
	rig.hdr.eyeposition = pMdlHdr->eyeposition;
	rig.hdr.illumposition = pMdlHdr->illumposition;
	rig.hdr.hull_min = pMdlHdr->hull_min;
	rig.hdr.hull_max = pMdlHdr->hull_max;
	rig.hdr.view_bbmin = pMdlHdr->view_bbmin;
	rig.hdr.view_bbmax = pMdlHdr->view_bbmax;
	//rig.hdr.flags = pMdlHdr->flags;
	rig.hdr.numbones = pMdlHdr->numbones;
	rig.hdr.activitylistversion = pMdlHdr->activitylistversion;
	rig.hdr.mass = 1.0f;
	rig.hdr.contents = 1;
	rig.hdr.defaultFadeDist = -1;
	rig.hdr.gathersize = 0;
	rig.hdr.surfaceprop = STRING_FROM_IDX(pMdlHdr, pMdlHdr->surfacepropindex);

	// bones
	rig.bones.resize(rig.hdr.numbones);
	p2::mstudiobone_t* pBones = reinterpret_cast<p2::mstudiobone_t*>(buffer + pMdlHdr->boneindex);
	for (int i = 0; i < pMdlHdr->numbones; i++) {
		rig.bones[i].name = STRING_FROM_IDX(&pBones[i], pBones[i].sznameindex);
		rig.bones[i].parent = pBones[i].parent;
		for (int j = 0; j < 6; j++) rig.bones[i].bonecontroller[j] = pBones[i].bonecontroller[j];
		rig.bones[i].pos = pBones[i].pos;
		rig.bones[i].q = pBones[i].quat;
		rig.bones[i].rot = pBones[i].rot;
		rig.bones[i].scl = { 1,1,1 };
		rig.bones[i].poseToBone = pBones[i].poseToBone;
		rig.bones[i].qAlignment = pBones[i].qAlignment;
		rig.bones[i].flags = pBones[i].flags & 0x200;
		rig.bones[i].proctype = pBones[i].proctype;
		rig.bones[i].procindex = pBones[i].procindex;
		rig.bones[i].physicsbone = pBones[i].physicsbone;
		rig.bones[i].contents = pBones[i].contents;
		rig.bones[i].surfacepropLookup = pBones[i].surfacepropLookup;
	}

	// bone by name
	auto* bonebyname = reinterpret_cast<uint8_t*>(buffer + pMdlHdr->bonetablebynameindex);
	rig.bonebyname.resize(rig.hdr.numbones);
	for (int i = 0; i < rig.hdr.numbones; i++) {
		rig.bonebyname[i] = bonebyname[i];
	}

	// hitboxsets
	rig.hitboxsets.resize(1);
	rig.hitboxsets[0].name = "default";
	/*rig.hitboxsets.resize(pMdlHdr->numhitboxsets);
	auto* pHitboxsets = PTR_FROM_IDX(p2::mstudiohitboxset_t, buffer, pMdlHdr->hitboxsetindex);
	for (int i = 0; i < pMdlHdr->numhitboxsets; i++) {
		rig.hitboxsets[i].name = STRING_FROM_IDX(&pHitboxsets[i], pHitboxsets[i].sznameindex);
		for (int j = 0; j < pHitboxsets[i].numhitboxes; j++) {
			auto* pHitbox = PTR_FROM_IDX(p2::mstudiobbox_t, &pHitboxsets[i], pHitboxsets[i].hitboxindex);
			temp::bbox_t hitbox{};
			hitbox.name = STRING_FROM_IDX(&pHitbox[j], pHitbox[j].szhitboxnameindex);
			hitbox.hitdataGroupOffset = STRING_FROM_IDX(&pHitbox[j], pHitbox[j].szhitboxnameindex);
			hitbox.bone = pHitbox[j].bone;
			hitbox.group = pHitbox[j].group;
			hitbox.bbmin = pHitbox[j].bbmin;
			hitbox.bbmax = pHitbox[j].bbmax;
			//hitbox.critShotOverride = pHitbox[j].critShotOverride;
			rig.hitboxsets[i].hitboxes.push_back(hitbox);
		}
	}*/

	// nodes
	int* pNodenames = reinterpret_cast<int*>(buffer + pMdlHdr->localnodenameindex);
	for (int i = 0; i < pMdlHdr->numlocalnodes; i++) {
		temp::node_t node{};
		node.name = STRING_FROM_IDX(reinterpret_cast<char*>(buffer), pNodenames[i]);
		if (node.name == "NODE_IGNORE") rig.ignorenode = i + 1;
		rig.nodes.push_back(node);
	}

	// pose param
	auto* pPoseparams = reinterpret_cast<p2::mstudioposeparamdesc_t*>(buffer + pMdlHdr->localposeparamindex);
	for (int i = 0; i < pMdlHdr->numlocalposeparameters; i++) {
		temp::poseparam_t poseparam{};
		poseparam.name = STRING_FROM_IDX(&pPoseparams[i], pPoseparams[i].sznameindex);
		poseparam.start = pPoseparams[i].start;
		poseparam.end = pPoseparams[i].end;
		poseparam.flags = pPoseparams[i].flags;
		poseparam.loop = pPoseparams[i].loop;
		rig.poseparams.push_back(poseparam);
	}

	// ik chains
	auto* pIkchains = reinterpret_cast<p2::mstudioikchain_t*>(buffer + pMdlHdr->ikchainindex);
	for (int i = 0; i < pMdlHdr->numikchains; i++) {
		temp::ikchain_t ikchain{};
		ikchain.name = STRING_FROM_IDX(&pIkchains[i], pIkchains[i].sznameindex);
		ikchain.linktype = pIkchains[i].linktype;
		ikchain.unk = 0.707f;

		auto* pIklinks = reinterpret_cast<p2::mstudioiklink_t*>((char*)&pIkchains[i] + pIkchains[i].linkindex);
		for (int j = 0; j < pIkchains[i].numlinks; j++) {
			temp::iklink_t iklink{};
			iklink.bone = pIklinks[j].bone;
			iklink.kneeDir = pIklinks[j].kneeDir;
			ikchain.iklinks.push_back(iklink);
			rig.bones[iklink.bone].flags |= 0x20;
		}
		SetFlagForDescendants(rig, pIklinks[pIkchains[i].numlinks - 1].bone, 0x20);
		rig.ikchains.push_back(ikchain);
	}

	// sequences
	ProgressBar bar(pMdlHdr->numlocalseq);
	if (!_enable_verbose && pMdlHdr->numlocalseq) bar.Print();
	rig.sequences.reserve(pMdlHdr->numlocalseq);
	for (int seq_idx = 0; seq_idx < pMdlHdr->numlocalseq; seq_idx++) {
		verbose("[%3d/%3d] ", seq_idx + 1, pMdlHdr->numlocalseq);

		auto* seqDesc = &pStudioSeqDesc[seq_idx];
		char* pSeqBase = reinterpret_cast<char*>(buffer - seqDesc->baseptr);

		std::string seq_dir, seqdescname;
		std::string raw_seq_name = STRING_FROM_IDX(pSeqBase, seqDesc->sznameindex);
		std::replace(raw_seq_name.begin(), raw_seq_name.end(), '\\', '/');
		std::replace(override_rseq_path.begin(), override_rseq_path.end(), '/', '\\');
		std::string seq_name = (raw_seq_name.rfind('/') != std::string::npos) ? raw_seq_name.substr(raw_seq_name.rfind('/') + 1, raw_seq_name.size()) + ".rseq" : raw_seq_name + ".rseq";

		// override path
		if (!override_rseq_path.empty()) {
			seq_dir = override_rseq_path;
			seqdescname = override_rseq_path + (override_rseq_path.back() == '\\' ? "" : "\\") + seq_name;
		}
		else {
			seq_dir = (raw_seq_name.rfind('/') != std::string::npos) ? raw_seq_name.substr(0, raw_seq_name.rfind('/')) + '/' : "";
			seqdescname = seq_dir + seq_name;
		}

		verbose("\n    ->%s\n", seqdescname.c_str());

		//header
		temp::Sequence seq(seqdescname, pMdlHdr->numbones);
		seq.anims.reserve(24);
		seq.path = output_dir + "/" + seqdescname;
		seq.flags = seqDesc->flags;
		seq.activityname = STRING_FROM_IDX(pSeqBase, seqDesc->szactivitynameindex);
		seq.activity = seqDesc->activity;
		seq.actweight = seqDesc->actweight;
		seq.bbmin = seqDesc->bbmin;
		seq.bbmax = seqDesc->bbmax;
		seq.groupsize[0] = seqDesc->groupsize[0];
		seq.groupsize[1] = seqDesc->groupsize[1];
		seq.paramindex[0] = seqDesc->paramindex[0];
		seq.paramindex[1] = seqDesc->paramindex[1];
		seq.paramstart[0] = seqDesc->paramstart[0];
		seq.paramstart[1] = seqDesc->paramstart[1];
		seq.paramend[0] = seqDesc->paramend[0] ? seqDesc->paramend[0] : 1;
		seq.paramend[1] = seqDesc->paramend[1] ? seqDesc->paramend[1] : 1;
		seq.paramparent = seqDesc->paramparent;
		seq.fadeintime = seqDesc->fadeintime;
		seq.fadeouttime = seqDesc->fadeouttime;
		seq.localentrynode = seqDesc->localentrynode;
		seq.localexitnode = seqDesc->localexitnode;
		seq.nodeflags = seqDesc->nodeflags;
		seq.entryphase = seqDesc->entryphase;
		seq.exitphase = seqDesc->exitphase;
		seq.lastframe = seqDesc->lastframe;
		seq.nextseq = seqDesc->nextseq;
		seq.pose = seqDesc->pose;
		rig.rseqpaths.push_back(seqdescname);

		// node
		temp::nodedata_t nodedata{};
		if (seqDesc->localentrynode != seqDesc->localexitnode) {
			nodedata.tonode = seqDesc->localexitnode;
			nodedata.seq = seq_idx;
			rig.nodes[seqDesc->localentrynode - 1].nodedatas.push_back(nodedata);
		}

		// posekey 
		if (seqDesc->posekeyindex) {
			auto* pPosekeys = reinterpret_cast<float*>(pSeqBase + seqDesc->posekeyindex);
			for (int i = 0; i < (seqDesc->groupsize[0] + seqDesc->groupsize[1]); i++) {
				seq.posekeys.push_back(pPosekeys[i]);
			}
		}

		// events
		if (seqDesc->numevents) {
			for (int i = 0; i < seqDesc->numevents; i++) {
				temp::seqevent_t event{};
				auto* pEvents = reinterpret_cast<p2::mstudioevent_t*>(pSeqBase + seqDesc->eventindex);
				event.name = STRING_FROM_IDX(&pEvents[i], pEvents[i].szeventindex);
				event.cycle = pEvents[i].cycle;
				event.event = pEvents[i].event;
				event.type = pEvents[i].type;
				event.options = std::string(pEvents[i].options);
				seq.events.push_back(event);
			}
		}

		//autolayer
		if (seqDesc->numautolayers) {
			p2::mstudioautolayer_t* v49AutoLayer = reinterpret_cast<p2::mstudioautolayer_t*>(pSeqBase + seqDesc->autolayerindex);
			for (int i = 0; i < seqDesc->numautolayers; i++) {
				temp::autolayer_t autolayer{};
				std::string n = std::string(STRING_FROM_IDX(&pStudioSeqDesc[v49AutoLayer[i].iSequence], pStudioSeqDesc[v49AutoLayer[i].iSequence].sznameindex)) + ".rseq";
				std::replace(n.begin(), n.end(), '\\', '/');
				if (!override_rseq_path.empty()) {
					n = n.substr(n.find_last_of('/') + 1);
					n = override_rseq_path + (override_rseq_path.back() == '\\' ? "" : "\\") + n;
				}
				std::replace(n.begin(), n.end(), '\\', '/');
				autolayer.guidSequence = StringToGuid(n.c_str());
				autolayer.iSequence = v49AutoLayer[i].iSequence;
				autolayer.iPose = v49AutoLayer[i].iPose;
				autolayer.flags = v49AutoLayer[i].flags;
				autolayer.start = v49AutoLayer[i].start;
				autolayer.peak = v49AutoLayer[i].peak;
				autolayer.tail = v49AutoLayer[i].tail;
				autolayer.end = v49AutoLayer[i].end;

				seq.autolayers.push_back(autolayer);
			}
		}

		// weightlist
		auto* pWeightList = PTR_FROM_IDX(float, pSeqBase, seqDesc->weightlistindex);
		for (int i = 0; i < pMdlHdr->numbones; i++) {
			seq.weightlist[i] = pWeightList[i];
		}

		// iklock

		// activitymodifiers
		if (seqDesc->numactivitymodifiers) {
			auto* pActmod = PTR_FROM_IDX(int, pSeqBase, seqDesc->activitymodifierindex);
			for (int i = 0; i < seqDesc->numactivitymodifiers; i++) {
				temp::actmod_t actmod{};
				actmod.name = STRING_FROM_IDX(&pActmod[i], pActmod[i]);
				if (actmod.name.length() >= 2 && actmod.name.compare(actmod.name.length() - 2, 2, "_1") == 0) {				// if activitymodifier is negate add _1 at the end of the name
					actmod.name.erase(actmod.name.length() - 2, 2);															// eg. activitymodifier crouch_1
					actmod.negate = 1;
				}
				seq.actmods.push_back(actmod);
			}
		}

		// blends
		short* studio_blends = PTR_FROM_IDX(short, pSeqBase, seqDesc->animindexindex);

		std::vector<int> blends_index_map;
		std::unordered_map<int, int> blendIndex;
		for (int blend_idx = 0; blend_idx < seqDesc->numblends; ++blend_idx) {
			int val = studio_blends[blend_idx]; 
			auto [it, inserted] = blendIndex.emplace(val, blends_index_map.size());
			if (inserted) blends_index_map.push_back(val);
			seq.blends.push_back(it->second);
		}
		seq.numuniqueblends = static_cast<uint32_t>(blends_index_map.size());

		for (int blend_idx = 0; blend_idx < seq.numuniqueblends; blend_idx++) {
			if (seqDesc->flags & 0x20) {
				continue;
			}

			//animdesc 
			int16_t anim_idx = studio_blends[blend_idx];

			temp::animdesc_t anim;
			auto* animDesc = &pStudioAnimDesc[anim_idx];
			anim.name = STRING_FROM_IDX(animDesc, animDesc->sznameindex);
			anim.fps = animDesc->fps;
			anim.flags = (animDesc->flags & 0x20 ? 0x0 : 0x20000) | (animDesc->flags & ~0x20) | (seq.IsAdditive() * r5::ANIM_DELTA);
			anim.numframes = animDesc->numframes;
			anim.InitData(pMdlHdr->numbones, rig, anim.IsAdditive());
			verbose("      L-> %s (%d sections)\n", anim.name.c_str(), animDesc->sectionframes > 0 ? (animDesc->numframes / animDesc->sectionframes) + 2 : 1);

			p2::mstudioanimsections_t* animsections = nullptr;
			int num_sections = 1;
			if (animDesc->sectionindex) {
				num_sections = GetSectionCount(*animDesc);
				animsections = reinterpret_cast<p2::mstudioanimsections_t*>((char*)animDesc + animDesc->sectionindex);
			}

			uint32_t sectionbaseframe = 0;
			for (int section = 0; section < num_sections; section++) {
				uint32_t sectionframes = GetSectionLength(*animDesc, section, num_sections);

				for (uint32_t localframe = 0; localframe < sectionframes; localframe++) {
					uint32_t frame = sectionbaseframe + localframe;

					//animvalue
					int anim_block_offset = 0;
					for (int i = 0; i < pMdlHdr->numbones; i++) {
						p2::mstudio_rle_anim_t* v49AnimRle{};

						if (num_sections > 1) {
							v49AnimRle = PTR_FROM_IDX(p2::mstudio_rle_anim_t, animDesc, animsections[section].animindex + anim_block_offset);
						}
						else {
							v49AnimRle = PTR_FROM_IDX(p2::mstudio_rle_anim_t, animDesc, animDesc->animindex + anim_block_offset);
						}

						if (v49AnimRle->bone > pMdlHdr->numbones) {
							break;
						}

						int read_offset = sizeof(p2::mstudio_rle_anim_t);
						auto* trackval = &anim.animdata[v49AnimRle->bone];

						//raw data
						if (v49AnimRle->flags & 0x20) {
							Quaternion64* mdlRawrot = PTR_FROM_IDX(Quaternion64, v49AnimRle, read_offset);
							trackval->rot[frame] = UnpackQuat64(*mdlRawrot).ToRad();
							read_offset += sizeof(Quaternion64);
						}

						if (v49AnimRle->flags & 0x1) {
							Vector48* mdlRawpos = PTR_FROM_IDX(Vector48, v49AnimRle, read_offset);
							trackval->pos[frame] = Unpack48(*mdlRawpos);
							read_offset += sizeof(Vector48);
						}

						// animrot
						if (v49AnimRle->flags & 0x08) {
							auto* pMdlRotV = reinterpret_cast<uint16_t*>((char*)v49AnimRle + read_offset);
							for (int j = 0; j < 3; j++) {
								float v1 = 0;
								if (pMdlRotV[j]) {
									auto* panimvalue = reinterpret_cast<r5::anim::mstudioanimvalue_t*>((char*)pMdlRotV + pMdlRotV[j]);
									p2::RLE::ExtractAnimValue(localframe, panimvalue, studioRotScale[v49AnimRle->bone][j], v1);
								}
								if (anim.IsAdditive()) trackval->rot[frame][j] = rig.bones[v49AnimRle->bone].rot[j] + v1;
								else trackval->rot[frame][j] += v1;
							}
							read_offset += sizeof(p2::studioanimvalue_ptr_t);
						}

						// animpos
						if (v49AnimRle->flags & 0x04) {
							auto* pMdlPosV = reinterpret_cast<uint16_t*>((char*)v49AnimRle + read_offset);
							for (int j = 0; j < 3; j++) {
								float v1 = 0;
								if (pMdlPosV[j]) {
									auto* panimvalue = reinterpret_cast<r5::anim::mstudioanimvalue_t*>((char*)pMdlPosV + pMdlPosV[j]);
									p2::RLE::ExtractAnimValue(localframe, panimvalue, studioPosScale[v49AnimRle->bone][j], v1);
								}
								if (anim.IsAdditive()) trackval->pos[frame][j] = rig.bones[v49AnimRle->bone].pos[j] + v1;
								else trackval->pos[frame][j] += v1;
							}
							read_offset += sizeof(p2::studioanimvalue_ptr_t);
						}
						anim_block_offset += v49AnimRle->nextoffset;
						if (v49AnimRle->nextoffset == 0) break;
					}
				}
				sectionbaseframe += sectionframes;
			}

			//TODO:
			//ikrules
			//movements

			seq.anims.push_back(anim);
		}
		if (!_enable_verbose && pMdlHdr->numlocalseq) bar.AddAndPrint();
		rig.sequences.push_back(seq);
	}
}

void ParseMDL_v53(char* buffer, temp::rig_t& rig, std::string output_dir, std::string override_rrig_path, std::string override_rseq_path) {
	r2::studiohdr_t* pMdlHdr = reinterpret_cast<r2::studiohdr_t*>(buffer);
	r2::mstudioseqdesc_t* pStudioSeqDesc = reinterpret_cast<r2::mstudioseqdesc_t*>((buffer + pMdlHdr->localseqindex));
	r2::mstudioanimdesc_t* pStudioAnimDesc = reinterpret_cast<r2::mstudioanimdesc_t*>((buffer + pMdlHdr->localanimindex));

	std::string raw = pMdlHdr->name;
	std::replace(raw.begin(), raw.end(), '/', '\\');

	if (raw.rfind("mdl", 0) == 0) raw.replace(0, 3, "animrig");
	std::string name = std::filesystem::path(raw).stem().string();
	std::string dir = override_rrig_path.empty() ? raw.substr(0, raw.rfind('\\') + 1) : override_rrig_path + (override_rrig_path.back() == '\\' ? "" : "\\");
	std::replace(dir.begin(), dir.end(), '/', '\\');
	std::replace(name.begin(), name.end(), '/', '\\');

	rig.name = dir + name.substr(0, name.rfind('.')) + ".rrig";
	rig.hdr.eyeposition = pMdlHdr->eyeposition;
	rig.hdr.illumposition = pMdlHdr->illumposition;
	rig.hdr.hull_min = pMdlHdr->hull_min;
	rig.hdr.hull_max = pMdlHdr->hull_max;
	rig.hdr.view_bbmin = pMdlHdr->view_bbmin;
	rig.hdr.view_bbmax = pMdlHdr->view_bbmax;
	//rig.hdr.flags = pMdlHdr->flags;
	rig.hdr.numbones = pMdlHdr->numbones;
	rig.hdr.activitylistversion = pMdlHdr->activitylistversion;
	rig.hdr.mass = 1.0f;
	rig.hdr.contents = 1;
	rig.hdr.defaultFadeDist = -1;
	rig.hdr.gathersize = 0;
	rig.hdr.surfaceprop = STRING_FROM_IDX(pMdlHdr, pMdlHdr->surfacepropindex);

	// bones
	rig.bones.resize(rig.hdr.numbones);
	auto* pBones = reinterpret_cast<r2::mstudiobone_t*>(buffer + pMdlHdr->boneindex);
	for (int i = 0; i < pMdlHdr->numbones; i++) {
		rig.bones[i].name = STRING_FROM_IDX(&pBones[i], pBones[i].sznameindex);
		rig.bones[i].parent = pBones[i].parent;
		for (int j = 0; j < 6; j++) rig.bones[i].bonecontroller[j] = pBones[i].bonecontroller[j];
		rig.bones[i].pos = pBones[i].pos;
		rig.bones[i].q = pBones[i].quat;
		rig.bones[i].rot = pBones[i].rot;
		rig.bones[i].scl = { 1,1,1 };
		rig.bones[i].poseToBone = pBones[i].poseToBone;
		rig.bones[i].qAlignment = pBones[i].qAlignment;
		rig.bones[i].flags = pBones[i].flags & 0x200;
		rig.bones[i].proctype = pBones[i].proctype; //
		rig.bones[i].procindex = pBones[i].procindex; //
		rig.bones[i].physicsbone = pBones[i].physicsbone; //
		rig.bones[i].contents = pBones[i].contents;
		rig.bones[i].surfacepropLookup = pBones[i].surfacepropLookup;
	}

	//bone by name
	auto* bonebyname = reinterpret_cast<uint8_t*>(buffer + pMdlHdr->bonetablebynameindex);
	rig.bonebyname.resize(rig.hdr.numbones);
	for (int i = 0; i < rig.hdr.numbones; i++) {
		rig.bonebyname[i] = bonebyname[i];
	}

	//hitboxsets
	rig.hitboxsets.resize(1);
	rig.hitboxsets[0].name = "default";

	//nodes
	int* pNodenames = reinterpret_cast<int*>(buffer + pMdlHdr->localnodenameindex);
	for (int i = 0; i < pMdlHdr->numlocalnodes; i++) {
		temp::node_t node{};
		node.name = STRING_FROM_IDX(reinterpret_cast<char*>(buffer), pNodenames[i]);
		if (node.name == "NODE_IGNORE") rig.ignorenode = i + 1;
		rig.nodes.push_back(node);
	}

	//pose param
	auto* pPoseparams = reinterpret_cast<r2::mstudioposeparamdesc_t*>(buffer + pMdlHdr->localposeparamindex);
	for (int i = 0; i < pMdlHdr->numlocalposeparameters; i++) {
		temp::poseparam_t poseparam{};
		poseparam.name = STRING_FROM_IDX(&pPoseparams[i], pPoseparams[i].sznameindex);
		poseparam.start = pPoseparams[i].start;
		poseparam.end = pPoseparams[i].end;
		poseparam.flags = pPoseparams[i].flags;
		poseparam.loop = pPoseparams[i].loop;
		rig.poseparams.push_back(poseparam);
	}

	//ik chains
	auto* pIkchains = reinterpret_cast<r2::mstudioikchain_t*>(buffer + pMdlHdr->ikchainindex);
	for (int i = 0; i < pMdlHdr->numikchains; i++) {
		temp::ikchain_t ikchain{};
		ikchain.name = STRING_FROM_IDX(&pIkchains[i], pIkchains[i].sznameindex);
		ikchain.linktype = pIkchains[i].linktype;
		ikchain.unk = pIkchains[i].unk;

		auto* pIklinks = reinterpret_cast<r2::mstudioiklink_t*>((char*)&pIkchains[i] + pIkchains[i].linkindex);
		for (int j = 0; j < pIkchains[i].numlinks; j++) {
			temp::iklink_t iklink{};
			iklink.bone = pIklinks[j].bone;
			iklink.kneeDir = pIklinks[j].kneeDir;
			ikchain.iklinks.push_back(iklink);
			rig.bones[iklink.bone].flags |= 0x20;
		}
		SetFlagForDescendants(rig, pIklinks[pIkchains[i].numlinks - 1].bone, 0x20);
		rig.ikchains.push_back(ikchain);
	}

	// sequences
	ProgressBar bar(pMdlHdr->numlocalseq);
	if (!_enable_verbose && pMdlHdr->numlocalseq) bar.Print();
	rig.sequences.reserve(pMdlHdr->numlocalseq);
	for (int seq_idx = 0; seq_idx < pMdlHdr->numlocalseq; seq_idx++) {
		verbose("[%3d/%3d] ", seq_idx + 1, pMdlHdr->numlocalseq);

		auto* seqDesc = &pStudioSeqDesc[seq_idx];
		char* pSeqBase = reinterpret_cast<char*>(buffer - seqDesc->baseptr);

		std::string seq_dir, seqdescname;
		std::string raw_seq_name = STRING_FROM_IDX(pSeqBase, seqDesc->szlabelindex);
		std::replace(raw_seq_name.begin(), raw_seq_name.end(), '\\', '/');
		std::replace(override_rseq_path.begin(), override_rseq_path.end(), '/', '\\');
		std::string seq_name = (raw_seq_name.rfind('/') != std::string::npos) ? raw_seq_name.substr(raw_seq_name.rfind('/') + 1, raw_seq_name.size()) + ".rseq" : raw_seq_name + ".rseq";

		// override path
		if (!override_rseq_path.empty()) {
			seq_dir = override_rseq_path;
			seqdescname = override_rseq_path + (override_rseq_path.back() == '\\' ? "" : "\\") + seq_name;
		}
		else {
			seq_dir = (raw_seq_name.rfind('/') != std::string::npos) ? raw_seq_name.substr(0, raw_seq_name.rfind('/')) + '/' : "";
			seqdescname = seq_dir + seq_name;
		}

		verbose("\n    ->%s\n", seqdescname.c_str());

		//header
		temp::Sequence seq(seqdescname, pMdlHdr->numbones);
		seq.anims.reserve(24);
		seq.path = output_dir + "/" + seqdescname;
		seq.flags = seqDesc->flags;
		seq.activityname = STRING_FROM_IDX(pSeqBase, seqDesc->szactivitynameindex);
		seq.activity = seqDesc->activity;
		seq.actweight = seqDesc->actweight;
		seq.bbmin = seqDesc->bbmin;
		seq.bbmax = seqDesc->bbmax;
		seq.groupsize[0] = seqDesc->groupsize[0];
		seq.groupsize[1] = seqDesc->groupsize[1];
		seq.paramindex[0] = seqDesc->paramindex[0];
		seq.paramindex[1] = seqDesc->paramindex[1];
		seq.paramstart[0] = seqDesc->paramstart[0];
		seq.paramstart[1] = seqDesc->paramstart[1];
		seq.paramend[0] = seqDesc->paramend[0] ? seqDesc->paramend[0] : 1;
		seq.paramend[1] = seqDesc->paramend[1] ? seqDesc->paramend[1] : 1;
		seq.paramparent = seqDesc->paramparent;
		seq.fadeintime = seqDesc->fadeintime;
		seq.fadeouttime = seqDesc->fadeouttime;
		seq.localentrynode = seqDesc->localentrynode;
		seq.localexitnode = seqDesc->localexitnode;
		seq.nodeflags = seqDesc->nodeflags;
		seq.entryphase = seqDesc->entryphase;
		seq.exitphase = seqDesc->exitphase;
		seq.lastframe = seqDesc->lastframe;
		seq.nextseq = seqDesc->nextseq;
		seq.pose = seqDesc->pose;

		rig.rseqpaths.push_back(seqdescname);
		temp::nodedata_t nodedata{};
		if (seqDesc->localentrynode != seqDesc->localexitnode) {
			nodedata.tonode = seqDesc->localexitnode;
			nodedata.seq = seq_idx;
			rig.nodes[seqDesc->localentrynode - 1].nodedatas.push_back(nodedata);
		}

		//posekey 
		if (seqDesc->posekeyindex) {
			auto* pPosekeys = reinterpret_cast<float*>(pSeqBase + seqDesc->posekeyindex);
			for (int i = 0; i < (seqDesc->groupsize[0] + seqDesc->groupsize[1]); i++) {
				seq.posekeys.push_back(pPosekeys[i]);
			}
		}

		// events
		if (seqDesc->numevents) {
			for (int i = 0; i < seqDesc->numevents; i++) {
				temp::seqevent_t event{};
				auto* pEvents = reinterpret_cast<p2::mstudioevent_t*>(pSeqBase + seqDesc->eventindex);
				event.name = STRING_FROM_IDX(&pEvents[i], pEvents[i].szeventindex);
				event.cycle = pEvents[i].cycle;
				event.event = pEvents[i].event;
				event.type = pEvents[i].type;
				event.options = std::string(pEvents[i].options);
				seq.events.push_back(event);
			}
		}

		//autolayer
		if (seqDesc->numautolayers) {
			p2::mstudioautolayer_t* v49AutoLayer = reinterpret_cast<p2::mstudioautolayer_t*>(pSeqBase + seqDesc->autolayerindex);
			for (int i = 0; i < seqDesc->numautolayers; i++) {
				temp::autolayer_t autolayer{};
				std::string n = std::string(STRING_FROM_IDX(&pStudioSeqDesc[v49AutoLayer[i].iSequence], pStudioSeqDesc[v49AutoLayer[i].iSequence].szlabelindex)) + ".rseq";
				std::replace(n.begin(), n.end(), '\\', '/');
				if (!override_rseq_path.empty()) {
					n = n.substr(n.find_last_of('/') + 1);
					n = override_rseq_path + (override_rseq_path.back() == '\\' ? "" : "\\") + n;
				}
				std::replace(n.begin(), n.end(), '\\', '/');
				autolayer.guidSequence = StringToGuid(n.c_str());
				autolayer.iSequence = v49AutoLayer[i].iSequence;
				autolayer.iPose = v49AutoLayer[i].iPose;
				autolayer.flags = v49AutoLayer[i].flags;
				autolayer.start = v49AutoLayer[i].start;
				autolayer.peak = v49AutoLayer[i].peak;
				autolayer.tail = v49AutoLayer[i].tail;
				autolayer.end = v49AutoLayer[i].end;

				seq.autolayers.push_back(autolayer);
			}
		}

		// weightlist
		auto* pWeightList = PTR_FROM_IDX(float, pSeqBase, seqDesc->weightlistindex);
		for (int i = 0; i < pMdlHdr->numbones; i++) {
			seq.weightlist[i] = pWeightList[i];
		}

		//iklock

		// activitymodifiers
		if (seqDesc->numactivitymodifiers) {
			auto* pActmod = PTR_FROM_IDX(int, pSeqBase, seqDesc->activitymodifierindex);
			for (int i = 0; i < seqDesc->numactivitymodifiers; i++) {
				temp::actmod_t actmod{};
				actmod.name = STRING_FROM_IDX(&pActmod[i], pActmod[i]);
				if (actmod.name.length() >= 2 && actmod.name.compare(actmod.name.length() - 2, 2, "_1") == 0) {				// if activitymodifier is negate add _1 at the end of the name
					actmod.name.erase(actmod.name.length() - 2, 2);															// eg. activitymodifier crouch_1
					actmod.negate = 1;
				}
				seq.actmods.push_back(actmod);
			}
		}

		// blends
		short* studio_blends = PTR_FROM_IDX(short, pSeqBase, seqDesc->animindexindex);

		std::vector<int> blends_index_map;
		std::unordered_map<int, int> blendIndex;
		for (int blend_idx = 0; blend_idx < seqDesc->numblends; ++blend_idx) {
			int val = studio_blends[blend_idx];
			auto [it, inserted] = blendIndex.emplace(val, blends_index_map.size());
			if (inserted) blends_index_map.push_back(val);
			seq.blends.push_back(it->second);
		}
		seq.numuniqueblends = static_cast<uint32_t>(blends_index_map.size());

		for (int blend_idx = 0; blend_idx < seqDesc->numblends; blend_idx++) {
			if (seqDesc->flags & 0x20) {
				continue;
			}

			//animdesc 
			int16_t anim_idx = studio_blends[blend_idx];

			temp::animdesc_t anim;
			auto* animDesc = &pStudioAnimDesc[anim_idx];
			char* pAnimBase = reinterpret_cast<char*>((char*)animDesc - animDesc->baseptr);
			anim.name = STRING_FROM_IDX(animDesc, animDesc->sznameindex);
			anim.fps = animDesc->fps;
			anim.flags = (animDesc->flags & 0x20 ? 0x0 : 0x20000) | (animDesc->flags & ~0x20) | (seq.IsAdditive() * r5::ANIM_DELTA);
			anim.numframes = animDesc->numframes;
			anim.InitData(pMdlHdr->numbones, rig, anim.IsAdditive());
			verbose("      L-> %s (%d sections)\n", anim.name.c_str(), animDesc->sectionframes > 0 ? (animDesc->numframes / animDesc->sectionframes) + 2 : 1);

			uint32_t* animsections = nullptr;
			size_t num_sections = 1;
			if (animDesc->sectionindex) {
				num_sections = GetSectionCount(*animDesc);
				animsections = reinterpret_cast<uint32_t*>((char*)animDesc + animDesc->sectionindex);
			}

			uint32_t sectionbaseframe = 0;
			for (int section = 0; section < num_sections; section++) {
				uint32_t sectionframes = GetSectionLength(*animDesc, section, num_sections);

				for (uint32_t localframe = 0; localframe < sectionframes; localframe++) {
					uint32_t frame = sectionbaseframe + localframe;

					//animvalue
					int anim_block_offset = 0;
					for (int i = 0; i < pMdlHdr->numbones; i++) {
						r2::mstudio_rle_anim_t* v53AnimRle{};

						if (num_sections > 1) {
							v53AnimRle = reinterpret_cast<r2::mstudio_rle_anim_t*>((char*)animDesc + animsections[section] + anim_block_offset);
						}
						else {
							v53AnimRle = reinterpret_cast<r2::mstudio_rle_anim_t*>((char*)animDesc + pStudioAnimDesc[anim_idx].animindex + anim_block_offset);
						}

						if (v53AnimRle->bone > pMdlHdr->numbones) {
							break;
						}

						bool bNoRot = v53AnimRle->flags & 0x10;
						auto* trackval = &anim.animdata[v53AnimRle->bone];

						//raw data
						if (v53AnimRle->flags & 0x4) {
							if (bNoRot) {
								if (anim.IsAdditive())
									trackval->rot[frame] = { 0, 0, 0 };
								else
									trackval->rot[frame] = rig.bones[v53AnimRle->bone].rot;
							}
							else {
								trackval->rot[frame] = UnpackQuat64(v53AnimRle->rot).ToRad();
							}
						}

						if (v53AnimRle->flags & 0x2) {
							Vector48* mdlRawpos = reinterpret_cast<Vector48*>(&v53AnimRle->pos);
							trackval->pos[frame] = Unpack48(*mdlRawpos);
						}

						if (v53AnimRle->flags & 0x08) {
							Vector48* mdlRawscl = reinterpret_cast<Vector48*>(&v53AnimRle->scale);
							trackval->scl[frame] = Unpack48(*mdlRawscl);
						}

						// Animated rot
						if (!(v53AnimRle->flags & 0x4) && !bNoRot) {
							auto* pMdlRotV = reinterpret_cast<uint16_t*>(&v53AnimRle->rot);
							for (int j = 0; j < 3; j++) {
								float v1 = 0;
								if (pMdlRotV[j]) {
									auto* panimvalue = reinterpret_cast<r5::anim::mstudioanimvalue_t*>((char*)pMdlRotV + pMdlRotV[j]);
									p2::RLE::ExtractAnimValue(localframe, panimvalue, pBones[v53AnimRle->bone].rotscale[j], v1);
								}
								trackval->rot[frame][j] += v1;
							}
						}

						// Animated pos
						if (!(v53AnimRle->flags & 0x2)) {
							auto* pMdlPosV = reinterpret_cast<uint16_t*>(&v53AnimRle->pos);
							for (int j = 0; j < 3; j++) {
								float v1 = 0;
								if (pMdlPosV[j]) {
									auto* panimvalue = reinterpret_cast<r5::anim::mstudioanimvalue_t*>((char*)pMdlPosV + pMdlPosV[j]);
									p2::RLE::ExtractAnimValue(localframe, panimvalue, v53AnimRle->posscale, v1);
								}
								trackval->pos[frame][j] += v1;
							}
						}

						// Animated scale
						if (!(v53AnimRle->flags & 0x08)) {
							auto* pMdlSclV = reinterpret_cast<uint16_t*>(&v53AnimRle->scale);
							for (int j = 0; j < 3; j++) {
								float v1 = 0;
								if (pMdlSclV[j]) {
									auto* panimvalue = reinterpret_cast<r5::anim::mstudioanimvalue_t*>((char*)pMdlSclV + pMdlSclV[j]);
									p2::RLE::ExtractAnimValue(localframe, panimvalue, pBones[v53AnimRle->bone].scalescale[j], v1);
								}
								trackval->scl[frame][j] += v1;
							}
						}
						anim_block_offset += v53AnimRle->nextoffset;
						if (v53AnimRle->nextoffset == 0) break;
					}
				}
				sectionbaseframe += sectionframes;
			}

			// ikrules
			if (animDesc->numikrules) {
				auto* ikrules = reinterpret_cast<r2::mstudioikrule_t*>((char*)animDesc + animDesc->ikruleindex);
				bool d = (anim.name == "@ref_reactions");
				for (int i = 0; i < animDesc->numikrules; i++) {
					temp::ikrule_t ikrule{};
					ikrule.index = ikrules[i].index;
					ikrule.type = ikrules[i].type;
					ikrule.chain = ikrules[i].chain;
					ikrule.bone = ikrules[i].bone;
					ikrule.slot = ikrules[i].slot;
					ikrule.height = ikrules[i].height;
					ikrule.radius = ikrules[i].radius;
					ikrule.floor = ikrules[i].floor;
					ikrule.pos = ikrules[i].pos;
					ikrule.q = ikrules[i].q;
					ikrule.iStart = ikrules[i].iStart;
					ikrule.start = ikrules[i].start;
					ikrule.peak = ikrules[i].peak;
					ikrule.tail = ikrules[i].tail;
					ikrule.end = ikrules[i].end;
					ikrule.contact = ikrules[i].contact;
					ikrule.drop = ikrules[i].drop;
					ikrule.top = ikrules[i].top;
					ikrule.endHeight = ikrules[i].endHeight;
					if (ikrules[i].szattachmentindex) ikrule.attachmentname = STRING_FROM_IDX(&ikrules[i], ikrules[i].szattachmentindex);

					// parse ikrule frames
					if (ikrules[i].compressedikerrorindex) {
						if (ikrules[i].end < ikrules[i].start) ikrules[i].end += 1;
						int32_t numikframes = static_cast<int32_t>(anim.numframes * (std::min({ ikrules[i].end , 1.f }) - ikrules[i].start));
						ikrule.ikruledata.resize(anim.numframes);
						
						auto* frameshdr = reinterpret_cast<r2::mstudiocompressedikerror_t*>((char*)&ikrules[i] + ikrules[i].compressedikerrorindex);
						for (int frame = 0; frame < numikframes; frame++) {
							for (int idx = 0; idx < 6; idx++) {
								ikrule.scale[idx] = frameshdr->posscale[idx];
								if (frameshdr->offset[idx]) {
									auto* panimvalue = reinterpret_cast<r5::anim::mstudioanimvalue_t*>((char*)frameshdr + frameshdr->offset[idx]);
									if (idx < 3)
										p2::RLE::ExtractAnimValue(frame, panimvalue, frameshdr->posscale[idx], ikrule.ikruledata.pos[ikrules[i].iStart + frame][idx]);
									else
										p2::RLE::ExtractAnimValue(frame, panimvalue, frameshdr->rotscale[idx - 3], ikrule.ikruledata.rot[ikrules[i].iStart + frame][idx - 3]);
								}
							}
						};
						ikrule.sectionframes = animDesc->sectionframes ? std::max({ animDesc->sectionframes, std::min({ animDesc->numframes, 0xFE}) }) : animDesc->numframes;
					}
					anim.ikrules.push_back(ikrule);
				}
			}

			// movement
			if ((animDesc->flags & r5::ANIM_FRAMEMOVEMENT) && animDesc->framemovementindex) {
				auto* frameshdr = reinterpret_cast<r2::mstudioframemovement_t*>((char*)animDesc + animDesc->framemovementindex);
				temp::framemovement_t movement{};
				movement.scale = frameshdr->scale;
				movement.sectionframes = animDesc->sectionframes ? std::max({ animDesc->sectionframes, std::min({animDesc->numframes, 0xFE}) }) : animDesc->numframes;
				movement.movementdata.resize(anim.numframes);

				for (int frame = 0; frame < animDesc->numframes; frame++) {
					for (int idx = 0; idx < 4; idx++) {
						if (frameshdr->offset[idx]) {
							auto* panimvalue = reinterpret_cast<r5::anim::mstudioanimvalue_t*>((char*)frameshdr + frameshdr->offset[idx]);
							p2::RLE::ExtractAnimValue(frame, panimvalue, frameshdr->scale[idx], movement.movementdata[frame][idx]);
						}
					}
				}
				anim.movement = movement;
			}
			seq.anims.push_back(anim);
		}
		if (!_enable_verbose && pMdlHdr->numlocalseq) bar.AddAndPrint();
		rig.sequences.push_back(seq);
	}
}
