#pragma once

#include <vector>
#include <define.h>

#define R5_ANIM_RAWPOS		0x01
#define R5_ANIM_RAWROT		0x02
#define R5_ANIM_ANIMPOS		0x04
#define R5_ANIM_ANIMROT		0x08
#define R5_ANIM_RAWROT      0x20
#define R5_ANIM_RAWSCALE 	0x40
#define R5_ANIM_ANIMSCALE	0x80

#define TF2_ANIM_DELTA		0x01
#define TF2_ANIM_RAWPOS		0x02
#define TF2_ANIM_RAWROT		0x04
#define TF2_ANIM_RAWSCALE	0x08
#define TF2_ANIM_NOROT		0x10

#define STUDIO_ANIMPTR_Z	0x01
#define STUDIO_ANIMPTR_Y	0x02
#define STUDIO_ANIMPTR_X	0x04


namespace pt2 {
	struct mstudioanimsections_t
	{
		int animblock;
		int animindex;
	};
	struct mstudio_rle_anim_t
	{
		unsigned char bone;
		char flags;
		short nextoffset;
	};

	struct mstudiohitboxset_t {
		int sznameindex;
		int numhitboxes;
		int hitboxindex;
	}; 

	struct mstudiobbox_t {
		int bone;
		int group;
		Vector3 bbmin;
		Vector3 bbmax;
		int szhitboxnameindex;
		int unused[8];
	};

	struct mstudioposeparamdesc_t
	{
		int	sznameindex;
		int flags;
		float start;
		float end;
		float loop;
	};

	struct mstudioiklink_t
	{
		int bone;
		Vector3	kneeDir;
		Vector3	unused0;
	};

	struct mstudiolinearbone_t
	{
		int numbones;
		int flagsindex;
		int	parentindex;
		int	posindex;
		int quatindex;
		int rotindex;
		int posetoboneindex;
		int	posscaleindex;
		int	rotscaleindex;
		int	qalignmentindex;
		int unused[6];
	};

	struct mstudioikchain_t
	{
		int sznameindex;
		int linktype;
		int numlinks;
		int linkindex;
	};

	struct mstudioautolayer_t
	{
		short iSequence;
		short iPose;
		int flags;
		float start;
		float peak;
		float tail;
		float end;
	};

	struct mstudiobone_t
	{
		int sznameindex;
		int parent;
		int bonecontroller[6];
		Vector3 pos;
		Quaternion quat;
		RadianEuler rot;
		Vector3 posscale;
		Vector3 rotscale;
		Matrix3x4_t poseToBone;
		Quaternion qAlignment;
		int flags;
		int proctype;
		int procindex;
		int physicsbone;
		int surfacepropidx;
		int contents;
		int surfacepropLookup;
		int unused[7];
	};

	struct studioanimvalue_ptr_t {
		short x;
		short y;
		short z;

		inline bool IsAllZero()
		{
			if (x == 0 && y == 0 && z == 0)
				return true;
			else
				return false;
		}

		inline short FirstThatIsNonZero() {
			if (x != 0) return x;
			if (y != 0) return y;
			if (z != 0) return z;
			return 0;
		}
	};

	struct studiohdr_t
	{
		int id;
		int version;
		long checksum;
		char name[64];
		int length;
		Vector3 eyeposition;
		Vector3 illumposition;
		Vector3 hull_min;
		Vector3 hull_max;
		Vector3 view_bbmin;
		Vector3 view_bbmax;
		int flags;
		int numbones;
		int boneindex;
		int numbonecontrollers;
		int bonecontrollerindex;
		int numhitboxsets;
		int hitboxsetindex;
		int numlocalanim;
		int localanimindex;
		int numlocalseq;
		int localseqindex;
		int activitylistversion;
		int eventsindexed;
		int numtextures;
		int textureindex;
		int numcdtextures;
		int cdtextureindex;
		int numskinref;
		int numskinfamilies;
		int skinindex;
		int numbodyparts;
		int bodypartindex;
		int numlocalattachments;
		int localattachmentindex;
		int numlocalnodes;
		int localnodeindex;
		int localnodenameindex;
		int numflexdesc;
		int flexdescindex;
		int numflexcontrollers;
		int flexcontrollerindex;
		int numflexrules;
		int flexruleindex;
		int numikchains;
		int ikchainindex;
		int nummouths;
		int mouthindex;
		int numlocalposeparameters;
		int localposeparamindex;
		int surfacepropindex;
		int keyvalueindex;
		int keyvaluesize;
		int numlocalikautoplaylocks;
		int localikautoplaylockindex;
		float mass;
		int contents;
		int numincludemodels;
		int includemodelindex;
		int virtualModel;
		int szanimblocknameindex;
		int numanimblocks;
		int animblockindex;
		int animblockModel;
		int bonetablebynameindex;
		int pVertexBase;
		int pIndexBase;
		byte constdirectionallightdot;
		byte rootLOD;
		byte numAllowedRootLODs;
		byte unused[1];
		int unused4;
		int numflexcontrollerui;
		int flexcontrolleruiindex;
		float flVertAnimFixedPointScale;
		mutable int surfacepropLookup;
		int studiohdr2index;
		int unused2[1];

