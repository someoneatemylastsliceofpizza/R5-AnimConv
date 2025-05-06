#include <mdl/rrig.h>
#include <mdl/studio.h>
#include <utils/stringtable.cpp>


void ProcessAnimValue(int& read_offset, int& write_offset, mstudio_rle_anim_t* mdlAnimRle, r5::studioanimvalue_ptr_t* pRseqValue, int numframe, std::vector<int>& idx_offset) {
	unsigned short* animvalue = PTR_FROM_IDX(unsigned short, g_model.pData, write_offset);
	idx_offset.push_back((animvalue - (unsigned short*)pRseqValue));

	animvalue[0] = 0x0 | (numframe << 8); //animvalue metadata
	write_offset += sizeof(short);

	int total_frame = 0;
	std::vector<int> read_anim_value;
	do {
		unsigned char* metadata = PTR_FROM_IDX(unsigned char, mdlAnimRle, read_offset);
		for (int j = 0; j < metadata[0]; j++) read_anim_value.push_back(metadata[j + 1]);
		for (int j = 0; j < metadata[1] - metadata[0]; j++) read_anim_value.push_back(0);

		read_offset += metadata[0] * 2 + 2;
		total_frame += metadata[1];
	} while (total_frame < numframe);

	printf("  %d\n", read_anim_value.size());

	for (int j = 0; j < numframe; j++) {
		animvalue[j + 1] = read_anim_value.at(j);
		write_offset += sizeof(short);
	}
}

