#include <mdl/rrig.h>
#include <mdl/studio.h>
#include <utils/stringtable.cpp>


void ProcessAnimValue(int& read_offset, int& write_offset, mstudio_rle_anim_t* mdlAnimRle, r5::studioanimvalue_ptr_t* pRseqValue, int numframe, std::vector<int>& idx_offset, float newScale, float oldScale);

void ConvertMDL_RSEQ(char* mdl_buffer, std::string output_dir, std::string filename) {
	studiohdr_t* pV49MdlHdr = reinterpret_cast<studiohdr_t*>(mdl_buffer);
	studiohdr2_t* pV49MdlHdr2 = PTR_FROM_IDX(studiohdr2_t, mdl_buffer, sizeof(studiohdr_t));
	mstudioseqdesc_t* pStudioSeqDesc = reinterpret_cast<mstudioseqdesc_t*>((mdl_buffer + pV49MdlHdr->localseqindex));
	mstudioanimdesc_t* pStudioAnimDesc = reinterpret_cast<mstudioanimdesc_t*>((mdl_buffer + pV49MdlHdr->localanimindex));

	mstudiolinearbone_t* linearboneindexes = PTR_FROM_IDX(mstudiolinearbone_t, mdl_buffer, pV49MdlHdr2->srcbonetransformindex);
	Vector3* studioPosScale = PTR_FROM_IDX(Vector3, linearboneindexes, linearboneindexes->posscaleindex);
	Vector3* studioRotScale = PTR_FROM_IDX(Vector3, linearboneindexes, linearboneindexes->rotscaleindex);

	for (int seq_idx = 0; seq_idx < pV49MdlHdr->numlocalseq; seq_idx++) {
		printf("Converting sequence %d/%d\n", seq_idx + 1, pV49MdlHdr->numlocalseq);

		int base_ptr = pStudioSeqDesc[seq_idx].baseptr;

		std::string model_folder = pV49MdlHdr->name;
		std::string seq_name = STRING_FROM_IDX(reinterpret_cast<char*>(mdl_buffer - base_ptr), pStudioSeqDesc[seq_idx].sznameindex);

		model_folder = "animseq/" + model_folder.substr(model_folder.find('/') + 1, model_folder.rfind('.') - model_folder.find('/') - 1);
		std::string seqdescname = model_folder + "/" + seq_name + ".rseq";
		std::string output_path = output_dir + "/" + seqdescname;
		std::replace(output_path.begin(), output_path.end(), '/', '\\');
		std::replace(seqdescname.begin(), seqdescname.end(), '\\', '/');

		std::replace(model_folder.begin(), model_folder.end(), '/', '\\');
		std::filesystem::create_directories(output_dir + "\\" + model_folder);
		
		std::ofstream outRseq(output_path, std::ios::out | std::ios::binary);
		printf("    ->%s\n", seqdescname.c_str());

		std::string seq_gen_name = model_folder + "_" + seq_name;
		seq_gen_name = seq_gen_name.substr(seq_gen_name.find_last_of('\\') + 1);

		//header
		g_model.pBase = new char[32 * 1024 * 1024] {};
		g_model.pData = g_model.pBase;
		r5::v8::mstudioseqdesc_t* pV7RseqDesc = reinterpret_cast<r5::v8::mstudioseqdesc_t*>(g_model.pData);
		pV7RseqDesc->flags = pStudioSeqDesc[seq_idx].flags;
		pV7RseqDesc->activity = -1.0f;
		pV7RseqDesc->actweight = 1.0f;
		pV7RseqDesc->numevents = pStudioSeqDesc[seq_idx].numevents;
		pV7RseqDesc->bbmin = pStudioSeqDesc[seq_idx].bbmin;
		pV7RseqDesc->bbmax = pStudioSeqDesc[seq_idx].bbmax;
		pV7RseqDesc->numblends = 1; //
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
		pV7RseqDesc->entryphase = pStudioSeqDesc[seq_idx].entryphase;
		pV7RseqDesc->exitphase = pStudioSeqDesc[seq_idx].exitphase;
		pV7RseqDesc->numikrules = 1;
		g_model.pHdr = pV7RseqDesc;
		g_model.pData += sizeof(r5::v8::mstudioseqdesc_t);
		BeginStringTable();


		AddToStringTable((char*)pV7RseqDesc, &pV7RseqDesc->szlabelindex, seqdescname.c_str());
		AddToStringTable((char*)pV7RseqDesc, &pV7RseqDesc->szactivitynameindex, STRING_FROM_IDX(reinterpret_cast<char*>(mdl_buffer - base_ptr), pStudioSeqDesc[seq_idx].szactivitynameindex));

		//posekey
		int numposekey = pV7RseqDesc->groupsize[0] + pV7RseqDesc->groupsize[1];
		pV7RseqDesc->posekeyindex = 0; //g_model.pData - g_model.pBase;
		//g_model.pData += sizeof(int) * (numposekey);

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
			weight[i] = 1.0f;
		}
		g_model.pData += sizeof(int) * pV49MdlHdr->numbones;

		//blends
		pV7RseqDesc->activitymodifierindex = g_model.pData - g_model.pBase;
		pV7RseqDesc->animindexindex = g_model.pData - g_model.pBase;
		int* blend = reinterpret_cast<int*>(g_model.pData);
		g_model.pData += sizeof(int) * pV7RseqDesc->numblends;

		//animdesc TODO:support multiple anim
		r5::v8::mstudioanimdesc_t* rAnimDesc = reinterpret_cast<r5::v8::mstudioanimdesc_t*>(g_model.pData);
		*blend = g_model.pData - g_model.pBase;
		rAnimDesc->fps = pStudioAnimDesc[seq_idx].fps;
		rAnimDesc->flags = (pStudioAnimDesc->numframes > 0 ? 0x20000 : 0x0) | pStudioAnimDesc[seq_idx].flags;
		rAnimDesc->numframes = pStudioAnimDesc[seq_idx].numframes;
		//rAnimDesc->animindex = g_model.pData - g_model.pBase;
		AddToStringTable((char*)rAnimDesc, &rAnimDesc->sznameindex, seq_gen_name.c_str());
		g_model.pData += sizeof(r5::v8::mstudioanimdesc_t);
		ALIGN16(g_model.pData);


		//anims
		mstudioanimsections_t* animsections = nullptr;
		unsigned int* sections_index{};
		int animbase_ptr = pStudioAnimDesc[seq_idx].baseptr;
		int num_frames = pStudioAnimDesc[seq_idx].numframes;
		bool hasSections = false;

		size_t num_sections = 1;
		if (pStudioAnimDesc[seq_idx].sectionindex) {
			hasSections = true;
			num_sections = (pStudioAnimDesc[seq_idx].animindex - pStudioAnimDesc[seq_idx].sectionindex) / 8 - 1;
			animsections = PTR_FROM_IDX(mstudioanimsections_t, mdl_buffer - animbase_ptr, pStudioAnimDesc[seq_idx].sectionindex);
			sections_index = reinterpret_cast<unsigned int*>(g_model.pData);
			//rAnimDesc->sectionindex = g_model.pData - g_model.pBase;
			//rAnimDesc->numframes = pStudioAnimDesc[seq_idx].numframes;
			g_model.pData += 4 * num_sections;
		}

		rAnimDesc->animindex = g_model.pData - (char*)rAnimDesc;
		rAnimDesc->sectionindex = hasSections ? rAnimDesc->animindex - num_sections * 4 : 0 ;
		rAnimDesc->sectionframes = pStudioAnimDesc[seq_idx].sectionframes;

		// TODO:
		for (size_t section = 0; section < num_sections; section++) {

			if (hasSections) {
				sections_index[section] = g_model.pData - (char*)rAnimDesc;
				num_frames = pStudioAnimDesc[seq_idx].sectionframes;

				if (section + 2 == num_sections) 
					num_frames = (pStudioAnimDesc[seq_idx].numframes % pStudioAnimDesc[seq_idx].sectionframes);
			}

			//boneflagarray (allocate)
			char* boneflagarray = reinterpret_cast<char*>(g_model.pData);
			std::vector<unsigned int> flaggedbones(pV49MdlHdr->numbones + 1, 0);
			g_model.pData += (size_t)(pV49MdlHdr->numbones + 2) / 2 ;

			//animvalue
			int anim_block_offset = 0;
			for (int i = 0; i < pV49MdlHdr->numbones; i++) {
				mstudio_rle_anim_t* mdlAnimRle{};

				if (hasSections) {
					mdlAnimRle = PTR_FROM_IDX(mstudio_rle_anim_t, mdl_buffer - animbase_ptr, animsections[section].animindex + anim_block_offset);
				} else {
					mdlAnimRle = PTR_FROM_IDX(mstudio_rle_anim_t, mdl_buffer - animbase_ptr, pStudioAnimDesc[seq_idx].animindex + anim_block_offset);
				}

				r5::mstudio_rle_anim_t* rseqAnimRle = reinterpret_cast<r5::mstudio_rle_anim_t*>(g_model.pData);
				rseqAnimRle->flags = 0;
				int read_offset = sizeof(mstudio_rle_anim_t);
				int write_offset = sizeof(r5::mstudio_rle_anim_t);
				int flags = 0;
				int tmp_r, tmp_p;

				r5::studioanimvalue_ptr_t* pRseqRotV{};
				r5::studioanimvalue_ptr_t* pRseqPosV{};
				studioanimvalue_ptr_t* pMdlRotV{};
				studioanimvalue_ptr_t* pMdlPosV{};

				//alloc posscale
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
					if (!pMdlRotV->IsAllZero()) {
						printf("%d   rotv: %d %d %d\n", mdlAnimRle->bone, pMdlRotV->x, pMdlRotV->y, pMdlRotV->z);
						//studioRotScale[mdlAnimRle->bone].Print(mdlAnimRle->bone);
					}
				}
				if (mdlAnimRle->flags & RTECH_ANIM_ANIMPOS) {
					pMdlPosV = PTR_FROM_IDX(studioanimvalue_ptr_t, mdlAnimRle, read_offset);
					tmp_p = read_offset;
					read_offset += sizeof(studioanimvalue_ptr_t);
					if (!pMdlPosV->IsAllZero()) {
						printf("%d   posv: %d %d %d\n", mdlAnimRle->bone, pMdlPosV->x, pMdlPosV->y, pMdlPosV->z);
						//studioPosScale[mdlAnimRle->bone].Print(mdlAnimRle->bone);
					}
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

				if (flags) {
					printf("section:%d bone:%d (%d)\n", section, mdlAnimRle->bone, num_frames);
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


		//ikrules
		rAnimDesc->numikrules = 1;
		rAnimDesc->ikruleindex = g_model.pData - g_model.pBase - *blend;
		r5::v8::mstudioikrule_t* ikrule = reinterpret_cast<r5::v8::mstudioikrule_t*>(g_model.pData);
		ikrule->type = 4;
		ikrule->compressedikerrorindex = 136;
		g_model.pData += sizeof(r5::v8::mstudioikrule_t);

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

		//if (animvalue->meta.valid <= 0 || animvalue->meta.total <= 0) break;
		for (int j = 0; j < animvalue->meta.valid; j++) read_anim_value.push_back(animvalue[j + 1].value);
		for (int j = 0; j < animvalue->meta.total - animvalue->meta.valid; j++) read_anim_value.push_back(animvalue[animvalue->meta.valid].value);

		read_offset += animvalue->meta.valid * 2 + 2;
		total_frame += animvalue->meta.total;
	};
	printf("  %d/%d %d\n", total_frame, numframe, -pRseqValue->flags);

	for (int j = 0; j < numframe; j++) {
		rseqanimvalue[j + 1].value = ((double)read_anim_value.at(j) * oldScale / newScale);
		write_offset += sizeof(short);
		//printf(" %d", rseqanimvalue[j + 1].value);
	}
	//printf(" \n");
}

