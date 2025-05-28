#include <mdl/rrig.h>
#include <mdl/studio.h>
#include <utils/stringtable.cpp>


void ProcessAnimValue(int& read_offset, int& write_offset, pt2::mstudio_rle_anim_t* mdlAnimRle, r5r::studioanimvalue_ptr_t* pRseqValue, int numframe, std::vector<int>& idx_offset, float newScale, float oldScale, float shift = 0.0f, bool isZero = false);

std::vector<std::string> ConvertMDL_RSEQ(char* mdl_buffer, std::string output_dir, std::string filename, std::vector<std::pair<std::pair<int, int>, int>> &nodedata) {
	pt2::studiohdr_t* pV49MdlHdr = reinterpret_cast<pt2::studiohdr_t*>(mdl_buffer);
	pt2::mstudioseqdesc_t* pStudioSeqDesc = reinterpret_cast<pt2::mstudioseqdesc_t*>((mdl_buffer + pV49MdlHdr->localseqindex));
	pt2::mstudioanimdesc_t* pStudioAnimDesc = reinterpret_cast<pt2::mstudioanimdesc_t*>((mdl_buffer + pV49MdlHdr->localanimindex));

	pt2::mstudiolinearbone_t* linearboneindexes = PTR_FROM_IDX(pt2::mstudiolinearbone_t, mdl_buffer, pV49MdlHdr->srcbonetransformindex);
	Vector3* studioPosScale = PTR_FROM_IDX(Vector3, linearboneindexes, linearboneindexes->posscaleindex);
	Vector3* studioRotScale = PTR_FROM_IDX(Vector3, linearboneindexes, linearboneindexes->rotscaleindex);
	Vector3* studioRot = PTR_FROM_IDX(Vector3, linearboneindexes, linearboneindexes->rotindex);

	std::vector<std::string> sequence_names;

	for (int seq_idx = 0; seq_idx < pV49MdlHdr->numlocalseq; seq_idx++) {
		printf("Converting sequence %d/%d\n", seq_idx + 1, pV49MdlHdr->numlocalseq);

		int base_ptr = pStudioSeqDesc[seq_idx].baseptr;

		std::string model_dir = "";
		std::string seq_name = STRING_FROM_IDX(reinterpret_cast<char*>(mdl_buffer), -base_ptr + pStudioSeqDesc[seq_idx].sznameindex);

		std::string seqdescname = seq_name + ".rseq";
		size_t pos = seq_name.rfind('/');
		model_dir = (pos != std::string::npos) ? seq_name.substr(0, pos) : "";
		std::string output_path = output_dir + "/" + seqdescname;
		std::replace(output_path.begin(), output_path.end(), '/', '\\');
		std::replace(model_dir.begin(), model_dir.end(), '/', '\\');
		std::filesystem::create_directories(output_dir + "\\" + model_dir);

		std::ofstream outRseq(output_path, std::ios::out | std::ios::binary);
		printf("    ->%s", seqdescname.c_str());

#ifdef _DEBUG
		printf("\n");
#else
		printf("  ...");
#endif

		if (seqdescname != "ref.rseq")
			sequence_names.push_back(seqdescname);

		if(pStudioSeqDesc[seq_idx].localentrynode != pStudioSeqDesc[seq_idx].localexitnode)
			nodedata.push_back({ {pStudioSeqDesc[seq_idx].localentrynode , pStudioSeqDesc[seq_idx].localexitnode}, seq_idx - 1 /*doesn't include ref.rseq*/});

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
		//if (pStudioSeqDesc[seq_idx].numautolayers) {
		//	pV7RseqDesc->numautolayers = pStudioSeqDesc[seq_idx].numautolayers;
		//	pV7RseqDesc->autolayerindex = g_model.pData - g_model.pBase;
		//	pt2::mstudioautolayer_t* v49AutoLayer = reinterpret_cast<pt2::mstudioautolayer_t*>(mdl_buffer - base_ptr + pStudioSeqDesc[seq_idx].autolayerindex);
		//	r5r::mstudioautolayer_t* v54AutoLayer = reinterpret_cast<r5r::mstudioautolayer_t*>(g_model.pData);
		//	for (int i = 0; i < pV7RseqDesc->numautolayers; i++) {
  //              std::string n = std::string(STRING_FROM_IDX(reinterpret_cast<char*>(mdl_buffer), -pStudioSeqDesc[v49AutoLayer[i].iSequence].baseptr + pStudioSeqDesc[v49AutoLayer[i].iSequence].sznameindex)) + ".rseq";
		//		v54AutoLayer[i].assetSequence = StringToGuid(n.c_str());
		//		v54AutoLayer[i].iSequence = v49AutoLayer[i].iSequence + sequence_names.size() - seq_idx - 1; //
		//		v54AutoLayer[i].iPose = v49AutoLayer[i].iPose;
		//		v54AutoLayer[i].flags = v49AutoLayer[i].flags;
		//		v54AutoLayer[i].start = v49AutoLayer[i].start;
		//		v54AutoLayer[i].peak = v49AutoLayer[i].peak;
		//		v54AutoLayer[i].tail = v49AutoLayer[i].tail;
		//		v54AutoLayer[i].end = v49AutoLayer[i].end;
		//	}
		//	g_model.pData += sizeof(r5r::mstudioautolayer_t) * pV7RseqDesc->numautolayers;
		//}

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
            } else {
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
			short blend = studio_blends[blend_idx];
			r5r::mstudioanimdesc_t* rAnimDesc = reinterpret_cast<r5r::mstudioanimdesc_t*>(g_model.pData);
			char* anim_name = (char *)STRING_FROM_IDX(mdl_buffer, -pStudioAnimDesc[blend].baseptr + pStudioAnimDesc[blend].sznameindex);
			rseq_blends[blend_idx] = g_model.pData - g_model.pBase;
			rAnimDesc->fps = pStudioAnimDesc[blend].fps;
			rAnimDesc->flags = (pStudioAnimDesc[blend].numframes > 0 ? 0x20000 : 0x0) | pStudioAnimDesc[blend].flags;
			rAnimDesc->numframes = pStudioAnimDesc[blend].numframes;
			AddToStringTable(reinterpret_cast<char*>(rAnimDesc), &rAnimDesc->sznameindex, anim_name);
			g_model.pData += sizeof(r5r::mstudioanimdesc_t);
			ALIGN16(g_model.pData);

			//anims
			pt2::mstudioanimsections_t* animsections = nullptr;
			unsigned int* sections_index{};
			int animbase_ptr = pStudioAnimDesc[blend].baseptr;
			int num_frames = pStudioAnimDesc[blend].numframes;
			bool hasSections = false;

#ifdef _DEBUG
			printf("      L-> %s (%d)\n", anim_name, pStudioAnimDesc[blend].sectionframes > 0 ? (pStudioAnimDesc[blend].numframes / pStudioAnimDesc[blend].sectionframes) + 1 : 0);
#endif
			size_t num_sections = 1;
			if (pStudioAnimDesc[blend].sectionindex) {
				hasSections = true;
				num_sections = (pStudioAnimDesc[blend].numframes / pStudioAnimDesc[blend].sectionframes) + 2;
				animsections = reinterpret_cast<pt2::mstudioanimsections_t*>(mdl_buffer - animbase_ptr + pStudioAnimDesc[blend].sectionindex);
				sections_index = reinterpret_cast<unsigned int*>(g_model.pData);
				g_model.pData += 4 * num_sections;
			}


			rAnimDesc->animindex = g_model.pData - (char*)rAnimDesc;
			rAnimDesc->sectionindex = hasSections ? rAnimDesc->animindex - ((num_sections) * 4 ) : 0;
			rAnimDesc->sectionframes = pStudioAnimDesc[blend].sectionframes;

			for (size_t section = 0; section < num_sections; section++) {
				if (hasSections) {
					sections_index[section] = g_model.pData - (char*)rAnimDesc; 
					num_frames = pStudioAnimDesc[blend].sectionframes + 1;
					if (section == num_sections - 1) {
						num_frames = pStudioAnimDesc[blend].numframes - ((num_sections - 2) * pStudioAnimDesc[blend].sectionframes + 1);
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
						mdlAnimRle = reinterpret_cast<pt2::mstudio_rle_anim_t*>(mdl_buffer - animbase_ptr + pStudioAnimDesc[blend].animindex + anim_block_offset);
					}

					r5r::mstudio_rle_anim_t* rseqAnimRle = reinterpret_cast<r5r::mstudio_rle_anim_t*>(g_model.pData);
					rseqAnimRle->flags = 0;
					int read_offset = sizeof(pt2::mstudio_rle_anim_t);
					int write_offset = sizeof(r5r::mstudio_rle_anim_t);
					int flags = 0;
					int tmp_r, tmp_p;

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
						Vector48* mdlRawpos = PTR_FROM_IDX(Vector48, mdlAnimRle, ((mdlAnimRle->flags & STUDIO_ANIM_RAWROT) ? 12 : read_offset));
						rseqRawpos->x = mdlRawpos->x;
						rseqRawpos->y = mdlRawpos->y;
						rseqRawpos->z = mdlRawpos->z;
						flags |= 0x1;
						write_offset += 6;
					}

					if (mdlAnimRle->flags & STUDIO_ANIM_RAWROT) {
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
						ProcessAnimValue(read_offset, write_offset, mdlAnimRle, pRseqPosV, num_frames, idx_offset, studioPosScale[mdlAnimRle->bone].Min(), studioPosScale[mdlAnimRle->bone].x, 0, !pMdlPosV->x);
						//pos y
						pRseqPosV->flags |= 2;
						read_offset = tmp_p + pMdlPosV->y;
						ProcessAnimValue(read_offset, write_offset, mdlAnimRle, pRseqPosV, num_frames, idx_offset, studioPosScale[mdlAnimRle->bone].Min(), studioPosScale[mdlAnimRle->bone].y, 0, !pMdlPosV->y);
						//pos z
						pRseqPosV->flags |= 1;
						read_offset = tmp_p + pMdlPosV->z;
						ProcessAnimValue(read_offset, write_offset, mdlAnimRle, pRseqPosV, num_frames, idx_offset, studioPosScale[mdlAnimRle->bone].Min(), studioPosScale[mdlAnimRle->bone].z, 0, !pMdlPosV->z);

						pRseqPosV->offset = idx_offset.at(0) * 2;
						pRseqPosV->idx1 = max(idx_offset.at(1) - idx_offset.at(0), 0);
						pRseqPosV->idx2 = max(idx_offset.at(2) - idx_offset.at(0), 0);

					}

					//animrot
					idx_offset.clear();
					if (mdlAnimRle->flags & R5_ANIM_ANIMROT && !pMdlRotV->IsAllZero()) {
						rseqAnimRle->flags |= 0x2;
						//rot x
						pRseqRotV->flags |= 4;
						read_offset = tmp_r + pMdlRotV->x;
						ProcessAnimValue(read_offset, write_offset, mdlAnimRle, pRseqRotV, num_frames, idx_offset, 0.00019175345f, studioRotScale[mdlAnimRle->bone].x, rAnimDesc->flags & 0x4 ? 0 : studioRot[mdlAnimRle->bone].x, !pMdlRotV->x);
						//rot y
						pRseqRotV->flags |= 2;
						read_offset = tmp_r + pMdlRotV->y;
						ProcessAnimValue(read_offset, write_offset, mdlAnimRle, pRseqRotV, num_frames, idx_offset, 0.00019175345f, studioRotScale[mdlAnimRle->bone].y, rAnimDesc->flags & 0x4 ? 0 : studioRot[mdlAnimRle->bone].y, !pMdlRotV->y);
						//rot z
						pRseqRotV->flags |= 1;
						read_offset = tmp_r + pMdlRotV->z;
						ProcessAnimValue(read_offset, write_offset, mdlAnimRle, pRseqRotV, num_frames, idx_offset, 0.00019175345f, studioRotScale[mdlAnimRle->bone].z, rAnimDesc->flags & 0x4 ? 0 : studioRot[mdlAnimRle->bone].z, !pMdlRotV->z);

						pRseqRotV->offset = idx_offset.at(0) * 2;
						pRseqRotV->idx1 = max(idx_offset.at(1) - idx_offset.at(0), 0);
						pRseqRotV->idx2 = max(idx_offset.at(2) - idx_offset.at(0), 0);
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
			rAnimDesc->numikrules = 1;// pStudioAnimDesc->numikrules;
			rAnimDesc->ikruleindex = g_model.pData - g_model.pBase - rseq_blends[blend_idx];
			r5r::mstudioikrule_t* v49ikrule = reinterpret_cast<r5r::mstudioikrule_t*>(mdl_buffer - animbase_ptr + pStudioAnimDesc[blend].ikruleindex);
			r5r::mstudioikrule_t* ikrule = reinterpret_cast<r5r::mstudioikrule_t*>(g_model.pData);
			for (int i = 0; i < rAnimDesc->numikrules; i++) {
				//ikrule[i].chain = v49ikrule[i].chain;
				ikrule[i].type = 4; //v49ikrule[i].type;
				//ikrule[i].bone = v49ikrule[i].bone;
				//ikrule[i].start = v49ikrule[i].start;
				//ikrule[i].end = v49ikrule[i].end;
				ikrule[i].compressedikerrorindex = 136; //v49ikrule[i].compressedikerrorindex;
			}
			g_model.pData += rAnimDesc->numikrules * sizeof(r5r::mstudioikrule_t);
		}

		//unk   movement??
		pV7RseqDesc->unkCount = 0; //14
		pV7RseqDesc->unkOffset = g_model.pData - g_model.pBase;
		//r5::unkseqdata_t* movementdata = reinterpret_cast<r5::unkseqdata_t*>(g_model.pData);
		//movementdata->unk = 108;
		//movementdata->unkfloat3 = 344.9211f;
		//movementdata->unkfloat4 = 344.9211f;
		//g_model.pData += sizeof(r5::unkseqdata_t) * pV7RseqDesc->unkCount;

		//write all data
		g_model.pData = WriteStringTable(g_model.pData);
		ALIGN4(g_model.pData);

		outRseq.write(g_model.pBase, g_model.pData - g_model.pBase);
		g_model.stringTable.clear();
		delete[] g_model.pBase;

		printf("Done!\n");
	}
	return sequence_names;
}

void ProcessAnimValue(int& read_offset, int& write_offset, pt2::mstudio_rle_anim_t* mdlAnimRle, r5r::studioanimvalue_ptr_t* pRseqValue, int numframe, std::vector<int>& idx_offset, float newScale, float oldScale, float shift, bool isZero) {
	r5r::mstudioanimvalue_t* rseqanimvalue = PTR_FROM_IDX(r5r::mstudioanimvalue_t, g_model.pData, write_offset);
	idx_offset.push_back(rseqanimvalue - (r5r::mstudioanimvalue_t*)pRseqValue);

	rseqanimvalue[0].meta.valid = 0x0;
	rseqanimvalue[0].meta.total = numframe;
	write_offset += sizeof(short);

	std::vector<int> read_anim_value;
	if (!isZero) {
		int total_frame = 0;
		while (total_frame < numframe) {
			r5r::mstudioanimvalue_t* animvalue = PTR_FROM_IDX(r5r::mstudioanimvalue_t, mdlAnimRle, read_offset);
			for (int j = 0; j < animvalue->meta.valid; j++)
				read_anim_value.push_back(animvalue[j + 1].value);
			for (int j = 0; j < animvalue->meta.total - animvalue->meta.valid; j++)
				read_anim_value.push_back(animvalue[animvalue->meta.valid].value);
			read_offset += animvalue->meta.valid * 2 + 2;
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
		write_offset += sizeof(short);
	}
}