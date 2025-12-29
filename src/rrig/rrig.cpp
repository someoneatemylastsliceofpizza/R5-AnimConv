#include <pch.h>
#include <rrig/rrig.h>

static void ParseRRIG_v121_bones(char* buffer, temp::rig_t& rig, int boneindex, int bonetablebynameindex) {
	rig.bones.resize(rig.hdr.numbones);
	r5::v121::mstudiobone_t* pBones = reinterpret_cast<r5::v121::mstudiobone_t*>(buffer + boneindex);
	for (int i = 0; i < rig.hdr.numbones; i++) {
		rig.bones[i].name = STRING_FROM_IDX(&pBones[i], pBones[i].sznameindex);
		rig.bones[i].parent = pBones[i].parent;
		for (int j = 0; j < 6; j++) rig.bones[i].bonecontroller[j] = pBones[i].bonecontroller[j];
		rig.bones[i].pos = pBones[i].pos;
		rig.bones[i].q = pBones[i].quat;
		rig.bones[i].rot = pBones[i].rot;
		rig.bones[i].scl = pBones[i].scale;
		rig.bones[i].poseToBone = pBones[i].poseToBone;
		rig.bones[i].qAlignment = pBones[i].qAlignment;
		rig.bones[i].flags = pBones[i].flags;
		rig.bones[i].proctype = pBones[i].proctype; //
		rig.bones[i].procindex = pBones[i].procindex; //
		rig.bones[i].physicsbone = pBones[i].physicsbone; //
		rig.bones[i].contents = pBones[i].contents;
		rig.bones[i].surfacepropLookup = pBones[i].surfacepropLookup;
	}

	//bone by name
	auto* bonebyname = reinterpret_cast<uint8_t*>(buffer + bonetablebynameindex);
	rig.bonebyname.resize(rig.hdr.numbones);
	for (int i = 0; i < rig.hdr.numbones; i++) {
		rig.bonebyname[i] = bonebyname[i];
	}
}

static void ParseRRIG_v16_bones(char* buffer, temp::rig_t& rig, uint32_t boneHdrOffset, uint32_t boneDataOffset, uint32_t bonetablebynameindex) {
	rig.bones.resize(rig.hdr.numbones);
	auto* pBones = reinterpret_cast<r5::v16::mstudiobonehdr_t*>(buffer + boneHdrOffset);
	auto* pBoneDatas = reinterpret_cast<r5::v16::mstudiobonedata_t*>(buffer + boneDataOffset);
	for (int i = 0; i < rig.hdr.numbones; i++) {
		rig.bones[i].name = STRING_FROM_IDX(&pBones[i], pBones[i].sznameindex);
		rig.bones[i].parent = pBoneDatas[i].parent;
		rig.bones[i].pos = pBoneDatas[i].pos;
		rig.bones[i].q = pBoneDatas[i].quat;
		rig.bones[i].rot = pBoneDatas[i].rot;
		rig.bones[i].scl = pBoneDatas[i].scale;
		rig.bones[i].poseToBone = pBoneDatas[i].poseToBone;
		rig.bones[i].qAlignment = pBoneDatas[i].qAlignment;
		rig.bones[i].flags = pBoneDatas[i].flags;
		rig.bones[i].proctype = pBoneDatas[i].proctype;
		rig.bones[i].procindex = pBoneDatas[i].procindex;
		rig.bones[i].physicsbone = pBones[i].physicsbone;
		rig.bones[i].contents = pBones[i].contents;
		rig.bones[i].surfacepropLookup = pBones[i].surfacepropLookup;
	}

	//bone by name
	auto* bonebyname = reinterpret_cast<uint8_t*>(buffer + bonetablebynameindex);
	rig.bonebyname.resize(rig.hdr.numbones);
	for (int i = 0; i < rig.hdr.numbones; i++) {
		rig.bonebyname[i] = bonebyname[i];
	}
}

