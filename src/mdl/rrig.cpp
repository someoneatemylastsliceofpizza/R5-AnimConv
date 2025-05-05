#include <mdl/rrig.h>
#include <mdl/studio.h>
#include <utils/stringtable.cpp>

void WritePreHeader(r5::v8::studiohdr_t* pV54RrigHdr, studiohdr_t* v49MdlHdr);

void ConvertMDL_RRIG(char* mdl_buffer, std::string output_dir, std::string filename) {
	printf("Convering Rrig...\n");
	g_model.pBase = new char[32 * 1024 * 1024] {};
	g_model.pData = g_model.pBase;

	// Header
	studiohdr_t* pV49MdlHdr = reinterpret_cast<studiohdr_t*>(mdl_buffer);
	r5::v8::studiohdr_t* pV54RrigHdr = reinterpret_cast<r5::v8::studiohdr_t*>(g_model.pData);

	WritePreHeader(pV54RrigHdr, pV49MdlHdr);

	g_model.pHdr = pV54RrigHdr;
	g_model.pData += sizeof(r5::v8::studiohdr_t);
	BeginStringTable();

	std::string model_name = pV49MdlHdr->name;
	model_name = "animrig/" + model_name.substr(model_name.find('/') + 1, model_name.rfind('.') - model_name.find('/') - 1) + ".rrig";
	std::ofstream outRrig(output_dir + "\\" + filename + ".rrig", std::ios::out | std::ios::binary);

	memcpy_s(&pV54RrigHdr->name, 64, model_name.c_str(), min(model_name.length(), 64));
	AddToStringTable((char*)pV54RrigHdr, &pV54RrigHdr->sznameindex, model_name.c_str());
	AddToStringTable((char*)pV54RrigHdr, &pV54RrigHdr->surfacepropindex, STRING_FROM_IDX(pV49MdlHdr, pV49MdlHdr->surfacepropindex));

	// Bones
	pV54RrigHdr->boneindex = g_model.pData - g_model.pBase;
	unsigned char iklinksbone[3] = {0,0,0};
	for (int i = 0; i < pV54RrigHdr->numbones; i++) {
		mstudiobone_t* v49bone = reinterpret_cast<mstudiobone_t*>((mdl_buffer + pV49MdlHdr->boneindex + sizeof(mstudiobone_t) * i));
		r5::v8::mstudiobone_t* v54bone = reinterpret_cast<r5::v8::mstudiobone_t*>(g_model.pData);
		const char* bone_name = STRING_FROM_IDX(v49bone, v49bone->sznameindex);
		AddToStringTable((char*)v54bone, &v54bone->sznameindex, bone_name);
		v54bone->parent = v49bone->parent;
		v54bone->bonecontroller[0] = v49bone->bonecontroller[0];
		v54bone->bonecontroller[1] = v49bone->bonecontroller[1];
		v54bone->bonecontroller[2] = v49bone->bonecontroller[2];
		v54bone->bonecontroller[3] = v49bone->bonecontroller[3];
		v54bone->bonecontroller[4] = v49bone->bonecontroller[4];
		v54bone->bonecontroller[5] = v49bone->bonecontroller[5];
		v54bone->pos = v49bone->pos;
		v54bone->quat = v49bone->quat;
		v54bone->rot = v49bone->rot;
		v54bone->scale = Vector3{1,1,1};
		v54bone->poseToBone = v49bone->poseToBone;
		v54bone->qAlignment = v49bone->qAlignment;
		int bone_flag = 0;
		if (v49bone->flags & 0x200) bone_flag |= 0x200;
		if (v49bone->flags & 0x20) bone_flag |= 0x20;
		v54bone->flags = bone_flag;
		v54bone->proctype = 0;
		v54bone->procindex = 0;
		v54bone->physicsbone = 0;
		AddToStringTable((char*)v54bone, &v54bone->surfacepropidx, STRING_FROM_IDX(v49bone, v49bone->surfacepropidx));
		v54bone->contents = v49bone->contents;
		v54bone->surfacepropLookup = v49bone->surfacepropLookup;

		if (strcmp(bone_name, "def_l_shoulder") == 0) iklinksbone[0] = i;
		if (strcmp(bone_name, "def_l_elbow") == 0) iklinksbone[1] = i;
		if (strcmp(bone_name, "def_l_wrist") == 0) iklinksbone[2] = i;
		g_model.pHdr = v54bone;
		g_model.pData += sizeof(r5::v8::mstudiobone_t);
	}

	//hboxset
	pV54RrigHdr->numhitboxsets = 1;
	pV54RrigHdr->hitboxsetindex = g_model.pData - g_model.pBase;
	r5::v8::mstudiohitboxset_t* v54hboxset = reinterpret_cast<r5::v8::mstudiohitboxset_t*>(g_model.pData);
	AddToStringTable((char*)v54hboxset, &v54hboxset->sznameindex, "default");
	v54hboxset->numhitboxes = 0;
	v54hboxset->hitboxindex = 12;
	g_model.pData += sizeof(r5::v8::mstudiohitboxset_t);

	//bone by name
	pV54RrigHdr->bonetablebynameindex = g_model.pData - g_model.pBase;
	char* v54bonebyname = reinterpret_cast<char*>(g_model.pData);
	memcpy_s(v54bonebyname, 256, mdl_buffer + pV49MdlHdr->bonetablebynameindex, pV54RrigHdr->numbones);
	g_model.pData += sizeof(char) * pV54RrigHdr->numbones;
	ALIGN4(g_model.pData);

	//node name
	pV54RrigHdr->localnodenameindex = g_model.pData - g_model.pBase;
	int* v49nodesname = reinterpret_cast<int*>((mdl_buffer + pV49MdlHdr->localnodenameindex));
	for (int i = 0; i < pV54RrigHdr->numlocalnodes; i++) {
		int* v54nodesname = reinterpret_cast<int*>(g_model.pData);
		AddToStringTable((char*)pV54RrigHdr, &v54nodesname[i], STRING_FROM_IDX(reinterpret_cast<char*>(mdl_buffer), v49nodesname[i]));
	}
	g_model.pData += sizeof(int) * pV54RrigHdr->numlocalnodes;

	//node data index (allocate)
	pV54RrigHdr->nodeDataOffsetsOffset = g_model.pData - g_model.pBase;
	int* v54nodedataindex = reinterpret_cast<int*>(g_model.pData);
	g_model.pData += sizeof(int) * pV54RrigHdr->numlocalnodes;

	//node data
	char* v49nodedata = reinterpret_cast<char*>((mdl_buffer + pV49MdlHdr->localnodeindex));
	short* v54nodedata = reinterpret_cast<short*>(g_model.pData);
	int offset = 0;
	for (int i = 0; i < pV54RrigHdr->numlocalnodes; i++) {
		v54nodedataindex[i] = g_model.pData - g_model.pBase;

		int nodecount = 0;
		std::vector<char> v49nodedata_buffer;

		for (int j = 0; j < pV54RrigHdr->numlocalnodes; j++) {
			char* node = &v49nodedata[pV54RrigHdr->numlocalnodes * i + j];
			if (*node > 0) {
				v49nodedata_buffer.push_back(*node);
				nodecount++;
			}
		}

		v54nodedata[offset] = nodecount;
		offset += 2;
		g_model.pData += sizeof(int);
		for (int j = 0; j < nodecount; j++) {
			v54nodedata[offset] = v49nodedata_buffer.at(j);
			v54nodedata[offset + 1] = 0xFF;
			offset += 2;
			g_model.pData += sizeof(int);
		};
	}

	//pose param
	pV54RrigHdr->localposeparamindex = g_model.pData - g_model.pBase;
	pV54RrigHdr->numlocalposeparameters = 3;
	r5::v8::mstudioposeparamdesc_t* v54poseparam = reinterpret_cast<r5::v8::mstudioposeparamdesc_t*>(g_model.pData);
	AddToStringTable((char*)&v54poseparam[0], &v54poseparam[0].sznameindex, "velocity");
	v54poseparam[0].end = 250.0f;
	AddToStringTable((char*)&v54poseparam[1], &v54poseparam[1].sznameindex, "crouchFraction");
	v54poseparam[1].end = 1.0f;
	AddToStringTable((char*)&v54poseparam[2], &v54poseparam[2].sznameindex, "sprintfrac");
	v54poseparam[2].end = 1.0f;
	g_model.pData += sizeof(r5::v8::mstudioposeparamdesc_t) * pV54RrigHdr->numlocalposeparameters;

	//ik chains
	pV54RrigHdr->numikchains = 1;
	pV54RrigHdr->ikchainindex = g_model.pData - g_model.pBase;
	r5::v8::mstudioikchain_t* v54ikchain = reinterpret_cast<r5::v8::mstudioikchain_t*>(g_model.pData);
	AddToStringTable((char*)v54ikchain, &v54ikchain->sznameindex, "hand_left");
	v54ikchain->linktype = 0;
	v54ikchain->linkindex = 20;
	v54ikchain->numlinks = 3;
	v54ikchain->unk = 0x3F5DB3D7;
	g_model.pData += sizeof(r5::v8::mstudioikchain_t);

	//ik links
	r5::v8::mstudioiklink_t* v54iklink = reinterpret_cast<r5::v8::mstudioiklink_t*>(g_model.pData);
	v54iklink[0].bone = iklinksbone[0];
	v54iklink[1].bone = iklinksbone[1];
	v54iklink[2].bone = iklinksbone[2];
	g_model.pData += sizeof(r5::v8::mstudioiklink_t) * v54ikchain->numlinks;

	//write all data
	g_model.pData = WriteStringTable(g_model.pData);
	ALIGN4(g_model.pData);

	pV54RrigHdr->length = g_model.pData - g_model.pBase;
	outRrig.write(g_model.pBase, g_model.pData - g_model.pBase);
	g_model.stringTable.clear();
	delete[] g_model.pBase;
	printf("Done!\n");
}