		int numsrcbonetransform;
		int srcbonetransformindex;
		int	illumpositionattachmentindex;
		float flMaxEyeDeflection;
		int linearboneindex;
		int sznameindex;
		int m_nBoneFlexDriverCount;
		int m_nBoneFlexDriverIndex;
		int reserved[56];
	};


	struct mstudioseqdesc_t
	{
		int baseptr;
		int	sznameindex;
		int szactivitynameindex;
		int flags;
		int activity;
		int actweight;
		int numevents;
		int eventindex;
		Vector3 bbmin;
		Vector3 bbmax;
		int numblends;
		int animindexindex;
		int movementindex;
		int groupsize[2];
		int paramindex[2];
		float paramstart[2];
		float paramend[2];
		int paramparent;
		float fadeintime;
		float fadeouttime;
		int localentrynode;
		int localexitnode;
		int nodeflags;
		float entryphase;
		float exitphase;
		float lastframe;
		int nextseq;
		int pose;
		int numikrules;
		int numautolayers;
		int autolayerindex;
		int weightlistindex;
		int posekeyindex;
		int numiklocks;
		int iklockindex;
		int	keyvalueindex;
		int keyvaluesize;
		int cycleposeindex;
		int activitymodifierindex;
		int numactivitymodifiers;
		int unused[5];
	};

	struct mstudioevent_t
	{
		float cycle;
		int event;
		int type;
		char options[64];
		int szeventindex;

	};

	struct mstudioanimdesc_t
	{
		int baseptr;
		int sznameindex;
		float fps;
		int flags;
		int numframes;
		int	nummovements;
		int movementindex;
		int ikrulezeroframeindex;
		int unused1[5];
		int animblock;
		int animindex;
		int numikrules;
		int ikruleindex;
		int animblockikruleindex;
		int numlocalhierarchy;
		int localhierarchyindex;
		int sectionindex;
		int sectionframes;
		short zeroframespan;
		short zeroframecount;
		int zeroframeindex;
		float zeroframestalltime;
	};

	struct mstudioikrule_t {
		int index;
		int type;
		int chain;
		int	bone;
		int slot;
		float height;
		float radius;
		float floor;
		Vector3 pos;
		Quaternion q;
		int compressedikerrorindex;
		int unused2;
		int iStart;
		int ikerrorindex;
		float start;
		float peak;
		float tail;
		float end;
		float unused3;
		float contact;
		float drop;
		float top;
		int unused6;
		int unused7;
		int unused8;
		int szattachmentindex;
		int unused[7];
	};
}