static void ParseRRIG_v8_nodes(char* buffer, temp::rig_t& rig, int numlocalnodes, int localnodenameindex, int nodedataindexindex) {
	//node name
	int* pNodenames = reinterpret_cast<int*>((buffer + localnodenameindex));
	for (int i = 0; i < numlocalnodes; i++) {
		temp::node_t node{};
		node.name = STRING_FROM_IDX(reinterpret_cast<char*>(buffer), pNodenames[i]);
		if (node.name == "NODE_IGNORE") rig.ignorenode = i + 1;
		rig.nodes.push_back(node);
	}

	//node data
	int* pNodeDataIndexes = reinterpret_cast<int*>(buffer + nodedataindexindex);
	for (int i = 0; i < numlocalnodes; i++) {
		int transitionCount = *reinterpret_cast<int*>(buffer + pNodeDataIndexes[i]);

		for (int j = 0; j < transitionCount; j++) {
			r5::v8::nodedata_t* pNodeDataV8 = reinterpret_cast<r5::v8::nodedata_t*>(buffer + pNodeDataIndexes[i] + sizeof(int));
			temp::nodedata_t nodedata{};
			nodedata.tonode = pNodeDataV8[j].tonode;
			nodedata.seq = pNodeDataV8[j].seq;
			rig.nodes[i].nodedatas.push_back(nodedata);
		}
	}
}

static void ParseRRIG_v14_nodes(char* buffer, temp::rig_t& rig, int numlocalnodes, int localnodenameindex, int nodedataindexindex) {
	//node name
	int* pNodenames = reinterpret_cast<int*>((buffer + localnodenameindex));
	for (int i = 0; i < numlocalnodes; i++) {
		temp::node_t node{};
		node.name = STRING_FROM_IDX(pNodenames, pNodenames[i]);
		rig.nodes.push_back(node);
	}

	//node data
	int* pNodeDataIndexes = reinterpret_cast<int*>(buffer + nodedataindexindex);
	for (int i = 0; i < numlocalnodes; i++) {
		int transitionCount = *reinterpret_cast<int*>(buffer + pNodeDataIndexes[i]);

		for (int j = 0; j < transitionCount; j++) {
			r5::v8::nodedata_t* pNodeDataV14 = reinterpret_cast<r5::v8::nodedata_t*>(buffer + pNodeDataIndexes[i] + sizeof(int));
			temp::nodedata_t nodedata{};
			nodedata.tonode = pNodeDataV14[j].tonode;
			nodedata.seq = pNodeDataV14[j].seq;
			rig.nodes[i].nodedatas.push_back(nodedata);
		}
	}
}

static void ParseRRIG_v16_nodes(char* buffer, temp::rig_t& rig, int numlocalnodes, int localnodenameindex, int localNodeDataOffset) {
	//node name
	auto* pNodenames = reinterpret_cast<uint16_t*>((buffer + localnodenameindex));
	for (int i = 0; i < numlocalnodes; i++) {
		temp::node_t node{};
		node.name = STRING_FROM_IDX(pNodenames, pNodenames[i]);
		rig.nodes.push_back(node);
	}

	//node data
	auto* pNodeDataIndexes = reinterpret_cast<uint16_t*>(buffer + localNodeDataOffset);
	for (int i = 0; i < numlocalnodes; i++) {
		uint16_t transitionCount = *reinterpret_cast<uint16_t*>((char*)pNodeDataIndexes + pNodeDataIndexes[i]);

		for (int j = 0; j < transitionCount; j++) {
			auto* pNodeDataV14 = reinterpret_cast<r5::v8::nodedata_t*>((char*)pNodeDataIndexes + pNodeDataIndexes[i] + sizeof(uint16_t));
			temp::nodedata_t nodedata{};
			nodedata.tonode = pNodeDataV14[j].tonode;
			nodedata.seq = pNodeDataV14[j].seq;
			rig.nodes[i].nodedatas.push_back(nodedata);
		}
	}
}

