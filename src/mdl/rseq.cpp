#include <mdl/rrig.h>
#include <mdl/studio.h>
#include <utils/stringtable.cpp>

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
		pV7RseqDesc->posekeyindex = g_model.pData - g_model.pBase;

		g_model.pData += sizeof(int) * (numposekey);

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

		//animdesc
		r5::v8::mstudioanimdesc_t* rAnimDesc = reinterpret_cast<r5::v8::mstudioanimdesc_t*>(g_model.pData);
		*blend = g_model.pData - g_model.pBase;
		rAnimDesc->fps = pStudioAnimDesc[seq_idx].fps;
		rAnimDesc->flags = 0x20000;
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
		int animbase_ptr = pStudioAnimDesc[seq_idx].baseptr;
		int offset = 0;
		for (int i = 0; i < pV49MdlHdr->numbones; i++) {
            mstudio_rle_anim_t* mdlAnimData = PTR_FROM_IDX(mstudio_rle_anim_t, mdl_buffer - animbase_ptr, pStudioAnimDesc[seq_idx].animindex + offset);
			r5::mstudio_rle_anim_t* rseqAnimData = reinterpret_cast<r5::mstudio_rle_anim_t*>(g_model.pData);
			
			int flags = mdlAnimData->flags;
			if (flags & STUDIO_ANIM_RAWROT) {
				flags ^= STUDIO_ANIM_RAWROT;
				flags |= RTECH_ANIM_RAWROT;
			}
			flaggedbones.push_back(flags);
			printf("%d f: %x\n", i, flags);
			offset += mdlAnimData->nextoffset;
		}
		flaggedbones.push_back(0);


		//boneflagarray (write)
		for (int i = 0; i < (pV49MdlHdr->numbones + 1) / 2; i++) {
			boneflagarray[i] = flaggedbones.at(i);
			printf("%d\n", i);
			boneflagarray[i] |= flaggedbones.at(i+1) << 4;
		}


		//unk movement??
		pV7RseqDesc->unkCount = 0; //14
		pV7RseqDesc->unkOffset = g_model.pData - g_model.pBase;
		g_model.pData += 18 * pV7RseqDesc->unkCount;


		//write all data
		g_model.pData = WriteStringTable(g_model.pData);
		ALIGN4(g_model.pData);

		outRseq.write(g_model.pBase, g_model.pData - g_model.pBase);
		g_model.stringTable.clear();
		delete[] g_model.pBase;
		printf("Done!\n");
	}
}