namespace tf2 {
	struct studiohdr_t
	{
		int magic;
		int version;
		int checksum;
		int sznameindex;
		char name[64];
		int length;
		Vector3 eyeposition;
		Vector3 illumposition;
		Vector3 hull_min;
		Vector3 hull_max;
		Vector3 view_bbmin;
		Vector3 view_bbmax;
		int flags;
		int numbones;
		int boneindex;
		int numbonecontrollers;
		int bonecontrollerindex;
		int numhitboxsets;
		int hitboxsetindex;
		int numlocalanim;
		int localanimindex;
		int numlocalseq;
		int	localseqindex;
		int activitylistversion;
		int eventsindexed;
		int numtextures;
		int textureindex;
		int numcdtextures;
		int cdtextureindex;
		int numskinref;
		int numskinfamilies;
		int skinindex;
		int numbodyparts;
		int bodypartindex;
		int numlocalattachments;
		int localattachmentindex;
		int numlocalnodes;
		int localnodeindex;
		int localnodenameindex;
		int localNodeUnk;
		int deprecated_flexdescindex;
		int deprecated_numflexcontrollers;
		int deprecated_flexcontrollerindex;
		int deprecated_numflexrules;
		int deprecated_flexruleindex;
		int numikchains;
		int ikchainindex;
		int uiPanelCount;
		int uiPanelOffset;
		int numlocalposeparameters;
		int localposeparamindex;
		int surfacepropindex;
		int keyvalueindex;
		int keyvaluesize;
		int numlocalikautoplaylocks;
		int localikautoplaylockindex;
		float mass;
		int contents;
		int numincludemodels;//
		int includemodelindex;
		int virtualModel;
		int bonetablebynameindex;
		char constdirectionallightdot;
		char rootLOD;
		char numAllowedRootLODs;
		char unused;
		float fadeDistance;
		int deprecated_numflexcontrollerui;
		int deprecated_flexcontrolleruiindex;
		float flVertAnimFixedPointScale;
		int surfacepropLookup;
		int sourceFilenameOffset;
		int numsrcbonetransform;
		int srcbonetransformindex;
		int	illumpositionattachmentindex;
		int linearboneindex;
		int m_nBoneFlexDriverCount;
		int m_nBoneFlexDriverIndex;
		int m_nPerTriAABBIndex;
		int m_nPerTriAABBNodeCount;
		int m_nPerTriAABBLeafCount;
		int m_nPerTriAABBVertCount;
		int unkStringOffset;
		int vtxOffset;
		int vvdOffset;
		int vvcOffset;
		int phyOffset;
		int vtxSize;
		int vvdSize;
		int vvcSize;
		int phySize;
		int collisionOffset;
		int staticCollisionCount;
		int boneFollowerCount;
		int boneFollowerOffset;
		int unused1[60];

	};

	struct mstudiobone_t{
		int sznameindex;
		int parent;
		int bonecontroller[6];
		Vector3 pos;
		Quaternion quat;
		RadianEuler rot;
		Vector3 scale;
		Vector3 posscale;
		Vector3 rotscale;
		Vector3 scalescale;
		Matrix3x4_t poseToBone;
		Quaternion qAlignment;
		int flags;
		int proctype;
		int procindex;
		int physicsbone;
		int surfacepropidx;
		int contents;
		int surfacepropLookup;
		uint16_t collisionIndex;
		uint16_t collisionCount;
		int unused[7];
	};

	struct mstudiohitboxset_t {
		int sznameindex;
		int numhitboxes;
		int hitboxindex;
	};

	struct mstudiobbox_t {
		int bone;
		int group;
		Vector3 bbmin;
		Vector3 bbmax;
		int szhitboxnameindex;
		int critShotOverride;
		int hitdataGroupOffset;
		int unused[6];
	};

	struct mstudioiklink_t{
		int		bone;
		Vector3	kneeDir;
		Vector3	unused0;
	};

	struct mstudioikchain_t{
		int	sznameindex;
		int	linktype;
		int	numlinks;
		int	linkindex;
		float unk;
		int	unused[3];
	};
	struct mstudioposeparamdesc_t
	{
		int	sznameindex;
		int flags;
		float start;
		float end;
		float loop;
	};

	struct mstudioactivitymodifier_t
	{
		int sznameindex;
		bool negate;
	};

	struct mstudioseqdesc_t{
		int baseptr;
		int	szlabelindex;
		int szactivitynameindex;
		int flags;
		int activity;
		int actweight;
		int numevents;
		int eventindex;
		Vector3 bbmin;
		Vector3 bbmax;
		int numblends;
		int animindexindex;
		int movementindex;
		int groupsize[2];
		int paramindex[2];
		float paramstart[2]; 
		float paramend[2];
		int paramparent;
		float fadeintime;
		float fadeouttime;
		int localentrynode;
		int localexitnode;
		int nodeflags;
		float entryphase;
		float exitphase;
		float lastframe;
		int nextseq;
		int pose;
		int numikrules;
		int numautolayers;
		int autolayerindex;
		int weightlistindex;
		int posekeyindex;
		int numiklocks;
		int iklockindex;
		int keyvalueindex;
		int keyvaluesize;
		int cycleposeindex;
		int activitymodifierindex;
		int numactivitymodifiers;
		int ikResetMask;
		int unk_C4;
		int unused[8];
	};

	struct mstudioanimdesc_t{
		int baseptr;
		int sznameindex;
		float fps;
		int flags;
		int numframes;
		int nummovements;
		int movementindex;
		int framemovementindex;
		int animindex;
		int numikrules;
		int ikruleindex;
		int numlocalhierarchy;
		int localhierarchyindex;
		int sectionindex;
		int sectionframes;
		int unused[8];
	};