static void ParseRRIG_v8_hitboxes(char* buffer, temp::rig_t& rig, int numhitboxsets, int hitboxsetindex) {
	//hboxset
	r5::v8::mstudiohitboxset_t* pHitboxsets = PTR_FROM_IDX(r5::v8::mstudiohitboxset_t, buffer, hitboxsetindex);
	rig.hitboxsets.resize(numhitboxsets);
	for (int i = 0; i < numhitboxsets; i++) {
		rig.hitboxsets[i].name = STRING_FROM_IDX(&pHitboxsets[i], pHitboxsets[i].sznameindex);
		verbose("");
		for (int j = 0; j < pHitboxsets[i].numhitboxes; j++) {
			r5::v8::mstudiobbox_t* pHitbox = PTR_FROM_IDX(r5::v8::mstudiobbox_t, &pHitboxsets[i], pHitboxsets[i].hitboxindex);
			temp::bbox_t hitbox{};
			hitbox.name = STRING_FROM_IDX(&pHitbox[j], pHitbox[j].szhitboxnameindex);
			hitbox.hitdataGroupOffset = STRING_FROM_IDX(&pHitbox[j], pHitbox[j].hitdataGroupOffset);
			hitbox.bone = pHitbox[j].bone;
			hitbox.group = pHitbox[j].group;
			hitbox.bbmin = pHitbox[j].bbmin;
			hitbox.bbmax = pHitbox[j].bbmax;
			hitbox.critShotOverride = pHitbox[j].critShotOverride;
			rig.hitboxsets[i].hitboxes.push_back(hitbox);
		}
	}
}

static void ParseRRIG_v16_hitboxes(char* buffer, temp::rig_t& rig, int numhitboxsets, int hitboxsetindex) {
	//hboxset
	auto* pHitboxsets = PTR_FROM_IDX(r5::v16::mstudiohitboxset_t, buffer, hitboxsetindex);
	rig.hitboxsets.resize(numhitboxsets);
	for (int i = 0; i < numhitboxsets; i++) {
		rig.hitboxsets[i].name = STRING_FROM_IDX(&pHitboxsets[i], pHitboxsets[i].sznameindex);
		for (int j = 0; j < pHitboxsets[i].numhitboxes; j++) {
			r5::v8::mstudiobbox_t* pHitbox = PTR_FROM_IDX(r5::v8::mstudiobbox_t, &pHitboxsets[i], pHitboxsets[i].hitboxindex);
			temp::bbox_t hitbox{};
			hitbox.name = STRING_FROM_IDX(&pHitbox[j], pHitbox[j].szhitboxnameindex);
			hitbox.hitdataGroupOffset = STRING_FROM_IDX(&pHitbox[j], pHitbox[j].hitdataGroupOffset);
			hitbox.bone = pHitbox[j].bone;
			hitbox.group = pHitbox[j].group;
			hitbox.bbmin = pHitbox[j].bbmin;
			hitbox.bbmax = pHitbox[j].bbmax;
			hitbox.critShotOverride = pHitbox[j].critShotOverride;
			rig.hitboxsets[i].hitboxes.push_back(hitbox);
		}
	}
}

static void ParseRRIG_v8_poseparams(char* buffer, temp::rig_t& rig, int numlocalposeparameters, int localposeparamindex) {
	//pose param
	r5::v8::mstudioposeparamdesc_t* pPoseparams = PTR_FROM_IDX(r5::v8::mstudioposeparamdesc_t, buffer, localposeparamindex);
	for (int i = 0; i < numlocalposeparameters; i++) {
		temp::poseparam_t poseparam{};
		poseparam.name = STRING_FROM_IDX(&pPoseparams[i], pPoseparams[i].sznameindex);
		poseparam.start = pPoseparams[i].start;
		poseparam.end = pPoseparams[i].end;
		poseparam.flags = pPoseparams[i].flags;
		poseparam.loop = pPoseparams[i].loop;
		rig.poseparams.push_back(poseparam);
	}
}

static void ParseRRIG_v16_poseparams(char* buffer, temp::rig_t& rig, int numlocalposeparameters, int localposeparamindex) {
	//pose param
	auto* pPoseparams = PTR_FROM_IDX(r5::v16::mstudioposeparamdesc_t, buffer, localposeparamindex);
	for (int i = 0; i < numlocalposeparameters; i++) {
		temp::poseparam_t poseparam{};
		poseparam.name = STRING_FROM_IDX(&pPoseparams[i], pPoseparams[i].sznameindex);
		poseparam.start = pPoseparams[i].start;
		poseparam.end = pPoseparams[i].end;
		poseparam.flags = pPoseparams[i].flags;
		poseparam.loop = pPoseparams[i].loop;
		rig.poseparams.push_back(poseparam);
	}
}

