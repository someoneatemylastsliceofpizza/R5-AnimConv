#include "src/pch.h"
#include <rrig/rrig.h>

void WriteRRIG_v8(std::string output_dir, const temp::rig_t& rig) {
	std::vector<char> buffer(16 * 1024 * 1024, 0);
	char* pBase = buffer.data();
	char* pData = pBase;
	temp::StringTable stringTables{};
	stringTables.Init();

	// Header
	r5::v8::studiohdr_t* pRigHdr = reinterpret_cast<r5::v8::studiohdr_t*>(pData);

	pRigHdr->id = 'TSDI';
	pRigHdr->version = 54;
	pRigHdr->checksum = 0x67676767; 
	strncpy_s(pRigHdr->name, 64, rig.name.c_str() + std::max<int>(0, rig.name.size() - 63), _TRUNCATE);
	pRigHdr->eyeposition = rig.hdr.eyeposition;
	pRigHdr->flags = rig.hdr.flags;
	pRigHdr->numbones = rig.hdr.numbones;
	pRigHdr->activitylistversion = rig.hdr.activitylistversion;
	pRigHdr->defaultFadeDist = rig.hdr.defaultFadeDist;
	pRigHdr->mass = rig.hdr.mass;
	pRigHdr->contents = rig.hdr.contents;
	pRigHdr->mins = rig.hdr.mins;
	pRigHdr->maxs = rig.hdr.maxs;
	pData += sizeof(r5::v8::studiohdr_t);

	std::filesystem::path temp_model_dir = rig.name;
	std::filesystem::create_directories(output_dir + "\\" + temp_model_dir.parent_path().string());
	std::ofstream outRrig(output_dir + "\\" + rig.name, std::ios::out | std::ios::binary);

	stringTables.Add((char*)pRigHdr, &pRigHdr->sznameindex, rig.name);
	stringTables.Add((char*)pRigHdr, &pRigHdr->surfacepropindex, rig.hdr.surfaceprop);

	// Bones
	pRigHdr->boneindex = pData - pBase;
	r5::v8::mstudiobone_t* pBones = reinterpret_cast<r5::v8::mstudiobone_t*>(pData);
	for (int i = 0; i < pRigHdr->numbones; i++) {
		auto& bone = rig.bones[i];
		stringTables.Add((char*)&pBones[i], &pBones[i].sznameindex, bone.name);
		pBones[i].parent = bone.parent;
		pBones[i].bonecontroller[0] = bone.bonecontroller[0];
		pBones[i].bonecontroller[1] = bone.bonecontroller[1];
		pBones[i].bonecontroller[2] = bone.bonecontroller[2];
		pBones[i].bonecontroller[3] = bone.bonecontroller[3];
		pBones[i].bonecontroller[4] = bone.bonecontroller[4];
		pBones[i].bonecontroller[5] = bone.bonecontroller[5];
		pBones[i].pos = bone.pos;
		pBones[i].quat = bone.q;
		pBones[i].rot = bone.rot;
		pBones[i].scale = bone.scl;
		pBones[i].poseToBone = bone.poseToBone;
		pBones[i].qAlignment = bone.qAlignment;
		pBones[i].flags = bone.flags;
		pBones[i].proctype = bone.proctype; //
		pBones[i].procindex = bone.procindex; //
		pBones[i].physicsbone = bone.physicsbone; //
		stringTables.Add((char*)&pBones[i], &pBones[i].surfacepropidx, bone.surfaceprop);
		pBones[i].contents = bone.contents;
		pBones[i].surfacepropLookup = bone.surfacepropLookup;
	}
	pData += pRigHdr->numbones * sizeof(r5::v8::mstudiobone_t);

	// bonecontroller?
	pRigHdr->numbonecontrollers = 0;
	pRigHdr->bonecontrollerindex = pData - pBase;

	//hboxset
	pRigHdr->numhitboxsets = rig.hitboxsets.size();
	pRigHdr->hitboxsetindex = pData - pBase;
	r5::v8::mstudiohitboxset_t* pHitboxsets = reinterpret_cast<r5::v8::mstudiohitboxset_t*>(pData);
	for (int i = 0; i < pRigHdr->numhitboxsets; i++) {
		stringTables.Add((char*)&pHitboxsets[i], &pHitboxsets[i].sznameindex, rig.hitboxsets[i].name);
		pHitboxsets[i].numhitboxes = rig.hitboxsets[i].hitboxes.size();
		pData += sizeof(r5::v8::mstudiohitboxset_t);
	}

	for (int i = 0; i < pRigHdr->numhitboxsets; i++) {
		r5::v8::mstudiobbox_t* pHitboxes = reinterpret_cast<r5::v8::mstudiobbox_t*>(pData);
		pHitboxsets[i].hitboxindex = pData - (char*)&pHitboxsets[i];
		for (int j = 0; j < pHitboxsets[i].numhitboxes; j++) {
			auto& hitbox = rig.hitboxsets[i].hitboxes[j];
			stringTables.Add((char*)&pHitboxes[j], &pHitboxes[j].szhitboxnameindex, hitbox.name);
			stringTables.Add((char*)&pHitboxes[j], &pHitboxes[j].hitdataGroupOffset, hitbox.hitdataGroupOffset);
			pHitboxes[j].bone = hitbox.bone;
			pHitboxes[j].group = hitbox.group;
			pHitboxes[j].bbmin = hitbox.bbmin;
			pHitboxes[j].bbmax = hitbox.bbmax;
			pHitboxes[j].critShotOverride = hitbox.critShotOverride;
			pData += sizeof(r5::v8::mstudiobbox_t);
		}
	}

	//bone by name
	pRigHdr->bonetablebynameindex = pData - pBase;
	char* pBonebynames = reinterpret_cast<char*>(pData);
	memcpy_s(pBonebynames, 256, rig.bonebyname.data(), pRigHdr->numbones);
	pData += sizeof(char) * pRigHdr->numbones;
	ALIGN4(pData);


	pRigHdr->localattachmentindex = pData - pBase;

	//node name
	pRigHdr->numlocalnodes = rig.nodes.size();
	pRigHdr->localnodenameindex = pData - pBase;
	for (int i = 0; i < pRigHdr->numlocalnodes; i++) {
		int* pNodenames = reinterpret_cast<int*>(pData);
		stringTables.Add((char*)pRigHdr, &pNodenames[i], rig.nodes[i].name);
	}
	pData += sizeof(int) * pRigHdr->numlocalnodes;

	//node data index (allocate)
	pRigHdr->nodeDataOffsetsOffset = pData - pBase;
	int* pNodeDataIndex = reinterpret_cast<int*>(pData);
	pData += sizeof(int) * pRigHdr->numlocalnodes;

	//node data
	for (int i = 0; i < pRigHdr->numlocalnodes; i++) {
		pNodeDataIndex[i] = pData - pBase;
		int transitionsCount = rig.nodes[i].nodedatas.size();
		int* pNodes = reinterpret_cast<int*>(pData);
		*pNodes = transitionsCount;
		pData += sizeof(int);

		for (int j = 0; j < transitionsCount; j++) {
			auto& nodedata = rig.nodes[i].nodedatas[j];
			r5::v8::nodedata_t* pNodeData = reinterpret_cast<r5::v8::nodedata_t*>(pData);
			pNodeData[j].tonode = nodedata.tonode;
			pNodeData[j].seq = nodedata.seq;
		}
		pData += sizeof(r5::v8::nodedata_t) * transitionsCount;
	}

	//pose param
	pRigHdr->numlocalposeparameters = rig.poseparams.size();
	pRigHdr->localposeparamindex = pData - pBase;
	r5::v8::mstudioposeparamdesc_t* pPoseparams = reinterpret_cast<r5::v8::mstudioposeparamdesc_t*>(pData);
	for (int i = 0; i < pRigHdr->numlocalposeparameters; i++) {
		stringTables.Add((char*)&pPoseparams[i], &pPoseparams[i].sznameindex, rig.poseparams[i].name);
		pPoseparams[i].start = rig.poseparams[i].start;
		pPoseparams[i].end = rig.poseparams[i].end;
		pPoseparams[i].flags = rig.poseparams[i].flags;
		pPoseparams[i].loop = rig.poseparams[i].loop;
	}
	pData += sizeof(r5::v8::mstudioposeparamdesc_t) * pRigHdr->numlocalposeparameters;

	//ik chains
	pRigHdr->numikchains = rig.ikchains.size();
	pRigHdr->ikchainindex = pData - pBase;
	r5::v8::mstudioikchain_t* pIkchains = reinterpret_cast<r5::v8::mstudioikchain_t*>(pData);
	for (int i = 0; i < pRigHdr->numikchains; i++) {
		stringTables.Add((char*)&pIkchains[i], &pIkchains[i].sznameindex, rig.ikchains[i].name);
		pIkchains[i].linktype = rig.ikchains[i].linktype;
		pIkchains[i].unk = rig.ikchains[i].unk;
		pIkchains[i].numlinks = rig.ikchains[i].iklinks.size();
	}
	pData += sizeof(r5::v8::mstudioikchain_t) * pRigHdr->numikchains;

	//ik links
	for (int i = 0; i < pRigHdr->numikchains; i++) {
		pIkchains[i].linkindex = pData - (char*)&pIkchains[i];
		r5::v8::mstudioiklink_t* pIklinks = reinterpret_cast<r5::v8::mstudioiklink_t*>(pData);
		for (int j = 0; j < rig.ikchains[i].iklinks.size(); j++) {
			auto& iklink = rig.ikchains[i].iklinks[j];
			pIklinks[j].bone = iklink.bone;
			pIklinks[j].kneeDir = iklink.kneeDir;
		}
		pData += sizeof(r5::v8::mstudioiklink_t) * pIkchains->numlinks;
	}

	//write all data
	pData = stringTables.Write(pData);
	ALIGN4(pData);
	pRigHdr->length = pData - pBase;
	outRrig.write(pBase, pData - pBase);
	stringTables.Init();
}

void SetFlagForDescendants(temp::rig_t& rig, int parentIdx, int flag) {
	for (int i = 0; i < rig.hdr.numbones; ++i) {
		if (rig.bones[i].parent == parentIdx) {
			rig.bones[i].flags |= flag;
			SetFlagForDescendants(rig, i, flag);
		}
	}
}