	struct mstudio_rle_anim_t {
		float posscale;
		uint8_t bone;
		uint8_t flags;
		char pad[2];
		Quaternion64 rot;
		Vector48 pos;
		Vector48 scale;
		int nextoffset;
	};

	struct mstudioikrule_t {
		int index;
		int type;
		int chain;
		int bone;
		int slot;
		float height;
		float radius;
		float floor;
		Vector3 pos;
		Quaternion q;
		int compressedikerrorindex;
		int iStart;
		int ikerrorindex;
		float start;
		float peak;
		float tail;
		float end;
		float contact;
		float drop;
		float top;
		int szattachmentindex;
		float endHeight;
		int unused[8];
	};

	struct animvalue_ptr_t {
		short x;
		short y;
		short z;
		short pad;

		inline bool IsAllZero()
		{
			if (x == 0 && y == 0 && z == 0)
				return true;
			else
				return false;
		}

		inline short FirstThatIsNonZero() {
			if (x != 0) return x;
			if (y != 0) return y;
			if (z != 0) return z;
			return 0;
		}
	};

	struct mstudiocompressedikerror_t {
		Vector3 posscale;
		Vector3 rotscale;
		short offset[6];
	};

	struct framemovement_t {
		Vector4 scale;
		short offset[4];
	};
}

namespace r5r{
	union mstudioanimvalue_t{
		struct{
			unsigned char	valid;
			unsigned char	total;
		} meta;
		short		value;
	};

	struct mstudioanim_valueptr_t
	{
		short offset : 13;
		short flags : 3;
		unsigned char axisIdx1;
		unsigned char axisIdx2;
	};

	struct mstudio_rle_anim_t
	{
		short size : 13;
		short flags : 3;
	};

	struct boneflagarray {
		char flag;
	};

	struct studioanimvalue_ptr_t {
		short offset : 13;
		short flags : 3;
		char idx1;
		char idx2;
	};

	struct mstudiobone_t
	{
		int sznameindex;
		int parent;
		int bonecontroller[6];
		Vector3 pos;
		Quaternion quat;
		RadianEuler rot;
		Vector3 scale;
		Matrix3x4_t poseToBone;
		Quaternion qAlignment;
		int flags;
		int proctype;
		int procindex;
		int physicsbone;
		int surfacepropidx;
		int contents;
		int surfacepropLookup;
		int unk;
		int unkid;
	};

	struct mstudiohitboxset_t
	{
		int sznameindex;
		int numhitboxes;
		int hitboxindex;
	};

	struct mstudiobbox_t {
		int bone;
		int group;
		Vector3 bbmin;
		Vector3 bbmax;
		int szhitboxnameindex;
		int critShotOverride;
		int hitdataGroupOffset;
	};

	//struct mstudiomovement_t {
	//	int endframe;
	//	int motionflags;
	//	float v_start;
	//	float v_end;
	//	float angle;
	//	Vector3 movement;
	//	Vector3 position;
	//};

	struct mstudioframemovement_t
	{
		Vector4 scale;
		int sectionFrames;
	};

	struct mstudiocompressedikerror_t {
		Vector3 posscale;
		Vector3 rotscale;
		int sectionframes;
	};

	struct mstudioikrule_t {
		int index;
		int type;
		int chain;
		int bone;
		int slot;
		float height;
		float radius;
		float floor;
		Vector3 pos;
		Quaternion q;
		mstudiocompressedikerror_t compressedikerror;
		int compressedikerrorindex;
		int iStart;
		int ikerrorindex;
		float start;
		float peak;
		float tail;
		float end;
		float contact;
		float drop;
		float top;
		int szattachmentindex;
		float endHeight;
	};

	struct mstudioanimdesc_t
	{
		int baseptr;
		int sznameindex;
		float fps;
		int flags;
		int numframes;
		int nummovements;
		int movementindex;
		int framemovementindex;
		int animindex;
		int numikrules;
		int ikruleindex;
		int sectionindex;
		int sectionframes; 
	};

	struct mstudioevent_t
	{
		float cycle;
		int event;
		int type;
		char options[256];
		int szeventindex;
	};

	struct mstudioautolayer_t
	{
		unsigned __int64 assetSequence;
		short iSequence;
		short iPose;
		int flags;
		float start;
		float peak;
		float tail;
		float end;
	};

	struct mstudioactivitymodifier_t
	{
		int sznameindex;
		bool negate;
	};