static void ParseRRIG_v8_ikchains(char* buffer, temp::rig_t& rig, int numikchains, int ikchainindex) {
	//ik chains
	r5::v8::mstudioikchain_t* pIkchains = PTR_FROM_IDX(r5::v8::mstudioikchain_t, buffer, ikchainindex);
	for (int i = 0; i < numikchains; i++) {
		temp::ikchain_t ikchain{};
		ikchain.name = STRING_FROM_IDX(&pIkchains[i], pIkchains[i].sznameindex);
		ikchain.linktype = pIkchains[i].linktype;
		ikchain.unk = pIkchains[i].unk;

		r5::v8::mstudioiklink_t* pIklinks = PTR_FROM_IDX(r5::v8::mstudioiklink_t, &pIkchains[i], pIkchains[i].linkindex);
		for (int j = 0; j < pIkchains[i].numlinks; j++) {
			temp::iklink_t iklink{};
			iklink.bone = pIklinks[j].bone;
			iklink.kneeDir = pIklinks[j].kneeDir;
			ikchain.iklinks.push_back(iklink);
		}
		rig.ikchains.push_back(ikchain);
	}
}

static void ParseRRIG_v16_ikchains(char* buffer, temp::rig_t& rig, int numikchains, int ikchainindex) {
	//ik chains
	auto* pIkchains = PTR_FROM_IDX(r5::v16::mstudioikchain_t, buffer, ikchainindex);
	for (int i = 0; i < numikchains; i++) {
		temp::ikchain_t ikchain{};
		ikchain.name = STRING_FROM_IDX(&pIkchains[i], pIkchains[i].sznameindex);
		ikchain.linktype = pIkchains[i].linktype;
		ikchain.unk = pIkchains[i].unk;

		auto* pIklinks = PTR_FROM_IDX(r5::v8::mstudioiklink_t, &pIkchains[i], pIkchains[i].linkindex);
		for (int j = 0; j < pIkchains[i].numlinks; j++) {
			temp::iklink_t iklink{};
			iklink.bone = pIklinks[j].bone;
			iklink.kneeDir = pIklinks[j].kneeDir;
			ikchain.iklinks.push_back(iklink);
		}
		rig.ikchains.push_back(ikchain);
	}
}

void ParseRRIG_v121(char* buffer, temp::rig_t& rig) {
	// Header
	auto* pRigHdr = reinterpret_cast<r5::v121::studiohdr_t*>(buffer);
	rig.name = STRING_FROM_IDX(pRigHdr, pRigHdr->sznameindex);
	rig.hdr.eyeposition = pRigHdr->eyeposition;
	rig.hdr.illumposition = pRigHdr->illumposition;
	rig.hdr.hull_min = pRigHdr->hull_min;
	rig.hdr.hull_max = pRigHdr->hull_max;
	rig.hdr.view_bbmin = pRigHdr->view_bbmin;
	rig.hdr.view_bbmax = pRigHdr->view_bbmax;
	rig.hdr.flags = pRigHdr->flags;
	rig.hdr.numbones = pRigHdr->numbones;
	rig.hdr.activitylistversion = pRigHdr->activitylistversion;
	rig.hdr.mass = pRigHdr->mass;
	rig.hdr.contents = pRigHdr->contents;
	rig.hdr.defaultFadeDist = pRigHdr->fadeDistance;
	rig.hdr.gathersize = pRigHdr->gathersize;
	rig.hdr.mins = pRigHdr->mins;
	rig.hdr.maxs = pRigHdr->maxs;
	rig.hdr.surfaceprop = STRING_FROM_IDX(pRigHdr, pRigHdr->surfacepropindex);

	ParseRRIG_v121_bones(buffer, rig, pRigHdr->boneindex, pRigHdr->bonetablebynameindex);
	ParseRRIG_v8_hitboxes(buffer, rig, pRigHdr->numhitboxsets, pRigHdr->hitboxsetindex);
	ParseRRIG_v8_nodes(buffer, rig, pRigHdr->numlocalnodes, pRigHdr->localnodenameindex, pRigHdr->nodedataindexindex);
	ParseRRIG_v8_poseparams(buffer, rig, pRigHdr->numlocalposeparameters, pRigHdr->localposeparamindex);
	ParseRRIG_v8_ikchains(buffer, rig, pRigHdr->numikchains, pRigHdr->ikchainindex);
}

