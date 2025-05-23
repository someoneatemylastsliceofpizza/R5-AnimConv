#include <mdl/rrig.h>
#include <mdl/studio.h>
#include <utils/stringtable.cpp>


void ProcessAnimValue(int& read_offset, int& write_offset, mstudio_rle_anim_t* mdlAnimRle, r5::studioanimvalue_ptr_t* pRseqValue, int numframe, std::vector<int>& idx_offset, float newScale, float oldScale);

std::vector<std::string> ConvertMDL_RSEQ(char* mdl_buffer, std::string output_dir, std::string filename, std::vector<std::pair<std::pair<int, int>, int>> &nodedata) {
	studiohdr_t* pV49MdlHdr = reinterpret_cast<studiohdr_t*>(mdl_buffer);
	studiohdr2_t* pV49MdlHdr2 = PTR_FROM_IDX(studiohdr2_t, mdl_buffer, sizeof(studiohdr_t));
	mstudioseqdesc_t* pStudioSeqDesc = reinterpret_cast<mstudioseqdesc_t*>((mdl_buffer + pV49MdlHdr->localseqindex));
	mstudioanimdesc_t* pStudioAnimDesc = reinterpret_cast<mstudioanimdesc_t*>((mdl_buffer + pV49MdlHdr->localanimindex));

	mstudiolinearbone_t* linearboneindexes = PTR_FROM_IDX(mstudiolinearbone_t, mdl_buffer, pV49MdlHdr2->srcbonetransformindex);
	Vector3* studioPosScale = PTR_FROM_IDX(Vector3, linearboneindexes, linearboneindexes->posscaleindex);
	Vector3* studioRotScale = PTR_FROM_IDX(Vector3, linearboneindexes, linearboneindexes->rotscaleindex);

	std::vector<std::string> sequence_names;

	for (int seq_idx = 0; seq_idx < pV49MdlHdr->numlocalseq; seq_idx++) {
		printf("Converting sequence %d/%d\n", seq_idx + 1, pV49MdlHdr->numlocalseq);

		int base_ptr = pStudioSeqDesc[seq_idx].baseptr;

		std::string model_dir = "";
		std::string seq_name = STRING_FROM_IDX(reinterpret_cast<char*>(mdl_buffer - base_ptr), pStudioSeqDesc[seq_idx].sznameindex);

		std::string seqdescname = seq_name + ".rseq";
		size_t pos = seq_name.rfind('/');
		model_dir = (pos != std::string::npos) ? seq_name.substr(0, pos) : "";
		std::string output_path = output_dir + "/" + seqdescname;
		std::replace(output_path.begin(), output_path.end(), '/', '\\');
		std::replace(model_dir.begin(), model_dir.end(), '/', '\\');
		std::filesystem::create_directories(output_dir + "\\" + model_dir);

		std::ofstream outRseq(output_path, std::ios::out | std::ios::binary);
		printf("    ->%s\n", seqdescname.c_str());
		if (seqdescname != "ref.rseq")
			sequence_names.push_back(seqdescname);

		if(pStudioSeqDesc[seq_idx].localentrynode != pStudioSeqDesc[seq_idx].localexitnode)
			nodedata.push_back({ {pStudioSeqDesc[seq_idx].localentrynode , pStudioSeqDesc[seq_idx].localexitnode}, seq_idx - 1 /*doesn't include ref.rseq*/});

		//header
		g_model.pBase = new char[32 * 1024 * 1024] {};
		g_model.pData = g_model.pBase;
		r5::v8::mstudioseqdesc_t* pV7RseqDesc = reinterpret_cast<r5::v8::mstudioseqdesc_t*>(g_model.pData);
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
		pV7RseqDesc->paramend[0] = pStudioSeqDesc[seq_idx].paramend[0];
		pV7RseqDesc->paramend[1] = pStudioSeqDesc[seq_idx].paramend[1];
		pV7RseqDesc->paramparent = pStudioSeqDesc[seq_idx].paramparent;
		pV7RseqDesc->fadeintime = pStudioSeqDesc[seq_idx].fadeintime;
		pV7RseqDesc->fadeouttime = pStudioSeqDesc[seq_idx].fadeouttime;
		pV7RseqDesc->localentrynode = pStudioSeqDesc[seq_idx].localentrynode;
		pV7RseqDesc->localexitnode = pStudioSeqDesc[seq_idx].localexitnode;
		pV7RseqDesc->entryphase = pStudioSeqDesc[seq_idx].entryphase;
		pV7RseqDesc->exitphase = pStudioSeqDesc[seq_idx].exitphase;
		pV7RseqDesc->numikrules = 1;
		g_model.pHdr = pV7RseqDesc;
		g_model.pData += sizeof(r5::v8::mstudioseqdesc_t);
		BeginStringTable();


		AddToStringTable((char*)pV7RseqDesc, &pV7RseqDesc->szlabelindex, seqdescname.c_str());
		AddToStringTable((char*)pV7RseqDesc, &pV7RseqDesc->szactivitynameindex, STRING_FROM_IDX(reinterpret_cast<char*>(mdl_buffer - base_ptr), pStudioSeqDesc[seq_idx].szactivitynameindex));

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
		r5::v8::mstudioevent_t* rseqEvent = reinterpret_cast<r5::v8::mstudioevent_t*>(g_model.pData);
		mstudioevent_t* studioEvent = reinterpret_cast<mstudioevent_t*>(mdl_buffer - base_ptr + pStudioSeqDesc[seq_idx].eventindex);
		for (int i = 0; i < pV7RseqDesc->numevents; i++) {
			rseqEvent[i].cycle = studioEvent[i].cycle;
			rseqEvent[i].event = studioEvent[i].event;
			rseqEvent[i].type = studioEvent[i].type;
			memcpy_s(rseqEvent[i].options, 256, studioEvent[i].options, 64);
			AddToStringTable((char*)&rseqEvent[i], &rseqEvent[i].szeventindex, STRING_FROM_IDX(&studioEvent[i], studioEvent[i].szeventindex));
		}
		g_model.pData += sizeof(r5::v8::mstudioevent_t) * pV7RseqDesc->numevents;

		//weight list
		pV7RseqDesc->weightlistindex = g_model.pData - g_model.pBase;
		float* weight = reinterpret_cast<float*>(g_model.pData);
		for (int i = 0; i < pV49MdlHdr->numbones; i++) {
			weight[i] = 1.0f;//
		}
		g_model.pData += sizeof(int) * pV49MdlHdr->numbones;

		//activity modifiers
		pV7RseqDesc->activitymodifierindex = g_model.pData - g_model.pBase;
		pV7RseqDesc->numactivitymodifiers = pStudioSeqDesc[seq_idx].numactivitymodifiers;
		int* mstudioActivityModifier = PTR_FROM_IDX(int, &pStudioSeqDesc[seq_idx], pStudioSeqDesc[seq_idx].activitymodifierindex);
		r5::v8::mstudioactivitymodifier_t* rseqActivityModifier = reinterpret_cast<r5::v8::mstudioactivitymodifier_t*>(g_model.pData);
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
		g_model.pData += sizeof(r5::v8::mstudioactivitymodifier_t) * pV7RseqDesc->numactivitymodifiers;


		//blends
		pV7RseqDesc->animindexindex = g_model.pData - g_model.pBase;
		short* studio_blends = PTR_FROM_IDX(short, &pStudioSeqDesc[seq_idx], pStudioSeqDesc[seq_idx].animindexindex);
		int* rseq_blends = reinterpret_cast<int*>(g_model.pData);
		g_model.pData += sizeof(int) * pV7RseqDesc->numblends;

		for (int blend_idx = 0; blend_idx < pV7RseqDesc->numblends; blend_idx++) {
			//animdesc 
			short blend = studio_blends[blend_idx];
			r5::v8::mstudioanimdesc_t* rAnimDesc = reinterpret_cast<r5::v8::mstudioanimdesc_t*>(g_model.pData);
			char* anim_name = (char *)STRING_FROM_IDX(mdl_buffer, pStudioAnimDesc[blend].sznameindex - pStudioAnimDesc[blend].baseptr);
			rseq_blends[blend_idx] = g_model.pData - g_model.pBase;
			rAnimDesc->fps = pStudioAnimDesc[blend].fps;
			rAnimDesc->flags = (pStudioAnimDesc[blend].numframes > 0 ? 0x20000 : 0x0) | pStudioAnimDesc[blend].flags;
			rAnimDesc->numframes = pStudioAnimDesc[blend].numframes;
			AddToStringTable(reinterpret_cast<char*>(rAnimDesc), &rAnimDesc->sznameindex, anim_name);
			g_model.pData += sizeof(r5::v8::mstudioanimdesc_t);
			ALIGN16(g_model.pData);

			//anims
			mstudioanimsections_t* animsections = nullptr;
			unsigned int* sections_index{};
			int animbase_ptr = pStudioAnimDesc[blend].baseptr;
			int num_frames = pStudioAnimDesc[blend].numframes;
			bool hasSections = false;

			size_t num_sections = 1;
			if (pStudioAnimDesc[blend].sectionindex) {
				hasSections = true;
				num_sections = (pStudioAnimDesc[blend].animindex - pStudioAnimDesc[blend].sectionindex) / 8 - 1;
				animsections = PTR_FROM_IDX(mstudioanimsections_t, mdl_buffer - animbase_ptr, pStudioAnimDesc[blend].sectionindex);
				sections_index = reinterpret_cast<unsigned int*>(g_model.pData);
				g_model.pData += 4 * num_sections;
			}

			rAnimDesc->animindex = g_model.pData - (char*)rAnimDesc;
			rAnimDesc->sectionindex = hasSections ? rAnimDesc->animindex - num_sections * 4 : 0;
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
					mstudio_rle_anim_t* mdlAnimRle{};

					if (hasSections) {
						mdlAnimRle = PTR_FROM_IDX(mstudio_rle_anim_t, mdl_buffer - animbase_ptr, animsections[section].animindex + anim_block_offset);
					}
					else {
						mdlAnimRle = PTR_FROM_IDX(mstudio_rle_anim_t, mdl_buffer - animbase_ptr, pStudioAnimDesc[blend].animindex + anim_block_offset);
					}

					r5::mstudio_rle_anim_t* rseqAnimRle = reinterpret_cast<r5::mstudio_rle_anim_t*>(g_model.pData);
					rseqAnimRle->flags = 0;
					int read_offset = sizeof(mstudio_rle_anim_t);
					int write_offset = sizeof(r5::mstudio_rle_anim_t);
					int flags = 0;
					bool additive_flag = 0;//pV7RseqDesc->flags & 0x0004;
					int tmp_r, tmp_p;

					r5::studioanimvalue_ptr_t* pRseqRotV{};
					r5::studioanimvalue_ptr_t* pRseqPosV{};
					studioanimvalue_ptr_t* pMdlRotV{};
					studioanimvalue_ptr_t* pMdlPosV{};

					//posscale
					if (mdlAnimRle->flags & RTECH_ANIM_ANIMPOS) {
						float* posscale = PTR_FROM_IDX(float, g_model.pData, write_offset);
						*posscale = studioPosScale[i].Min();
						write_offset += 4;
					}
					//raw data
					if (mdlAnimRle->flags & RTECH_ANIM_RAWPOS) {
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
					if (mdlAnimRle->flags & RTECH_ANIM_ANIMROT) {
						pMdlRotV = PTR_FROM_IDX(studioanimvalue_ptr_t, mdlAnimRle, read_offset);
						tmp_r = read_offset;
						read_offset += sizeof(studioanimvalue_ptr_t);
						//if (!pMdlRotV->IsAllZero()) {
							//printf("%d   rotv: %d %d %d\n", mdlAnimRle->bone, pMdlRotV->x, pMdlRotV->y, pMdlRotV->z);
							//studioRotScale[mdlAnimRle->bone].Print(mdlAnimRle->bone);
						//}
					}
					if (mdlAnimRle->flags & RTECH_ANIM_ANIMPOS) {
						pMdlPosV = PTR_FROM_IDX(studioanimvalue_ptr_t, mdlAnimRle, read_offset);
						tmp_p = read_offset;
						read_offset += sizeof(studioanimvalue_ptr_t);
						//if (!pMdlPosV->IsAllZero()) {
							//printf("%d   posv: %d %d %d\n", mdlAnimRle->bone, pMdlPosV->x, pMdlPosV->y, pMdlPosV->z);
							//studioPosScale[mdlAnimRle->bone].Print(mdlAnimRle->bone);
						//}
					}


					if (mdlAnimRle->flags & RTECH_ANIM_ANIMPOS && !pMdlPosV->IsAllZero()) {
						pRseqPosV = PTR_FROM_IDX(r5::studioanimvalue_ptr_t, g_model.pData, write_offset);
						write_offset += sizeof(r5::studioanimvalue_ptr_t);
						flags |= 0x1;
					}

					if (mdlAnimRle->flags & RTECH_ANIM_ANIMROT && !pMdlRotV->IsAllZero()) {
						pRseqRotV = PTR_FROM_IDX(r5::studioanimvalue_ptr_t, g_model.pData, write_offset);
						write_offset += sizeof(r5::studioanimvalue_ptr_t);
						flags |= 0x2;

					}

					//write animvalue
					std::vector<int> idx_offset;
					if (mdlAnimRle->flags & RTECH_ANIM_ANIMPOS && !pMdlPosV->IsAllZero()) {
						rseqAnimRle->flags |= 0x4;
						if (pMdlPosV->x) {
							pRseqPosV->flags |= 4;
							read_offset = tmp_p + pMdlPosV->x;
							ProcessAnimValue(read_offset, write_offset, mdlAnimRle, pRseqPosV, num_frames, idx_offset, studioPosScale[mdlAnimRle->bone].Min(), studioPosScale[mdlAnimRle->bone].x);
						}
						else idx_offset.push_back(0);
						if (pMdlPosV->y) {
							pRseqPosV->flags |= 2;
							read_offset = tmp_p + pMdlPosV->y;
							ProcessAnimValue(read_offset, write_offset, mdlAnimRle, pRseqPosV, num_frames, idx_offset, studioPosScale[mdlAnimRle->bone].Min(), studioPosScale[mdlAnimRle->bone].y);
						}
						else idx_offset.push_back(0);
						if (pMdlPosV->z) {
							pRseqPosV->flags |= 1;
							read_offset = tmp_p + pMdlPosV->z;
							ProcessAnimValue(read_offset, write_offset, mdlAnimRle, pRseqPosV, num_frames, idx_offset, studioPosScale[mdlAnimRle->bone].Min(), studioPosScale[mdlAnimRle->bone].z);
						}
						else idx_offset.push_back(0);
						idx_offset.push_back(0);
						idx_offset.push_back(0);

						while (!idx_offset.empty() && idx_offset.front() == 0) idx_offset.erase(idx_offset.begin());

						pRseqPosV->offset = idx_offset.at(0) * 2;
						pRseqPosV->idx1 = max(idx_offset.at(1) - idx_offset.at(0), 0);
						pRseqPosV->idx2 = max(idx_offset.at(2) - idx_offset.at(0), 0);

					}

					idx_offset.clear();
					if (mdlAnimRle->flags & RTECH_ANIM_ANIMROT && !pMdlRotV->IsAllZero()) {
						rseqAnimRle->flags |= 0x2;
						if (pMdlRotV->x) {
							pRseqRotV->flags |= 4;
							read_offset = tmp_r + pMdlRotV->x;
							ProcessAnimValue(read_offset, write_offset, mdlAnimRle, pRseqRotV, num_frames, idx_offset, 0.00019175345f, studioRotScale[mdlAnimRle->bone].x);
						}
						else idx_offset.push_back(0);
						if (pMdlRotV->y) {
							pRseqRotV->flags |= 2;
							read_offset = tmp_r + pMdlRotV->y;
							ProcessAnimValue(read_offset, write_offset, mdlAnimRle, pRseqRotV, num_frames, idx_offset, 0.00019175345f, studioRotScale[mdlAnimRle->bone].y);
						}
						else idx_offset.push_back(0);
						if (pMdlRotV->z) {
							pRseqRotV->flags |= 1;
							read_offset = tmp_r + pMdlRotV->z;
							ProcessAnimValue(read_offset, write_offset, mdlAnimRle, pRseqRotV, num_frames, idx_offset, 0.00019175345f, studioRotScale[mdlAnimRle->bone].z);
						}
						else idx_offset.push_back(0);
						idx_offset.push_back(0);
						idx_offset.push_back(0);

						while (!idx_offset.empty() && idx_offset.front() == 0) idx_offset.erase(idx_offset.begin()); //remove 0s in the front

						pRseqRotV->offset = idx_offset.at(0) * 2;
						pRseqRotV->idx1 = max(idx_offset.at(1) - idx_offset.at(0), 0);
						pRseqRotV->idx2 = max(idx_offset.at(2) - idx_offset.at(0), 0);
					}

					/*	if (flags) {
							printf("section:%d/%d bone:%d (%d)\n", section, num_sections, mdlAnimRle->bone, num_frames);
						}*/

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
			/*rAnimDesc->numikrules = pStudioAnimDesc->numikrules;
			rAnimDesc->ikruleindex = g_model.pData - g_model.pBase - rseq_blends[blend_idx];
			r5::v8::mstudioikrule_t* v49ikrule = PTR_FROM_IDX(r5::v8::mstudioikrule_t, mdl_buffer - animbase_ptr, pStudioAnimDesc[blend].ikruleindex);
			r5::v8::mstudioikrule_t* ikrule = reinterpret_cast<r5::v8::mstudioikrule_t*>(g_model.pData);
			for (int i = 0; i < rAnimDesc->numikrules; i++) {
				ikrule[i].chain = v49ikrule[i].chain;
				ikrule[i].type = v49ikrule[i].type;
				ikrule[i].bone = v49ikrule[i].bone;
				ikrule[i].start = v49ikrule[i].start;
				ikrule[i].end = v49ikrule[i].end;
				ikrule[i].compressedikerrorindex = v49ikrule[i].compressedikerrorindex;
			}
			g_model.pData += sizeof(r5::v8::mstudioikrule_t);*/
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

void ProcessAnimValue(int& read_offset, int& write_offset, mstudio_rle_anim_t* mdlAnimRle, r5::studioanimvalue_ptr_t* pRseqValue, int numframe, std::vector<int>& idx_offset, float newScale, float oldScale) {
	r5::mstudioanimvalue_t* rseqanimvalue = PTR_FROM_IDX(r5::mstudioanimvalue_t, g_model.pData, write_offset);
	idx_offset.push_back(rseqanimvalue - (r5::mstudioanimvalue_t*)pRseqValue);

	rseqanimvalue[0].meta.valid = 0x0;      //TODO: support compressed anim?
	rseqanimvalue[0].meta.total = numframe;
	write_offset += sizeof(short);

	int total_frame = 0;
	std::vector<int> read_anim_value;
	while (total_frame < numframe) {
		r5::mstudioanimvalue_t* animvalue = PTR_FROM_IDX(r5::mstudioanimvalue_t, mdlAnimRle, read_offset);

		for (int j = 0; j < animvalue->meta.valid; j++) read_anim_value.push_back(animvalue[j + 1].value);
		for (int j = 0; j < animvalue->meta.total - animvalue->meta.valid; j++) read_anim_value.push_back(animvalue[animvalue->meta.valid].value);

		read_offset += animvalue->meta.valid * 2 + 2;
		total_frame += animvalue->meta.total;
	};
	//printf("  %d/%d %d\n", total_frame, numframe, -pRseqValue->flags);

	for (int j = 0; j < numframe; j++) {
		rseqanimvalue[j + 1].value = ((double)read_anim_value.at(j) * oldScale / newScale);
		write_offset += sizeof(short);
		//printf(" %d", rseqanimvalue[j + 1].value);
	}
	//printf(" \n");
}