	struct mstudioseqdesc_t
	{
		int baseptr;
		int	szlabelindex;
		int szactivitynameindex;
		int flags;
		int activity;
		int actweight;
		int numevents;
		int eventindex;
		Vector3 bbmin;
		Vector3 bbmax;
		int numblends;
		int animindexindex;
		int movementindex;
		int groupsize[2];
		int paramindex[2];
		float paramstart[2];
		float paramend[2];
		int paramparent;
		float fadeintime;
		float fadeouttime;
		int localentrynode;
		int localexitnode;
		int nodeflags;
		float entryphase;
		float exitphase;
		float lastframe;
		int nextseq;
		int pose;
		int numikrules;
		int numautolayers;
		int autolayerindex;
		int weightlistindex;
		int posekeyindex;
		int numiklocks;
		int iklockindex;
		int keyvalueindex;
		int keyvaluesize;
		int cycleposeindex;
		int activitymodifierindex;
		int numactivitymodifiers;
		int ikResetMask;
		int unk1;
		int unkOffset;
		int unkCount;
	};

	struct mstudioiklink_t
	{
		int bone;
		Vector3	kneeDir;
	};

	struct mstudioikchain_t
	{
		int sznameindex;
		int linktype;
		int numlinks;
		int linkindex;
		float unk;
	};

	struct mstudioposeparamdesc_t
	{
		int sznameindex;
		int flags;
		float start;
		float end;
		float loop;
	};

	struct studiohdr_t
	{
		int id;
		int version;
		int checksum;
		int sznameindex;
		char name[64];
		int length;
		Vector3 eyeposition;
		Vector3 illumposition;
		Vector3 hull_min;
		Vector3 hull_max;
		Vector3 view_bbmin;
		Vector3 view_bbmax;
		int flags;
		int numbones;
		int boneindex;
		int numbonecontrollers;
		int bonecontrollerindex;
		int numhitboxsets;
		int hitboxsetindex;
		int numlocalanim;
		int localanimindex;
		int numlocalseq;
		int	localseqindex;
		int activitylistversion;
		int materialtypesindex;
		int numtextures;
		int textureindex;
		int numcdtextures;
		int cdtextureindex;
		int numskinref;
		int numskinfamilies;
		int skinindex;
		int numbodyparts;
		int bodypartindex;
		int numlocalattachments;
		int localattachmentindex;
		int numlocalnodes;
		int localnodeindex;
		int localnodenameindex;
		int unkNodeCount;
		int nodeDataOffsetsOffset;
		int meshOffset;
		int deprecated_numflexcontrollers;
		int deprecated_flexcontrollerindex;
		int deprecated_numflexrules;
		int deprecated_flexruleindex;
		int numikchains;
		int ikchainindex;
		int uiPanelCount;
		int uiPanelOffset;
		int numlocalposeparameters;
		int localposeparamindex;
		int surfacepropindex;
		int keyvalueindex;
		int keyvaluesize;
		int numlocalikautoplaylocks;
		int localikautoplaylockindex;
		float mass;
		int contents;
		int numincludemodels;
		int includemodelindex;
		int virtualModel;
		int bonetablebynameindex;
		char constdirectionallightdot;
		char rootLOD;
		char numAllowedRootLODs;
		char unused;
		float defaultFadeDist;
		float gatherSize;
		int deprecated_numflexcontrollerui;
		int deprecated_flexcontrolleruiindex;
		float flVertAnimFixedPointScale;
		int surfacepropLookup;
		int sourceFilenameOffset;
		int numsrcbonetransform;
		int srcbonetransformindex;
		int	illumpositionattachmentindex;
		int linearboneindex;
		int procBoneCount;
		int procBoneTableOffset;
		int linearProcBoneOffset;
		int deprecated_m_nBoneFlexDriverCount;
		int deprecated_m_nBoneFlexDriverIndex;
		int deprecated_m_nPerTriAABBIndex;
		int deprecated_m_nPerTriAABBNodeCount;
		int deprecated_m_nPerTriAABBLeafCount;
		int deprecated_m_nPerTriAABBVertCount;
		int unkStringOffset;
		int vtxOffset;
		int vvdOffset;
		int vvcOffset;
		int phyOffset;
		int vtxSize;
		int vvdSize;
		int vvcSize;
		int phySize;
		int deprecated_unkOffset;
		int deprecated_unkCount;
		int boneFollowerCount;
		int boneFollowerOffset;
		Vector3 mins;
		Vector3 maxs;
		int unk3_v54[3];
		int bvhOffset;
		short unk4_v54[2];
		int vvwOffset;
		int vvwSize;
	};
}