void ParseRRIG_v13(char* buffer, temp::rig_t& rig) {
	// Header
	auto* pRigHdr = reinterpret_cast<r5::v13::studiohdr_t*>(buffer);
	rig.name = STRING_FROM_IDX(pRigHdr, pRigHdr->sznameindex);
	rig.hdr.eyeposition = pRigHdr->eyeposition;
	rig.hdr.illumposition = pRigHdr->illumposition;
	rig.hdr.hull_min = pRigHdr->hull_min;
	rig.hdr.hull_max = pRigHdr->hull_max;
	rig.hdr.view_bbmin = pRigHdr->view_bbmin;
	rig.hdr.view_bbmax = pRigHdr->view_bbmax;
	rig.hdr.flags = pRigHdr->flags;
	rig.hdr.numbones = pRigHdr->numbones;
	rig.hdr.activitylistversion = pRigHdr->activitylistversion;
	rig.hdr.mass = pRigHdr->mass;
	rig.hdr.contents = pRigHdr->contents;
	rig.hdr.defaultFadeDist = pRigHdr->fadeDistance;
	rig.hdr.gathersize = pRigHdr->gathersize;
	rig.hdr.mins = pRigHdr->mins;
	rig.hdr.maxs = pRigHdr->maxs;
	rig.hdr.surfaceprop = STRING_FROM_IDX(pRigHdr, pRigHdr->surfacepropindex);

	ParseRRIG_v121_bones(buffer, rig, pRigHdr->boneindex, pRigHdr->bonetablebynameindex);
	ParseRRIG_v8_hitboxes(buffer, rig, pRigHdr->numhitboxsets, pRigHdr->hitboxsetindex);
	ParseRRIG_v8_nodes(buffer, rig, pRigHdr->numlocalnodes, pRigHdr->localnodenameindex, pRigHdr->nodedataindexindex);
	ParseRRIG_v8_poseparams(buffer, rig, pRigHdr->numlocalposeparameters, pRigHdr->localposeparamindex);
	ParseRRIG_v8_ikchains(buffer, rig, pRigHdr->numikchains, pRigHdr->ikchainindex);
}

void ParseRRIG_v14(char* buffer, temp::rig_t& rig) {
	// Header
	auto* pRigHdr = reinterpret_cast<r5::v14::studiohdr_t*>(buffer);
	rig.name = STRING_FROM_IDX(pRigHdr, pRigHdr->sznameindex);
	rig.hdr.eyeposition = pRigHdr->eyeposition;
	rig.hdr.illumposition = pRigHdr->illumposition;
	rig.hdr.hull_min = pRigHdr->hull_min;
	rig.hdr.hull_max = pRigHdr->hull_max;
	rig.hdr.view_bbmin = pRigHdr->view_bbmin;
	rig.hdr.view_bbmax = pRigHdr->view_bbmax;
	rig.hdr.flags = pRigHdr->flags;
	rig.hdr.numbones = pRigHdr->numbones;
	rig.hdr.activitylistversion = pRigHdr->activitylistversion;
	rig.hdr.mass = pRigHdr->mass;
	rig.hdr.contents = pRigHdr->contents;
	rig.hdr.defaultFadeDist = pRigHdr->fadeDistance;
	rig.hdr.gathersize = pRigHdr->gathersize;
	rig.hdr.mins = pRigHdr->bvhMin;
	rig.hdr.maxs = pRigHdr->bvhMax;
	rig.hdr.surfaceprop = STRING_FROM_IDX(pRigHdr, pRigHdr->surfacepropindex);

	ParseRRIG_v121_bones(buffer, rig, pRigHdr->boneindex, pRigHdr->bonetablebynameindex);
	ParseRRIG_v8_hitboxes(buffer, rig, pRigHdr->numhitboxsets, pRigHdr->hitboxsetindex);
	ParseRRIG_v16_nodes(buffer, rig, pRigHdr->numlocalnodes, pRigHdr->localnodenameindex, pRigHdr->localNodeDataOffset);
	ParseRRIG_v8_poseparams(buffer, rig, pRigHdr->numlocalposeparameters, pRigHdr->localposeparamindex);
	ParseRRIG_v8_ikchains(buffer, rig, pRigHdr->numikchains, pRigHdr->ikchainindex);
}