void ConvertMDL_RSEQ(char* mdl_buffer, std::string output_dir, std::string filename) {
	studiohdr_t* pV49MdlHdr = reinterpret_cast<studiohdr_t*>(mdl_buffer);
	mstudioseqdesc_t* pStudioSeqDesc = reinterpret_cast<mstudioseqdesc_t*>((mdl_buffer + pV49MdlHdr->localseqindex));
	mstudioanimdesc_t* pStudioAnimDesc = reinterpret_cast<mstudioanimdesc_t*>((mdl_buffer + pV49MdlHdr->localanimindex));

	for (int seq_idx = 0; seq_idx < pV49MdlHdr->numlocalseq; seq_idx++) {
		printf("Converting sequence %d/%d\n", seq_idx, pV49MdlHdr->numlocalseq);

		int base_ptr = pStudioSeqDesc[seq_idx].baseptr;

		std::string model_folder = pV49MdlHdr->name;
		std::string seq_name = STRING_FROM_IDX(reinterpret_cast<char*>(mdl_buffer - base_ptr), pStudioSeqDesc[seq_idx].sznameindex);

		model_folder = "animseq/" + model_folder.substr(model_folder.find('/') + 1, model_folder.rfind('.') - model_folder.find('/') - 1);
		std::string seqdescname = model_folder + "/" + seq_name + ".rseq";
		std::string output_path = output_dir + "/" + seqdescname;
		std::replace(output_path.begin(), output_path.end(), '/', '\\');
		std::replace(seqdescname.begin(), seqdescname.end(), '\\', '/');
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

		//animdesc TODO:support many anims
		r5::v8::mstudioanimdesc_t* rAnimDesc = reinterpret_cast<r5::v8::mstudioanimdesc_t*>(g_model.pData);
		*blend = g_model.pData - g_model.pBase;
		rAnimDesc->fps = pStudioAnimDesc[seq_idx].fps;
		rAnimDesc->flags = 0x20000 | pStudioAnimDesc[seq_idx].flags;
		rAnimDesc->numframes = pStudioAnimDesc[seq_idx].numframes;
		rAnimDesc->animindex = g_model.pData - g_model.pBase;
		AddToStringTable((char*)rAnimDesc, &rAnimDesc->sznameindex, seq_gen_name.c_str());
		g_model.pData += sizeof(r5::v8::mstudioanimdesc_t);
		ALIGN16(g_model.pData);

		//anim
		//boneflagarray (allocate)
		rAnimDesc->animindex = g_model.pData - g_model.pBase - rAnimDesc->animindex;
		char* boneflagarray = reinterpret_cast<char*>(g_model.pData);
		std::vector<unsigned int> flaggedbones;
		g_model.pData += (size_t)(pV49MdlHdr->numbones + 1) / 2 + 1;

		//anim data
		int anim_block_offset = 0;
		int animbase_ptr = pStudioAnimDesc[seq_idx].baseptr;
		for (int i = 0; i < pV49MdlHdr->numbones; i++) {
            mstudio_rle_anim_t* mdlAnimRle = PTR_FROM_IDX(mstudio_rle_anim_t, mdl_buffer - animbase_ptr, pStudioAnimDesc[seq_idx].animindex + anim_block_offset);
			r5::mstudio_rle_anim_t* rseqAnimRle = reinterpret_cast<r5::mstudio_rle_anim_t*>(g_model.pData);
			rseqAnimRle->flags = 0;
			int read_offset = sizeof(mstudio_rle_anim_t);
			int write_offset = sizeof(r5::mstudio_rle_anim_t);
			int flags = mdlAnimRle->flags;

			r5::studioanimvalue_ptr_t* pRseqRotV{};
			r5::studioanimvalue_ptr_t* pRseqPosV{};
			studioanimvalue_ptr_t* pMdlRotV{};
			studioanimvalue_ptr_t* pMdlPosV{};
			printf("bone %d\n", i);

			//alloc posscale
			if (flags & RTECH_ANIM_ANIMPOS) {
				float* posscale = PTR_FROM_IDX(float, g_model.pData, write_offset);
				*posscale = 1. / 256.; //TODO: get it from linearbone
				write_offset += 4;
			}

			//raw data
			if (mdlAnimRle->flags & RTECH_ANIM_RAWPOS) {
				Vector48* rseqRawpos = PTR_FROM_IDX(Vector48, g_model.pData, write_offset);
				Vector48* mdlRawpos = PTR_FROM_IDX(Vector48, mdlAnimRle, ((mdlAnimRle->flags & STUDIO_ANIM_RAWROT) ? 12 : read_offset));
				rseqRawpos->x = mdlRawpos->x;
				rseqRawpos->y = mdlRawpos->y;
				rseqRawpos->z = mdlRawpos->z;
				write_offset += 6;
			}

			if (mdlAnimRle->flags & STUDIO_ANIM_RAWROT) {
				flags ^= 0x22;
				Quaternion64 * rseqRawrot= PTR_FROM_IDX(Quaternion64, g_model.pData, write_offset);
				Quaternion64* mdlRawrot = PTR_FROM_IDX(Quaternion64, mdlAnimRle, read_offset);
				rseqRawrot->x = mdlRawrot->x;
				rseqRawrot->y = mdlRawrot->y;
				rseqRawrot->z = mdlRawrot->z;
				rseqRawrot->wneg = mdlRawrot->wneg;
				write_offset += 8;

			}
			//animvalue_ptr
			if (mdlAnimRle->flags & RTECH_ANIM_ANIMROT) {
				flags ^= RTECH_ANIM_ANIMROT; 
				flags |= RTECH_ANIM_RAWROT;
				pMdlRotV = PTR_FROM_IDX(studioanimvalue_ptr_t, mdlAnimRle, read_offset);
				read_offset += sizeof(studioanimvalue_ptr_t);

				pRseqRotV = PTR_FROM_IDX(r5::studioanimvalue_ptr_t, g_model.pData, write_offset);
				printf("   rotv: %d %d %d\n", pMdlRotV->x, pMdlRotV->y, pMdlRotV->z);
				write_offset += sizeof(r5::studioanimvalue_ptr_t);
			}
			if (mdlAnimRle->flags & RTECH_ANIM_ANIMPOS) {
				flags ^= RTECH_ANIM_ANIMPOS;
				flags |= RTECH_ANIM_RAWPOS;
				pMdlPosV = PTR_FROM_IDX(studioanimvalue_ptr_t, mdlAnimRle, read_offset);
				read_offset += sizeof(studioanimvalue_ptr_t);

				pRseqPosV = PTR_FROM_IDX(r5::studioanimvalue_ptr_t, g_model.pData, write_offset);
				printf("   posv: %d %d %d\n", pMdlPosV->x, pMdlPosV->y, pMdlPosV->z);
				write_offset += sizeof(r5::studioanimvalue_ptr_t);
			}

			//write animvalue
			std::vector<int> idx_offset;
			if (pMdlRotV != nullptr && !pMdlRotV->IsAllZero() && mdlAnimRle->flags & RTECH_ANIM_ANIMROT && pRseqRotV != nullptr && pMdlRotV != nullptr) {
				rseqAnimRle->flags = 0x2;
				if (pMdlRotV->x) {
					pRseqRotV->flags |= 4;
					ProcessAnimValue(read_offset, write_offset, mdlAnimRle, pRseqRotV, pStudioAnimDesc[seq_idx].numframes, idx_offset);
				}
				else idx_offset.push_back(0);
				if (pMdlRotV->y) {
					pRseqRotV->flags |= 2;
					ProcessAnimValue(read_offset, write_offset, mdlAnimRle, pRseqRotV, pStudioAnimDesc[seq_idx].numframes, idx_offset);
				}
				else idx_offset.push_back(0);
				if (pMdlRotV->z) {
					pRseqRotV->flags |= 1;
					ProcessAnimValue(read_offset, write_offset, mdlAnimRle, pRseqRotV, pStudioAnimDesc[seq_idx].numframes, idx_offset);
				}
				else idx_offset.push_back(0);
				idx_offset.push_back(0);
				idx_offset.push_back(0);

				while (!idx_offset.empty() && idx_offset.front() == 0) idx_offset.erase(idx_offset.begin()); //remove 0s in the front

				pRseqRotV->offset = idx_offset.at(0) * 2;
				pRseqRotV->idx1 = max(idx_offset.at(1) - idx_offset.at(0), 0);
				pRseqRotV->idx2 = max(idx_offset.at(2) - idx_offset.at(0), 0);
				
			}

			idx_offset.clear();
			if (pMdlPosV != nullptr && !pMdlPosV->IsAllZero() && mdlAnimRle->flags & RTECH_ANIM_ANIMPOS && pRseqPosV != nullptr && pMdlPosV != nullptr) {
				rseqAnimRle->flags |= 0x4;
				if (pMdlPosV->x) {
					pRseqPosV->flags |= 4;
					ProcessAnimValue(read_offset, write_offset, mdlAnimRle, pRseqPosV, pStudioAnimDesc[seq_idx].numframes, idx_offset);
				}
				else idx_offset.push_back(0);
				if (pMdlPosV->y) {
					pRseqPosV->flags |= 2;
					ProcessAnimValue(read_offset, write_offset, mdlAnimRle, pRseqPosV, pStudioAnimDesc[seq_idx].numframes, idx_offset);
				}
				else idx_offset.push_back(0);
				if (pMdlPosV->z) {
					pRseqPosV->flags |= 1;
					ProcessAnimValue(read_offset, write_offset, mdlAnimRle, pRseqPosV, pStudioAnimDesc[seq_idx].numframes, idx_offset);
				}
				else idx_offset.push_back(0);
				idx_offset.push_back(0);
				idx_offset.push_back(0);

				while (!idx_offset.empty() && idx_offset.front() == 0) {
					idx_offset.erase(idx_offset.begin());
				}

				pRseqPosV->offset = idx_offset.at(0) * 2;
				pRseqPosV->idx1 = max(idx_offset.at(1) - idx_offset.at(0), 0);
				pRseqPosV->idx2 = max(idx_offset.at(2) - idx_offset.at(0), 0);

			}

			flaggedbones.push_back(flags);
			rseqAnimRle->size = write_offset;
			g_model.pData += write_offset;
			anim_block_offset += mdlAnimRle->nextoffset;

			if (mdlAnimRle->nextoffset == 0) break;
		}
		flaggedbones.push_back(0);


		//boneflagarray (write)
		for (int i = 0; i < (flaggedbones.size()) / 2; i++) {
			boneflagarray[i] = flaggedbones.at(i * 2);
			boneflagarray[i] |= flaggedbones.at(i * 2 + 1) << 4;
			//printf("%d %d\n%d %d\n", i * 2, flaggedbones.at(i * 2), i * 2 + 1, flaggedbones.at(i * 2 +1));
		}

		//ikrules
		rAnimDesc->numikrules = 1;
		rAnimDesc->ikruleindex = g_model.pData - g_model.pBase - *blend;
		r5::v8::mstudioikrule_t* ikrule = reinterpret_cast<r5::v8::mstudioikrule_t*>(g_model.pData);
		ikrule->type = 4;
		ikrule->compressedikerrorindex = 136;
		g_model.pData += sizeof(r5::v8::mstudioikrule_t);

		//unk movement??
		pV7RseqDesc->unkCount = 1; //14
		pV7RseqDesc->unkOffset = g_model.pData - g_model.pBase;
		r5::unkseqdata_t* movementdata = reinterpret_cast<r5::unkseqdata_t*>(g_model.pData);
		movementdata->unk = 108;
		movementdata->unkfloat3 = 344.9211f;
		movementdata->unkfloat4 = 344.9211f;
		g_model.pData += sizeof(r5::unkseqdata_t) * pV7RseqDesc->unkCount;


		//write all data
		g_model.pData = WriteStringTable(g_model.pData);
		ALIGN4(g_model.pData);

		outRseq.write(g_model.pBase, g_model.pData - g_model.pBase);
		g_model.stringTable.clear();
		delete[] g_model.pBase;
		printf("Done!\n");
	}
}

//unsigned short* animvalue = PTR_FROM_IDX(unsigned short, g_model.pData, write_offset);
//idx_offset.push_back((animvalue - (unsigned short*)pRseqRotV));

//animvalue[0] = 0x0 | (pStudioAnimDesc->numframes << 8); //animvalue metadata
//write_offset += sizeof(short);

//int total_frame = 0;
//std::vector<int> read_anim_value;
//do {
//	unsigned char* metadata = PTR_FROM_IDX(unsigned char, mdlAnimRle, read_offset);
//	for (int j = 0; j < metadata[0]; j++) read_anim_value.push_back(metadata[j + 1]);
//	for (int j = 0; j < metadata[1] - metadata[0]; j++) read_anim_value.push_back(0);

//	read_offset += metadata[0] * 2 + 2;
//	total_frame += metadata[1];
//} while (total_frame < pStudioAnimDesc->numframes);

//printf("  %d\n", read_anim_value.size());

//for (int j = 0; j < pStudioAnimDesc->numframes; j++) {
//	animvalue[j + 1] = read_anim_value.at(j);
//	write_offset += sizeof(short);
//}