void WritePreHeader(r5::v8::studiohdr_t* pV54RrigHdr, studiohdr_t* v49MdlHdr) {
	pV54RrigHdr->id = 'TSDI';
	pV54RrigHdr->version = 54;
	pV54RrigHdr->checksum = v49MdlHdr->checksum;
	pV54RrigHdr->checksum = 0x0;
	memcpy_s(pV54RrigHdr->name, 64, v49MdlHdr->name, 64);
	pV54RrigHdr->length = 0xDEADDEAD;
	pV54RrigHdr->eyeposition = v49MdlHdr->eyeposition;
	pV54RrigHdr->illumposition = v49MdlHdr->illumposition;
	pV54RrigHdr->flags = 0;
	pV54RrigHdr->numbones = v49MdlHdr->numbones;
	pV54RrigHdr->boneindex = 0xDEADDEAD;
	pV54RrigHdr->numhitboxsets = 0xDEADDEAD;
	pV54RrigHdr->hitboxsetindex = 0xDEADDEAD;
	pV54RrigHdr->numlocalanim = 0;
	pV54RrigHdr->numlocalseq = 0;
	pV54RrigHdr->activitylistversion = 0;
	pV54RrigHdr->numtextures = 0;
	pV54RrigHdr->numcdtextures = 0;
	pV54RrigHdr->numskinref = 0;
	pV54RrigHdr->numskinfamilies = 0;
	pV54RrigHdr->numbodyparts = 0;
	pV54RrigHdr->numlocalattachments = 0;
	pV54RrigHdr->numlocalnodes = v49MdlHdr->numlocalnodes;
	pV54RrigHdr->localnodeindex = 0;
	pV54RrigHdr->localnodenameindex = 0xDEADDEAD;
	pV54RrigHdr->numikchains = 0xDEADDEAD;
	pV54RrigHdr->ikchainindex = 0xDEADDEAD;
	pV54RrigHdr->nodeDataOffsetsOffset = 0xDEADDEAD;
	pV54RrigHdr->numlocalposeparameters = 3;
	pV54RrigHdr->localposeparamindex = 0xDEADDEAD;
	pV54RrigHdr->keyvaluesize = 0;
	pV54RrigHdr->numsrcbonetransform = 0;
	pV54RrigHdr->mass = 1.0f;
	pV54RrigHdr->contents = v49MdlHdr->contents;
	pV54RrigHdr->numincludemodels = 0;
	pV54RrigHdr->bonetablebynameindex = 0xDEADDEAD;
	pV54RrigHdr->constdirectionallightdot = v49MdlHdr->constdirectionallightdot;
	pV54RrigHdr->rootLOD = v49MdlHdr->rootLOD;
	pV54RrigHdr->numAllowedRootLODs = v49MdlHdr->numAllowedRootLODs;
	pV54RrigHdr->defaultFadeDist = -1;
	pV54RrigHdr->flVertAnimFixedPointScale = v49MdlHdr->flVertAnimFixedPointScale;
}