void ParseRRIG_v16(char* buffer, temp::rig_t& rig) {
	// Header
	auto* pRigHdr = reinterpret_cast<r5::v16::studiohdr_t*>(buffer);
	rig.name = STRING_FROM_IDX(pRigHdr, pRigHdr->sznameindex);
	rig.hdr.illumposition = pRigHdr->illumposition;
	rig.hdr.hull_min = pRigHdr->hull_min;
	rig.hdr.hull_max = pRigHdr->hull_max;
	rig.hdr.view_bbmin = pRigHdr->view_bbmin;
	rig.hdr.view_bbmax = pRigHdr->view_bbmax;
	rig.hdr.flags = pRigHdr->flags;
	rig.hdr.numbones = pRigHdr->numbones;
	rig.hdr.activitylistversion = pRigHdr->activitylistversion;
	rig.hdr.mass = pRigHdr->mass;
	rig.hdr.defaultFadeDist = pRigHdr->fadeDistance;
	rig.hdr.gathersize = pRigHdr->gathersize;
	rig.hdr.surfaceprop = STRING_FROM_IDX(pRigHdr, pRigHdr->surfacepropindex);

	ParseRRIG_v16_bones(buffer, rig, pRigHdr->boneindex, pRigHdr->boneDataOffset, pRigHdr->bonetablebynameindex);
	ParseRRIG_v16_hitboxes(buffer, rig, pRigHdr->numhitboxsets, pRigHdr->hitboxsetindex);
	ParseRRIG_v16_nodes(buffer, rig, pRigHdr->numlocalnodes, pRigHdr->localnodenameindex, pRigHdr->nodedataindexindex);
	ParseRRIG_v16_poseparams(buffer, rig, pRigHdr->numlocalposeparameters, pRigHdr->localposeparamindex);
	ParseRRIG_v16_ikchains(buffer, rig, pRigHdr->numikchains, pRigHdr->ikchainindex);
}

void ParseRRIG_v17(char* buffer, temp::rig_t& rig) {
	// Header
	auto* pRigHdr = reinterpret_cast<r5::v17::studiohdr_t*>(buffer);
	rig.name = STRING_FROM_IDX(pRigHdr, pRigHdr->sznameindex);
	rig.hdr.illumposition = pRigHdr->illumposition;
	rig.hdr.hull_min = pRigHdr->hull_min;
	rig.hdr.hull_max = pRigHdr->hull_max;
	rig.hdr.view_bbmin = pRigHdr->view_bbmin;
	rig.hdr.view_bbmax = pRigHdr->view_bbmax;
	rig.hdr.flags = pRigHdr->flags;
	rig.hdr.numbones = pRigHdr->boneCount;
	rig.hdr.activitylistversion = pRigHdr->activitylistversion;
	rig.hdr.mass = pRigHdr->mass;
	rig.hdr.defaultFadeDist = pRigHdr->fadeDistance;
	rig.hdr.gathersize = pRigHdr->gathersize;
	rig.hdr.surfaceprop = STRING_FROM_IDX(pRigHdr, pRigHdr->surfacepropindex);

	ParseRRIG_v16_bones(buffer, rig, pRigHdr->boneHdrOffset, pRigHdr->boneDataOffset, pRigHdr->bonetablebynameindex);
	ParseRRIG_v16_hitboxes(buffer, rig, pRigHdr->numhitboxsets, pRigHdr->hitboxsetindex);
	ParseRRIG_v16_nodes(buffer, rig, pRigHdr->numlocalnodes, pRigHdr->localnodenameindex, pRigHdr->localNodeDataOffset);
	ParseRRIG_v16_poseparams(buffer, rig, pRigHdr->numlocalposeparameters, pRigHdr->localposeparamindex);
	ParseRRIG_v16_ikchains(buffer, rig, pRigHdr->numikchains, pRigHdr->ikchainindex);
}

