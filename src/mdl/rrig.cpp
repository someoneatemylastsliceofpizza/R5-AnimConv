#include <mdl/rrig.h>
#include <mdl/studio.h>
#include <utils/stringtable.cpp>
#include <utils/print.h>

void SetFlagForDescendants(r5r::mstudiobone_t* bones, int numbones, int parentIdx, int flag);

std::string ConvertMDL_RRIG(char* mdl_buffer, std::string output_dir, std::string filename, std::vector<std::pair<std::pair<int, int>, int>>&nodedata) {
	print("\nConverting rrig...\n");
	g_model.pBase = new char[32 * 1024 * 1024] {};
	g_model.pData = g_model.pBase;

	// Header
	pt2::studiohdr_t* pV49MdlHdr = reinterpret_cast<pt2::studiohdr_t*>(mdl_buffer);
	r5r::studiohdr_t* pV54RrigHdr = reinterpret_cast<r5r::studiohdr_t*>(g_model.pData);

	pV54RrigHdr->id = 'TSDI';
	pV54RrigHdr->version = 54;
	pV54RrigHdr->checksum = 0xFFFFFFFF;
	memcpy_s(pV54RrigHdr->name, 64, pV49MdlHdr->name, 64);
	pV54RrigHdr->eyeposition = pV49MdlHdr->eyeposition;
	pV54RrigHdr->illumposition = pV49MdlHdr->illumposition;
	pV54RrigHdr->numbones = pV49MdlHdr->numbones;
	pV54RrigHdr->numlocalnodes = pV49MdlHdr->numlocalnodes;
	pV54RrigHdr->mass = 1.0f;
	pV54RrigHdr->contents = pV49MdlHdr->contents;
	pV54RrigHdr->constdirectionallightdot = pV49MdlHdr->constdirectionallightdot;
	pV54RrigHdr->rootLOD = pV49MdlHdr->rootLOD;
	pV54RrigHdr->numAllowedRootLODs = pV49MdlHdr->numAllowedRootLODs;
	pV54RrigHdr->defaultFadeDist = -1;
	pV54RrigHdr->flVertAnimFixedPointScale = pV49MdlHdr->flVertAnimFixedPointScale;

	g_model.pHdr = pV54RrigHdr;
	g_model.pData += sizeof(r5r::studiohdr_t);
	BeginStringTable();

	std::string model_name = pV49MdlHdr->name;
	std::string temp_model_dir = model_name;
	std::replace(temp_model_dir.begin(), temp_model_dir.end(), '/', '\\');
	model_name = "animrig/" + temp_model_dir.substr(temp_model_dir.find('\\') + 1, temp_model_dir.rfind('.') - temp_model_dir.find('\\') - 1) + ".rrig";
    
	std::string model_dir = model_name.substr(0, model_name.find_last_of("\\") + 1);
    std::replace(model_dir.begin(), model_dir.end(), '/', '\\');

    std::filesystem::create_directories(output_dir + "\\" + model_dir);
	std::ofstream outRrig(output_dir + "\\" + model_dir + "\\" + filename + ".rrig", std::ios::out | std::ios::binary);

	memcpy_s(&pV54RrigHdr->name, 64, model_name.c_str(), MIN(model_name.length(), 64));
	AddToStringTable((char*)pV54RrigHdr, &pV54RrigHdr->sznameindex, model_name.c_str());
	AddToStringTable((char*)pV54RrigHdr, &pV54RrigHdr->surfacepropindex, STRING_FROM_IDX(pV49MdlHdr, pV49MdlHdr->surfacepropindex));

	// Bones
	pV54RrigHdr->boneindex = g_model.pData - g_model.pBase;
	r5r::mstudiobone_t* v54bone = reinterpret_cast<r5r::mstudiobone_t*>(g_model.pData);
	pt2::mstudiobone_t* v49bone = reinterpret_cast<pt2::mstudiobone_t*>(mdl_buffer + pV49MdlHdr->boneindex);
	for (int i = 0; i < pV54RrigHdr->numbones; i++) {
		const char* bone_name = STRING_FROM_IDX(&v49bone[i], v49bone[i].sznameindex);
		AddToStringTable((char*)&v54bone[i], &v54bone[i].sznameindex, bone_name);
		v54bone[i].parent = v49bone[i].parent;
		v54bone[i].bonecontroller[0] = v49bone[i].bonecontroller[0];
		v54bone[i].bonecontroller[1] = v49bone[i].bonecontroller[1];
		v54bone[i].bonecontroller[2] = v49bone[i].bonecontroller[2];
		v54bone[i].bonecontroller[3] = v49bone[i].bonecontroller[3];
		v54bone[i].bonecontroller[4] = v49bone[i].bonecontroller[4];
		v54bone[i].bonecontroller[5] = v49bone[i].bonecontroller[5];
		v54bone[i].pos = v49bone[i].pos;
		v54bone[i].quat = v49bone[i].quat;
		v54bone[i].rot = v49bone[i].rot;
		v54bone[i].scale = Vector3{ 1,1,1 };
		v54bone[i].poseToBone = v49bone[i].poseToBone;
		v54bone[i].qAlignment = v49bone[i].qAlignment;
		v54bone[i].flags = v49bone[i].flags & 0x200;
		v54bone[i].proctype = 0;
		v54bone[i].procindex = 0;
		v54bone[i].physicsbone = 0;
		AddToStringTable((char*)&v54bone[i], &v54bone[i].surfacepropidx, STRING_FROM_IDX(&v49bone[i], v49bone[i].surfacepropidx));
		v54bone[i].contents = v49bone[i].contents;
		v54bone[i].surfacepropLookup = v49bone[i].surfacepropLookup;
	}
	g_model.pHdr = v54bone;
	g_model.pData += pV54RrigHdr->numbones * sizeof(r5r::mstudiobone_t);

	//TODO:
	//hboxset
	pV54RrigHdr->numhitboxsets = 1;
	pV54RrigHdr->hitboxsetindex = g_model.pData - g_model.pBase;
	r5r::mstudiohitboxset_t* v54hboxset = reinterpret_cast<r5r::mstudiohitboxset_t*>(g_model.pData);
	AddToStringTable((char*)v54hboxset, &v54hboxset->sznameindex, "default");
	v54hboxset->numhitboxes = 0;
	v54hboxset->hitboxindex = 12;
	g_model.pData += sizeof(r5r::mstudiohitboxset_t);

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
	int c_node = 0;
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
            for (const auto& node : nodedata) {
				if (node.first == std::pair{ i + 1, v49nodedata_buffer.at(j) }) {
					v54nodedata[offset + 1] = node.second;
					break;
				}
            }
			offset += 2;
			g_model.pData += sizeof(int);
		};
	}

	//pose param
	pV54RrigHdr->localposeparamindex = g_model.pData - g_model.pBase;
	pV54RrigHdr->numlocalposeparameters = pV49MdlHdr->numlocalposeparameters;
	pt2::mstudioposeparamdesc_t* v49poseparam = PTR_FROM_IDX(pt2::mstudioposeparamdesc_t, mdl_buffer, pV49MdlHdr->localposeparamindex);
	r5r::mstudioposeparamdesc_t* v54poseparam = reinterpret_cast<r5r::mstudioposeparamdesc_t*>(g_model.pData);
	for (int i = 0; i < pV54RrigHdr->numlocalposeparameters; i++) {
		AddToStringTable((char*)&v54poseparam[i], &v54poseparam[i].sznameindex, STRING_FROM_IDX(&v49poseparam[i], v49poseparam[i].sznameindex));
		v54poseparam[i].start = v49poseparam[i].start;
		v54poseparam[i].end = v49poseparam[i].end;
	}
	g_model.pData += sizeof(r5r::mstudioposeparamdesc_t) * pV54RrigHdr->numlocalposeparameters;

	//ik chains
	pV54RrigHdr->numikchains = pV49MdlHdr->numikchains;
	pV54RrigHdr->ikchainindex = g_model.pData - g_model.pBase;
	pt2::mstudioikchain_t* v49ikchain = PTR_FROM_IDX(pt2::mstudioikchain_t, mdl_buffer, pV49MdlHdr->ikchainindex);
	r5r::mstudioikchain_t* v54ikchain = reinterpret_cast<r5r::mstudioikchain_t*>(g_model.pData);
	for (int i = 0; i < pV49MdlHdr->numikchains; i++) {
		AddToStringTable((char*)&v54ikchain[i], &v54ikchain[i].sznameindex, STRING_FROM_IDX(&v49ikchain[i], v49ikchain[i].sznameindex));
		v54ikchain[i].linktype = v49ikchain[i].linktype;
		v54ikchain[i].numlinks = v49ikchain[i].numlinks;
		v54ikchain[i].unk = 0.707f;
	}
	g_model.pData += sizeof(r5r::mstudioikchain_t) * pV54RrigHdr->numikchains;

	//ik links
	for (int i = 0; i < pV49MdlHdr->numikchains; i++) {
		v54ikchain[i].linkindex = g_model.pData - PTR_FROM_IDX(char, &v54ikchain[i], 0);
		pt2::mstudioiklink_t* v49iklink = PTR_FROM_IDX(pt2::mstudioiklink_t, &v49ikchain[i], v49ikchain[i].linkindex);
		r5r::mstudioiklink_t* v54iklink = reinterpret_cast<r5r::mstudioiklink_t*>(g_model.pData);
		v54iklink[0].bone = v49iklink[0].bone;
		v54iklink[1].bone = v49iklink[1].bone;
		v54iklink[2].bone = v49iklink[2].bone;

		v54bone[v54iklink[0].bone].flags |= 0x20;
		v54bone[v54iklink[1].bone].flags |= 0x20;
		v54bone[v54iklink[2].bone].flags |= 0x20;
		SetFlagForDescendants(v54bone, pV54RrigHdr->numbones, v54iklink[0].bone, 0x20);
		g_model.pData += sizeof(r5r::mstudioiklink_t) * v54ikchain->numlinks;
	}

	//write all data
	g_model.pData = WriteStringTable(g_model.pData);
	ALIGN4(g_model.pData);
	std::string ret_name = model_dir + filename + ".rrig";
	std::replace(ret_name.begin(), ret_name.end(), '\\', '/');
	print("    ->%s  ...", ret_name.c_str());

	pV54RrigHdr->length = g_model.pData - g_model.pBase;
	outRrig.write(g_model.pBase, g_model.pData - g_model.pBase);
	g_model.stringTable.clear();
	delete[] g_model.pBase;
	print("Done!\n\n");

	return ret_name;
}

void SetFlagForDescendants(r5r::mstudiobone_t* bones, int numbones, int parentIdx, int flag) {
	for (int i = 0; i < numbones; ++i) {
		if (bones[i].parent == parentIdx) {
			bones[i].flags |= flag;
			SetFlagForDescendants(bones, numbones, i, flag);
		}
	}
}