void WriteRRIG_v8(std::string output_dir, const temp::rig_t& rig) {
	std::vector<char> buffer(8 * 1024 * 1024, 0);
	char* pBase = buffer.data();
	char* pData = pBase;
	temp::StringTable stringTables{};
	stringTables.Init();

	// Header
	r5::v8::studiohdr_t* pRigHdr = reinterpret_cast<r5::v8::studiohdr_t*>(pData);

	pRigHdr->id = 'TSDI';
	pRigHdr->version = 54;
	pRigHdr->checksum = 0x454E494E;
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
	pRigHdr->boneindex = static_cast<int32_t>(pData - pBase);
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
	pRigHdr->bonecontrollerindex = static_cast<uint32_t>(pData - pBase);

	//hboxset
	pRigHdr->numhitboxsets = static_cast<uint32_t>(rig.hitboxsets.size());
	pRigHdr->hitboxsetindex = static_cast<uint32_t>(pData - pBase);
	r5::v8::mstudiohitboxset_t* pHitboxsets = reinterpret_cast<r5::v8::mstudiohitboxset_t*>(pData);
	for (int i = 0; i < pRigHdr->numhitboxsets; i++) {
		stringTables.Add((char*)&pHitboxsets[i], &pHitboxsets[i].sznameindex, rig.hitboxsets[i].name);
		pHitboxsets[i].numhitboxes = static_cast<uint32_t>(rig.hitboxsets[i].hitboxes.size());
		pData += sizeof(r5::v8::mstudiohitboxset_t);
	}

	for (int i = 0; i < pRigHdr->numhitboxsets; i++) {
		r5::v8::mstudiobbox_t* pHitboxes = reinterpret_cast<r5::v8::mstudiobbox_t*>(pData);
		pHitboxsets[i].hitboxindex = static_cast<uint32_t>(pData - (char*)&pHitboxsets[i]);
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
	pRigHdr->bonetablebynameindex = static_cast<uint32_t>(pData - pBase);
	char* pBonebynames = reinterpret_cast<char*>(pData);
	memcpy_s(pBonebynames, 256, rig.bonebyname.data(), pRigHdr->numbones);
	pData += sizeof(char) * pRigHdr->numbones;
	ALIGN4(pData);


	pRigHdr->localattachmentindex = static_cast<uint32_t>(pData - pBase);

	//node name
	pRigHdr->numlocalnodes = static_cast<uint32_t>(rig.nodes.size());
	pRigHdr->localnodenameindex = static_cast<uint32_t>(pData - pBase);
	for (int i = 0; i < pRigHdr->numlocalnodes; i++) {
		int* pNodenames = reinterpret_cast<int*>(pData);
		stringTables.Add((char*)pRigHdr, &pNodenames[i], rig.nodes[i].name);
	}
	pData += sizeof(int) * pRigHdr->numlocalnodes;

	//node data index (allocate)
	pRigHdr->nodeDataOffsetsOffset = static_cast<uint32_t>(pData - pBase);
	int* pNodeDataIndex = reinterpret_cast<int*>(pData);
	pData += sizeof(int) * pRigHdr->numlocalnodes;

	//node data
	for (int i = 0; i < pRigHdr->numlocalnodes; i++) {
		pNodeDataIndex[i] = static_cast<uint32_t>(pData - pBase);
		int transitionsCount = static_cast<uint32_t>(rig.nodes[i].nodedatas.size());
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
	pRigHdr->numlocalposeparameters = static_cast<uint32_t>(rig.poseparams.size());
	pRigHdr->localposeparamindex = static_cast<uint32_t>(pData - pBase);
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
	pRigHdr->numikchains = static_cast<uint32_t>(rig.ikchains.size());
	pRigHdr->ikchainindex = static_cast<uint32_t>(pData - pBase);
	r5::v8::mstudioikchain_t* pIkchains = reinterpret_cast<r5::v8::mstudioikchain_t*>(pData);
	for (int i = 0; i < pRigHdr->numikchains; i++) {
		stringTables.Add((char*)&pIkchains[i], &pIkchains[i].sznameindex, rig.ikchains[i].name);
		pIkchains[i].linktype = rig.ikchains[i].linktype;
		pIkchains[i].unk = rig.ikchains[i].unk;
		pIkchains[i].numlinks = static_cast<uint32_t>(rig.ikchains[i].iklinks.size());
	}
	pData += sizeof(r5::v8::mstudioikchain_t) * pRigHdr->numikchains;

	//ik links
	for (int i = 0; i < pRigHdr->numikchains; i++) {
		pIkchains[i].linkindex = static_cast<uint32_t>(pData - (char*)&pIkchains[i]);
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
	pRigHdr->length = static_cast<uint32_t>(pData - pBase);
	outRrig.write(pBase, pData - pBase);
	std::vector<char>().swap(buffer);
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