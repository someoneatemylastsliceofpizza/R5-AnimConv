#pragma once
#include <vector>
#include <define.h>

namespace p2 {
	struct mstudioanimsections_t {
		int32_t animblock;
		int32_t animindex;
	};
	struct mstudio_rle_anim_t {
		unsigned char bone;
		char flags;
		int16_t nextoffset;
	};

	struct mstudiohitboxset_t {
		int32_t sznameindex;
		int32_t numhitboxes;
		int32_t hitboxindex;
	};

	struct mstudiobbox_t {
		int32_t bone;
		int32_t group;
		Vector3 bbmin;
		Vector3 bbmax;
		int32_t szhitboxnameindex;
		int32_t unused[8];
	};

	struct mstudioposeparamdesc_t {
		int	sznameindex;
		int32_t flags;
		float start;
		float end;
		float loop;
	};

	struct mstudioiklink_t {
		int32_t bone;
		Vector3	kneeDir;
		Vector3	unused0;
	};

	struct mstudiolinearbone_t {
		int32_t numbones;
		int32_t flagsindex;
		int	parentindex;
		int	posindex;
		int32_t quatindex;
		int32_t rotindex;
		int32_t posetoboneindex;
		int	posscaleindex;
		int	rotscaleindex;
		int	qalignmentindex;
		int32_t unused[6];
	};

	struct mstudioikchain_t {
		int32_t sznameindex;
		int32_t linktype;
		int32_t numlinks;
		int32_t linkindex;
	};

	struct mstudioautolayer_t {
		int16_t iSequence;
		int16_t iPose;
		int32_t flags;
		float start;
		float peak;
		float tail;
		float end;
	};

	struct mstudiobone_t {
		int32_t sznameindex;
		int32_t parent;
		int32_t bonecontroller[6];
		Vector3 pos;
		Quaternion quat;
		Vector3 rot;
		Vector3 posscale;
		Vector3 rotscale;
		Matrix3x4_t poseToBone;
		Quaternion qAlignment;
		int32_t flags;
		int32_t proctype;
		int32_t procindex;
		int32_t physicsbone;
		int32_t surfacepropidx;
		int32_t contents;
		int32_t surfacepropLookup;
		int32_t unused[7];
	};

	struct studioanimvalue_ptr_t {
		int16_t x;
		int16_t y;
		int16_t z;

		inline short& operator[](int32_t i) {
			return ((short*)this)[i];
		}

		inline int16_t operator[](int32_t i) const {
			return ((short*)this)[i];
		}
	};

	struct studiohdr_t {
		int32_t id;
		int32_t version;
		long checksum;
		char name[64];
		int32_t length;
		Vector3 eyeposition;
		Vector3 illumposition;
		Vector3 hull_min;
		Vector3 hull_max;
		Vector3 view_bbmin;
		Vector3 view_bbmax;
		int32_t flags;
		int32_t numbones;
		int32_t boneindex;
		int32_t numbonecontrollers;
		int32_t bonecontrollerindex;
		int32_t numhitboxsets;
		int32_t hitboxsetindex;
		int32_t numlocalanim;
		int32_t localanimindex;
		int32_t numlocalseq;
		int32_t localseqindex;
		int32_t activitylistversion;
		int32_t eventsindexed;
		int32_t numtextures;
		int32_t textureindex;
		int32_t numcdtextures;
		int32_t cdtextureindex;
		int32_t numskinref;
		int32_t numskinfamilies;
		int32_t skinindex;
		int32_t numbodyparts;
		int32_t bodypartindex;
		int32_t numlocalattachments;
		int32_t localattachmentindex;
		int32_t numlocalnodes;
		int32_t localnodeindex;
		int32_t localnodenameindex;
		int32_t numflexdesc;
		int32_t flexdescindex;
		int32_t numflexcontrollers;
		int32_t flexcontrollerindex;
		int32_t numflexrules;
		int32_t flexruleindex;
		int32_t numikchains;
		int32_t ikchainindex;
		int32_t nummouths;
		int32_t mouthindex;
		int32_t numlocalposeparameters;
		int32_t localposeparamindex;
		int32_t surfacepropindex;
		int32_t keyvalueindex;
		int32_t keyvaluesize;
		int32_t numlocalikautoplaylocks;
		int32_t localikautoplaylockindex;
		float mass;
		int32_t contents;
		int32_t numincludemodels;
		int32_t includemodelindex;
		int32_t virtualModel;
		int32_t szanimblocknameindex;
		int32_t numanimblocks;
		int32_t animblockindex;
		int32_t animblockModel;
		int32_t bonetablebynameindex;
		int32_t pVertexBase;
		int32_t pIndexBase;
		char constdirectionallightdot;
		char rootLOD;
		char numAllowedRootLODs;
		char unused[1];
		int32_t unused4;
		int32_t numflexcontrollerui;
		int32_t flexcontrolleruiindex;
		float flVertAnimFixedPointScale;
		mutable int32_t surfacepropLookup;
		int32_t studiohdr2index;
		int32_t unused2[1];

		int32_t numsrcbonetransform;
		int32_t srcbonetransformindex;
		int	illumpositionattachmentindex;
		float flMaxEyeDeflection;
		int32_t linearboneindex;
		int32_t sznameindex;
		int32_t m_nBoneFlexDriverCount;
		int32_t m_nBoneFlexDriverIndex;
		int32_t reserved[56];
	};


	struct mstudioseqdesc_t {
		int32_t baseptr;
		int	sznameindex;
		int32_t szactivitynameindex;
		int32_t flags;
		int32_t activity;
		int32_t actweight;
		int32_t numevents;
		int32_t eventindex;
		Vector3 bbmin;
		Vector3 bbmax;
		int32_t numblends;
		int32_t animindexindex;
		int32_t movementindex;
		int32_t groupsize[2];
		int32_t paramindex[2];
		float paramstart[2];
		float paramend[2];
		int32_t paramparent;
		float fadeintime;
		float fadeouttime;
		int32_t localentrynode;
		int32_t localexitnode;
		int32_t nodeflags;
		float entryphase;
		float exitphase;
		float lastframe;
		int32_t nextseq;
		int32_t pose;
		int32_t numikrules;
		int32_t numautolayers;
		int32_t autolayerindex;
		int32_t weightlistindex;
		int32_t posekeyindex;
		int32_t numiklocks;
		int32_t iklockindex;
		int	keyvalueindex;
		int32_t keyvaluesize;
		int32_t cycleposeindex;
		int32_t activitymodifierindex;
		int32_t numactivitymodifiers;
		int32_t unused[5];
	};

	struct mstudioevent_t {
		float cycle;
		int32_t event;
		int32_t type;
		char options[64];
		int32_t szeventindex;

	};

	struct mstudioanimdesc_t {
		int32_t baseptr;
		int32_t sznameindex;
		float fps;
		int32_t flags;
		int32_t numframes;
		int	nummovements;
		int32_t movementindex;
		int32_t ikrulezeroframeindex;
		int32_t unused1[5];
		int32_t animblock;
		int32_t animindex;
		int32_t numikrules;
		int32_t ikruleindex;
		int32_t animblockikruleindex;
		int32_t numlocalhierarchy;
		int32_t localhierarchyindex;
		int32_t sectionindex;
		int32_t sectionframes;
		int16_t zeroframespan;
		int16_t zeroframecount;
		int32_t zeroframeindex;
		float zeroframestalltime;
	};

	struct mstudioikrule_t {
		int32_t index;
		int32_t type;
		int32_t chain;
		int	bone;
		int32_t slot;
		float height;
		float radius;
		float floor;
		Vector3 pos;
		Quaternion q;
		int32_t compressedikerrorindex;
		int32_t unused2;
		int32_t iStart;
		int32_t ikerrorindex;
		float start;
		float peak;
		float tail;
		float end;
		float unused3;
		float contact;
		float drop;
		float top;
		int32_t unused6;
		int32_t unused7;
		int32_t unused8;
		int32_t szattachmentindex;
		int32_t unused[7];
	};
}

namespace r2 {
	struct studiohdr_t {
		int32_t magic;
		int32_t version;
		int32_t checksum;
		int32_t sznameindex;
		char name[64];
		int32_t length;
		Vector3 eyeposition;
		Vector3 illumposition;
		Vector3 hull_min;
		Vector3 hull_max;
		Vector3 view_bbmin;
		Vector3 view_bbmax;
		int32_t flags;
		int32_t numbones;
		int32_t boneindex;
		int32_t numbonecontrollers;
		int32_t bonecontrollerindex;
		int32_t numhitboxsets;
		int32_t hitboxsetindex;
		int32_t numlocalanim;
		int32_t localanimindex;
		int32_t numlocalseq;
		int	localseqindex;
		int32_t activitylistversion;
		int32_t eventsindexed;
		int32_t numtextures;
		int32_t textureindex;
		int32_t numcdtextures;
		int32_t cdtextureindex;
		int32_t numskinref;
		int32_t numskinfamilies;
		int32_t skinindex;
		int32_t numbodyparts;
		int32_t bodypartindex;
		int32_t numlocalattachments;
		int32_t localattachmentindex;
		int32_t numlocalnodes;
		int32_t localnodeindex;
		int32_t localnodenameindex;
		int32_t localNodeUnk;
		int32_t deprecated_flexdescindex;
		int32_t deprecated_numflexcontrollers;
		int32_t deprecated_flexcontrollerindex;
		int32_t deprecated_numflexrules;
		int32_t deprecated_flexruleindex;
		int32_t numikchains;
		int32_t ikchainindex;
		int32_t uiPanelCount;
		int32_t uiPanelOffset;
		int32_t numlocalposeparameters;
		int32_t localposeparamindex;
		int32_t surfacepropindex;
		int32_t keyvalueindex;
		int32_t keyvaluesize;
		int32_t numlocalikautoplaylocks;
		int32_t localikautoplaylockindex;
		float mass;
		int32_t contents;
		int32_t numincludemodels;//
		int32_t includemodelindex;
		int32_t virtualModel;
		int32_t bonetablebynameindex;
		char constdirectionallightdot;
		char rootLOD;
		char numAllowedRootLODs;
		char unused;
		float fadeDistance;
		int32_t deprecated_numflexcontrollerui;
		int32_t deprecated_flexcontrolleruiindex;
		float flVertAnimFixedPointScale;
		int32_t surfacepropLookup;
		int32_t sourceFilenameOffset;
		int32_t numsrcbonetransform;
		int32_t srcbonetransformindex;
		int	illumpositionattachmentindex;
		int32_t linearboneindex;
		int32_t m_nBoneFlexDriverCount;
		int32_t m_nBoneFlexDriverIndex;
		int32_t m_nPerTriAABBIndex;
		int32_t m_nPerTriAABBNodeCount;
		int32_t m_nPerTriAABBLeafCount;
		int32_t m_nPerTriAABBVertCount;
		int32_t unkStringOffset;
		int32_t vtxOffset;
		int32_t vvdOffset;
		int32_t vvcOffset;
		int32_t phyOffset;
		int32_t vtxSize;
		int32_t vvdSize;
		int32_t vvcSize;
		int32_t phySize;
		int32_t collisionOffset;
		int32_t staticCollisionCount;
		int32_t boneFollowerCount;
		int32_t boneFollowerOffset;
		int32_t unused1[60];

	};

	struct mstudiobone_t {
		int32_t sznameindex;
		int32_t parent;
		int32_t bonecontroller[6];
		Vector3 pos;
		Quaternion quat;
		Vector3 rot;
		Vector3 scale;
		Vector3 posscale;
		Vector3 rotscale;
		Vector3 scalescale;
		Matrix3x4_t poseToBone;
		Quaternion qAlignment;
		int32_t flags;
		int32_t proctype;
		int32_t procindex;
		int32_t physicsbone;
		int32_t surfacepropidx;
		int32_t contents;
		int32_t surfacepropLookup;
		uint16_t collisionIndex;
		uint16_t collisionCount;
		int32_t unused[7];
	};

	struct mstudiohitboxset_t {
		int32_t sznameindex;
		int32_t numhitboxes;
		int32_t hitboxindex;
	};

	struct mstudiobbox_t {
		int32_t bone;
		int32_t group;
		Vector3 bbmin;
		Vector3 bbmax;
		int32_t szhitboxnameindex;
		int32_t critShotOverride;
		int32_t hitdataGroupOffset;
		int32_t unused[6];
	};

	struct mstudioiklink_t {
		int		bone;
		Vector3	kneeDir;
		Vector3	unused0;
	};

	struct mstudioikchain_t {
		int	sznameindex;
		int	linktype;
		int	numlinks;
		int	linkindex;
		float unk;
		int	unused[3];
	};
	struct mstudioposeparamdesc_t {
		int	sznameindex;
		int32_t flags;
		float start;
		float end;
		float loop;
	};

	struct mstudioactivitymodifier_t {
		int32_t sznameindex;
		bool negate;
	};

	struct mstudioseqdesc_t {
		int32_t baseptr;
		int	szlabelindex;
		int32_t szactivitynameindex;
		int32_t flags;
		int32_t activity;
		int32_t actweight;
		int32_t numevents;
		int32_t eventindex;
		Vector3 bbmin;
		Vector3 bbmax;
		int32_t numblends;
		int32_t animindexindex;
		int32_t movementindex;
		int32_t groupsize[2];
		int32_t paramindex[2];
		float paramstart[2];
		float paramend[2];
		int32_t paramparent;
		float fadeintime;
		float fadeouttime;
		int32_t localentrynode;
		int32_t localexitnode;
		int32_t nodeflags;
		float entryphase;
		float exitphase;
		float lastframe;
		int32_t nextseq;
		int32_t pose;
		int32_t numikrules;
		int32_t numautolayers;
		int32_t autolayerindex;
		int32_t weightlistindex;
		int32_t posekeyindex;
		int32_t numiklocks;
		int32_t iklockindex;
		int32_t keyvalueindex;
		int32_t keyvaluesize;
		int32_t cycleposeindex;
		int32_t activitymodifierindex;
		int32_t numactivitymodifiers;
		int32_t ikResetMask;
		int32_t unk_C4;
		int32_t unused[8];
	};

	struct mstudioanimdesc_t {
		int32_t baseptr;
		int32_t sznameindex;
		float fps;
		int32_t flags;
		int32_t numframes;
		int32_t nummovements;
		int32_t movementindex;
		int32_t framemovementindex;
		int32_t animindex;
		int32_t numikrules;
		int32_t ikruleindex;
		int32_t numlocalhierarchy;
		int32_t localhierarchyindex;
		int32_t sectionindex;
		int32_t sectionframes;
		int32_t unused[8];
	};

	struct mstudio_rle_anim_t {
		float posscale;
		uint8_t bone;
		uint8_t flags;
		char pad[2];
		Quaternion64 rot;
		Vector48 pos;
		Vector48 scale;
		int32_t nextoffset;
	};

	struct mstudioikrule_t {
		int32_t index;
		int32_t type;
		int32_t chain;
		int32_t bone;
		int32_t slot;
		float height;
		float radius;
		float floor;
		Vector3 pos;
		Quaternion q;
		int32_t compressedikerrorindex;
		int32_t iStart;
		int32_t ikerrorindex;
		float start;
		float peak;
		float tail;
		float end;
		float contact;
		float drop;
		float top;
		int32_t szattachmentindex;
		float endHeight;
		int32_t unused[8];
	};

	struct animvalue_ptr_t {
		int16_t x;
		int16_t y;
		int16_t z;
		int16_t pad;
	};

	struct mstudiocompressedikerror_t {
		Vector3 posscale;
		Vector3 rotscale;
		int16_t offset[6];
	};

	struct mstudioframemovement_t {
		Vector4 scale;
		int16_t offset[4];
	};
}

namespace r5 {
	namespace anim {
		union mstudioanimvalue_t {
			struct {
				unsigned char type;
				unsigned char total;
			} meta;
			short		value;
		};

		struct mstudio_rle_anim_t {
			uint16_t size : 13;
			uint16_t bAnimScale : 1;
			uint16_t bAnimRotation : 1;
			uint16_t bAnimPosition : 1;
		};

		struct studioanimvalue_ptr_t {
			uint16_t offset : 13;
			uint16_t flags : 3;
			uint8_t idx1;
			uint8_t idx2;
		};

		struct mstudioanimsections_t {
			int32_t animidx;
			int32_t isExternal;
		};

		namespace v7 {
			struct mstudioframemovement_t {
				Vector4 scale;
				int32_t sectionframes;
			};

			struct mstudiocompressedikerror_t {
				Vector3 posscale;
				Vector3 rotscale;
				int32_t sectionframes;
			};

			struct mstudioikrule_t {
				int32_t index;
				int32_t type;
				int32_t chain;
				int32_t bone;
				int32_t slot;
				float height;
				float radius;
				float floor;
				Vector3 pos;
				Quaternion q;

				float scale[6];
				int32_t sectionframes;

				int32_t compressedikerrorindex;
				int32_t iStart;
				int32_t ikerrorindex;
				float start;
				float peak;
				float tail;
				float end;
				float contact;
				float drop;
				float top;
				int32_t szattachmentindex;
				float endHeight;
			};

			struct mstudioanimdesc_t {
				int32_t baseptr;
				int32_t sznameindex;
				float fps;
				int32_t flags;
				int32_t numframes;
				int32_t nummovements;
				int32_t movementindex;
				int32_t framemovementindex;
				int32_t animindex;
				int32_t numikrules;
				int32_t ikruleindex;
				int32_t sectionindex;
				int32_t sectionframes;
			};

			struct mstudioevent_t {
				float cycle;
				int32_t event;
				int32_t type;
				char options[256];
				int32_t szeventindex;
			};

			struct mstudioautolayer_t {
				uint64_t guidSequence;
				int16_t iSequence;
				int16_t iPose;
				int32_t flags;
				float start;
				float peak;
				float tail;
				float end;
			};

			struct mstudioactivitymodifier_t {
				int32_t sznameindex;
				bool negate;
			};

			struct mstudioseqdesc_t {
				int32_t baseptr;
				int	szlabelindex;
				int32_t szactivitynameindex;
				int32_t flags;
				int32_t activity;
				int32_t actweight;
				int32_t numevents;
				int32_t eventindex;
				Vector3 bbmin;
				Vector3 bbmax;
				int32_t numblends;
				int32_t animindexindex;
				int32_t movementindex;
				int32_t groupsize[2];
				int32_t paramindex[2];
				float paramstart[2];
				float paramend[2];
				int32_t paramparent;
				float fadeintime;
				float fadeouttime;
				int32_t localentrynode;
				int32_t localexitnode;
				int32_t nodeflags;
				float entryphase;
				float exitphase;
				float lastframe;
				int32_t nextseq;
				int32_t pose;
				int32_t numikrules;
				int32_t numautolayers;
				int32_t autolayerindex;
				int32_t weightlistindex;
				int32_t posekeyindex;
				int32_t numiklocks;
				int32_t iklockindex;
				int32_t keyvalueindex;
				int32_t keyvaluesize;
				int32_t cycleposeindex;
				int32_t activitymodifierindex;
				int32_t numactivitymodifiers;
				int32_t ikResetMask;
				int32_t unk1;
				int32_t weightFixupOffset;
				int32_t weightFixupCount;
			};
		}

		namespace v10 {
			struct mstudioanim_valueptr_t {
				int16_t offset : 13;
				int16_t flags : 3;
				unsigned char axisIdx1;
				unsigned char axisIdx2;
			};

			struct mstudioikrule_t {
				int32_t index;
				int32_t type;
				int32_t chain;
				int32_t bone;
				int32_t slot;
				float height;
				float radius;
				float floor;
				Vector3 pos;
				Quaternion q;

				float scale[6];
				int32_t sectionframes;

				int32_t compressedikerrorindex;
				int32_t iStart;
				int32_t ikerrorindex;
				float start;
				float peak;
				float tail;
				float end;
				float contact;
				float drop;
				float top;
				int32_t szattachmentindex;
				float endHeight;
			};

			struct mstudioanimdesc_t {
				int32_t baseptr;
				int32_t sznameindex;
				float fps;
				int32_t flags;
				int32_t numframes;
				int32_t nummovements;
				int32_t movementindex;
				int32_t framemovementindex;
				int32_t animindex;
				int32_t numikrules;
				int32_t ikruleindex;
				int32_t sectionindex;
				int32_t sectionstallframes;
				int32_t sectionframes;
				int32_t unk1;
				int32_t unk2;
			};

			struct mstudioevent_t {
				float cycle;
				int	event;
				int32_t type;
				char options[256];
				int32_t szeventindex;
			};

			struct mstudioevent_2_t {
				float cycle;
				int	event;
				int32_t type;
				int32_t unk1;
				char options[256];
				int32_t szeventindex;
			};

			struct mstudioactivitymodifier_t {
				int32_t sznameindex;
				bool negate;
			};

			struct mstudioseqdesc_t {
				int32_t baseptr;
				int	szlabelindex;
				int32_t szactivitynameindex;
				int32_t flags;
				int32_t activity;
				int32_t actweight;
				int32_t numevents;
				int32_t eventindex;
				Vector3 bbmin;
				Vector3 bbmax;
				int32_t numblends;
				int32_t animindexindex;
				int32_t movementindex;
				int32_t groupsize[2];
				int32_t paramindex[2];
				float paramstart[2];
				float paramend[2];
				int32_t paramparent;
				float fadeintime;
				float fadeouttime;
				int32_t localentrynode;
				int32_t localexitnode;
				int32_t nodeflags;
				float entryphase;
				float exitphase;
				float lastframe;
				int32_t nextseq;
				int32_t pose;
				int32_t numikrules;
				int32_t numautolayers;
				int32_t autolayerindex;
				int32_t weightlistindex;
				int32_t posekeyindex;
				int32_t numiklocks;
				int32_t iklockindex;
				int32_t keyvalueindex;
				int32_t keyvaluesize;
				int32_t cycleposeindex;
				int32_t activitymodifierindex;
				int32_t numactivitymodifiers;
				int32_t ikResetMask;
				int32_t unk1;
				int32_t weightFixupOffset;
				int32_t weightFixupCount;
			};
		}

		namespace v11 {
			struct mstudioseqdesc_t{
				uint16_t szlabelindex;
				uint16_t szactivitynameindex;
				int flags;
				uint16_t activity; 
				uint16_t actweight;
				uint16_t numevents;
				uint16_t eventindex;
				Vector3 bbmin; 
				Vector3 bbmax;
				uint16_t numblends;
				uint16_t animindexindex;
				short paramindex[2];
				float paramstart[2];
				float paramend[2]; 
				float fadeintime;
				float fadeouttime;
				uint16_t localentrynode; 
				uint16_t localexitnode;
				uint16_t numikrules;
				uint16_t numautolayers;
				uint16_t autolayerindex;
				uint16_t weightlistindex;
				uint8_t groupsize[2];
				uint16_t posekeyindex;
				uint16_t numiklocks;
				uint16_t iklockindex;
				uint16_t unk_5C;
				uint16_t cycleposeindex; 
				uint16_t activitymodifierindex;
				uint16_t numactivitymodifiers;
				int ikResetMask; 
				int unk1;
				uint16_t weightFixupOffset;
				uint16_t weightFixupCount;
			};

			struct mstudioautolayer_t{
				uint64_t guidSequence;
				int iPose;
				int flags;
				float start;
				float peak;	
				float tail;
				float end;
			};

			struct mstudioactivitymodifier_t{
				uint16_t sznameindex;
				bool negate; 
				char pad;
			};

			struct mstudioseqweightfixup_t{
				float unk_0;
				int bone;
				float unk_8;
				float unk_C;
				float unk_10;
				float unk_14;
			};

			struct mstudioikrule_t{
				short chain;
				short bone;
				char type;
				char slot;
				uint16_t sectionframes;
				float scale[6];
				uint16_t compressedikerrorindex;
				short iStart;
				uint16_t ikerrorindex;
				uint16_t szattachmentindex; 
				float start;
				float peak;
				float tail;
				float end;
				float contact; 
				float drop; 
				float top;
				float height;
				float endHeight;
				float radius;
				float floor;
				Vector3 pos;
				Quaternion q;
			};

			struct mstudioframemovement_t{
				float scale[4];
				int sectionframes;
			};

			struct mstudioevent_t {
				float cycle;
				int	event;
				int32_t type;
				int32_t unk;
				int16_t optionsindex;
				int16_t szeventindex;
			};

			struct mstudioanimdesc_t{
				float fps;
				int flags;
				int numframes;
				uint16_t sznameindex;
				uint16_t framemovementindex;
				int animindex;
				uint16_t numikrules;
				uint16_t ikruleindex;
				int64_t sectionDataExternal;
				uint16_t unk1;
				uint16_t sectionindex;
				uint16_t sectionstallframes;
				uint16_t sectionframes;
			};
		}
		
		namespace v12 {
			struct mstudioseqdesc_t {
				int16_t szlabelindex;
				int16_t szactivitynameindex;
				int32_t flags;
				int16_t activity;
				int16_t actweight;
				int16_t numevents;
				int16_t eventindex;
				Vector3 bbmin;
				Vector3 bbmax;
				int16_t numblends;
				int16_t animindexindex;
				int16_t paramindex[2];
				float paramstart[2];
				float paramend[2];
				float fadeintime;
				float fadeouttime;
				int16_t localentrynode;
				int16_t localexitnode;
				int16_t numikrules;
				int16_t numautolayers;
				uint16_t autolayerindex;
				uint16_t weightlistindex;
				uint8_t groupsize[2];
				uint16_t posekeyindex;
				int16_t numiklocks;
				int16_t iklockindex;
				uint16_t unk_5C;
				int16_t cycleposeindex;
				int16_t activitymodifierindex;
				int16_t numactivitymodifiers;
				int32_t ikResetMask;
				int32_t unk1;
				uint16_t unkindex;
				int16_t unkcount;
			};

			struct mstudiocompressedikerror_t {
				int16_t sectionframes;
				float scale[6];
			};

			struct mstudioikrule_t {
				int16_t index;
				int16_t bone;
				uint8_t type;
				uint8_t slot;
				mstudiocompressedikerror_t compressedikerror;
				int32_t compressedikerrorindex;
				int16_t iStart;
				int16_t ikerrorindex;
				float start;
				float peak;
				float tail;
				float end;
				float contact;
				float drop;
				float top;
				float endHeight;
				float height;
				float radius;
				float floor;
				Vector3 pos;
				Quaternion q;
			};

			struct mstudioanimdesc_t {
				float fps;
				int32_t flags;
				int32_t numframes;
				int16_t sznameindex;
				uint16_t framemovementindex;
				int32_t animindex;
				int16_t numikrules;
				uint16_t ikruleindex;
				__int64 unk2;
				uint16_t unk1;
				int16_t sectionindex;
				int16_t sectionstallframes;
				int16_t sectionframes;
			};

			struct mstudioactivitymodifier_t {
				int16_t sznameindex;
				bool negate;
			};
		}

		namespace v121 {
			struct mstudioseqdesc_t{
				uint16_t szlabelindex;
				uint16_t szactivitynameindex;
				int32_t flags;
				int16_t activity;
				uint16_t actweight;
				uint16_t numevents;
				uint16_t eventindex;
				Vector3 bbmin;
				Vector3 bbmax;
				uint16_t numblends;
				uint16_t animindexindex;
				int16_t paramindex[2];
				float paramstart[2];
				float paramend[2];
				float fadeintime;
				float fadeouttime;
				uint16_t localentrynode;
				uint16_t localexitnode;
				uint16_t numikrules;
				uint16_t numautolayers;
				uint16_t autolayerindex;
				uint16_t weightlistindex;
				uint8_t groupsize[2];
				uint16_t posekeyindex;
				uint16_t numiklocks;
				uint16_t iklockindex;
				uint16_t unk_5C;
				uint16_t cycleposeindex;
				uint16_t activitymodifierindex;
				uint16_t numactivitymodifiers;
				int32_t ikResetMask;
				int32_t unk1;
				uint16_t weightFixupOffset;
				uint16_t weightFixupCount;
				uint16_t noInterpFrameOffset;
				uint16_t noInterpFrameCount;
			};

			struct mstudioautolayer_t{
				uint64_t iSequence; 
				int32_t iPose;
				int32_t flags;
				float start;
				float peak;
				float tail;
				float end;
			};

			struct mstudioseqweightfixup_t {
				float scale;
				int32_t bone;
				float unk_8;
				float unk_C;
				float unk_10;
				float unk_14;
			};

			struct mstudio_nointerpframes_t{
				int32_t firstFrame;
				int32_t lastFrame;
			};

			struct mstudioanimdesc_t {
				float fps;
				int32_t flags;
				int32_t numframes;
				uint16_t sznameindex;
				uint16_t framemovementindex;
				uint16_t numikrules;
				uint8_t unused_12[4];
				uint16_t ikruleindex;
				uint64_t animDataAsset;
				int64_t sectionDataExternal;
				uint16_t unk1;
				uint16_t sectionindex;
				uint16_t sectionstallframes;
				uint16_t sectionframes;
			};
		}

	}
	namespace v8 {
		struct studiohdr_t {
			int32_t id;
			int32_t version;
			int32_t checksum;
			int32_t sznameindex;
			char name[64];
			uint32_t length;
			Vector3 eyeposition;
			Vector3 illumposition;
			Vector3 hull_min;
			Vector3 hull_max;
			Vector3 view_bbmin;
			Vector3 view_bbmax;
			int32_t flags;
			int32_t numbones;
			int32_t boneindex;
			int32_t numbonecontrollers;
			int32_t bonecontrollerindex;
			int32_t numhitboxsets;
			int32_t hitboxsetindex;
			int32_t numlocalanim;
			int32_t localanimindex;
			int32_t numlocalseq;
			int	localseqindex;
			int32_t activitylistversion;
			int32_t materialtypesindex;
			int32_t numtextures;
			int32_t textureindex;
			int32_t numcdtextures;
			int32_t cdtextureindex;
			int32_t numskinref;
			int32_t numskinfamilies;
			int32_t skinindex;
			int32_t numbodyparts;
			int32_t bodypartindex;
			int32_t numlocalattachments;
			int32_t localattachmentindex;
			int32_t numlocalnodes;
			int32_t localnodeindex;
			int32_t localnodenameindex;
			int32_t unkNodeCount;
			int32_t nodeDataOffsetsOffset;
			int32_t meshOffset;
			int32_t deprecated_numflexcontrollers;
			int32_t deprecated_flexcontrollerindex;
			int32_t deprecated_numflexrules;
			int32_t deprecated_flexruleindex;
			int32_t numikchains;
			int32_t ikchainindex;
			int32_t numruimeshes;
			int32_t ruimeshindex;
			int32_t numlocalposeparameters;
			int32_t localposeparamindex;
			int32_t surfacepropindex;
			int32_t keyvalueindex;
			int32_t keyvaluesize;
			int32_t numlocalikautoplaylocks;
			int32_t localikautoplaylockindex;
			float mass;
			int32_t contents;
			int32_t numincludemodels;
			int32_t includemodelindex;
			int32_t virtualModel;
			int32_t bonetablebynameindex;
			char constdirectionallightdot;
			char rootLOD;
			char numAllowedRootLODs;
			char unused;
			float defaultFadeDist;
			float gathersize;
			int32_t deprecated_numflexcontrollerui;
			int32_t deprecated_flexcontrolleruiindex;
			float flVertAnimFixedPointScale;
			int32_t surfacepropLookup;
			int32_t sourceFilenameOffset;
			int32_t numsrcbonetransform;
			int32_t srcbonetransformindex;
			int	illumpositionattachmentindex;
			int32_t linearboneindex;
			int32_t procBoneCount;
			int32_t procBoneTableOffset;
			int32_t linearProcBoneOffset;
			int32_t deprecated_m_nBoneFlexDriverCount;
			int32_t deprecated_m_nBoneFlexDriverIndex;
			int32_t deprecated_m_nPerTriAABBIndex;
			int32_t deprecated_m_nPerTriAABBNodeCount;
			int32_t deprecated_m_nPerTriAABBLeafCount;
			int32_t deprecated_m_nPerTriAABBVertCount;
			int32_t unkStringOffset;
			int32_t vtxOffset;
			int32_t vvdOffset;
			int32_t vvcOffset;
			int32_t phyOffset;
			int32_t vtxSize;
			int32_t vvdSize;
			int32_t vvcSize;
			int32_t phySize;
			int32_t deprecated_unkOffset;
			int32_t deprecated_unkCount;
			int32_t boneFollowerCount;
			int32_t boneFollowerOffset;
			Vector3 mins;
			Vector3 maxs;
			int32_t unk3_v54[3];
			int32_t bvhOffset;
			int16_t unk4_v54[2];
			int32_t vvwOffset;
			int32_t vvwSize;
		};

		struct mstudiobone_t {
			int32_t sznameindex;
			int32_t parent;
			int32_t bonecontroller[6];
			Vector3 pos;
			Quaternion quat;
			Vector3 rot;
			Vector3 scale;
			Matrix3x4_t poseToBone;
			Quaternion qAlignment;
			int32_t flags;
			int32_t proctype;
			int32_t procindex;
			int32_t physicsbone;
			int32_t surfacepropidx;
			int32_t contents;
			int32_t surfacepropLookup;
			int32_t unk;
			int32_t unkid;
		};

		struct mstudiohitboxset_t {
			int32_t sznameindex;
			int32_t numhitboxes;
			int32_t hitboxindex;
		};

		struct mstudiobbox_t {
			int32_t bone;
			int32_t group;
			Vector3 bbmin;
			Vector3 bbmax;
			int32_t szhitboxnameindex;
			int32_t critShotOverride;
			int32_t hitdataGroupOffset;
		};

		//struct mstudiomovement_t {
		//	int32_t endframe;
		//	int32_t motionflags;
		//	float v_start;
		//	float v_end;
		//	float angle;
		//	Vector3 movement;
		//	Vector3 position;
		//};

		struct mstudioiklink_t {
			int32_t bone;
			Vector3	kneeDir;
		};

		struct mstudioikchain_t {
			int32_t sznameindex;
			int32_t linktype;
			int32_t numlinks;
			int32_t linkindex;
			float unk;
		};

		struct mstudioposeparamdesc_t {
			int32_t sznameindex;
			int32_t flags;
			float start;
			float end;
			float loop;
		};

		struct nodedata_t {
			uint16_t tonode;
			uint16_t seq;
		};
	}

	namespace v121 {
		struct studiohdr_t {
			int32_t id;
			int32_t version;
			int32_t checksum;
			int32_t sznameindex;
			char name[64];
			int32_t length;
			Vector3 eyeposition;
			Vector3 illumposition;
			Vector3 hull_min;
			Vector3 hull_max;
			Vector3 view_bbmin;
			Vector3 view_bbmax;
			int32_t flags;
			int32_t numbones;
			int32_t boneindex;
			int32_t numbonecontrollers;
			int32_t bonecontrollerindex;
			int32_t numhitboxsets;
			int32_t hitboxsetindex;
			int32_t numlocalanim;
			int32_t localanimindex;
			int32_t numlocalseq;
			int	localseqindex;
			int32_t activitylistversion;
			int32_t materialtypesindex;
			int32_t numtextures;
			int32_t textureindex;
			int32_t numcdtextures;
			int32_t cdtextureindex;
			int32_t numskinref;
			int32_t numskinfamilies;
			int32_t skinindex;
			int32_t numbodyparts;
			int32_t bodypartindex;
			int32_t numlocalattachments;
			int32_t localattachmentindex;
			int32_t numlocalnodes;
			int32_t localnodeindex;
			int32_t localnodenameindex;
			int32_t numunknodes;
			int32_t nodedataindexindex;
			int32_t numikchains;
			int32_t ikchainindex;
			int32_t numruimeshes;
			int32_t ruimeshindex;
			int32_t numlocalposeparameters;
			int32_t localposeparamindex;
			int32_t surfacepropindex;
			int32_t keyvalueindex;
			int32_t keyvaluesize;
			int32_t numlocalikautoplaylocks;
			int32_t localikautoplaylockindex;
			float mass;
			int32_t contents;
			int32_t numincludemodels;
			int32_t includemodelindex;
			uint32_t virtualModel;
			int32_t bonetablebynameindex;
			int32_t numVGMeshes;
			int32_t vgMeshIndex;
			int32_t boneStateIndex;
			int32_t numBoneStates;
			int32_t unk_v54_v121;
			int32_t vgSize;
			int16_t vgUnk;
			int16_t vgLODCount;
			int32_t vgNumUnknown;
			int32_t vgHeaderIndex;
			int32_t numVGHeaders;
			int32_t vgLODIndex;
			int32_t numVGLODs;
			float fadeDistance;
			float gathersize;
			float flVertAnimFixedPointScale;
			int32_t surfacepropLookup;
			int32_t sourceFilenameOffset;
			int32_t numsrcbonetransform;
			int32_t srcbonetransformindex;
			int	illumpositionattachmentindex;
			int32_t linearboneindex;
			int32_t numprocbonesunk;
			int32_t procbonearrayindex;
			int32_t procbonemaskindex;
			int32_t unkstringindex;
			int32_t vtxindex;
			int32_t vvdindex;
			int32_t vvcindex;
			int32_t vphyindex;
			int32_t vtxsize;
			int32_t vvdsize;
			int32_t vvcsize;
			int32_t vphysize;
			int32_t numbonefollowers;
			int32_t bonefollowerindex;
			Vector3 mins;
			Vector3 maxs;
			int32_t bvh4index;
			int16_t unk4_v54[2];
			int32_t vvwindex;
			int32_t vvwsize;
		};

		struct mstudiomodel_t {
			char name[64];
			int32_t unkindex2;
			int32_t type;
			float boundingradius;
			int32_t nummeshes;
			int32_t meshindex;
			int32_t numvertices;
			int32_t vertexindex;
			int32_t tangentsindex;
			int32_t numattachments;
			int32_t attachmentindex;
			int32_t colorindex;
			int32_t uv2index;
		};

		struct mstudiobone_t {
			int32_t sznameindex;
			int32_t parent;
			int32_t bonecontroller[6];
			Vector3 pos;
			Quaternion quat;
			Vector3 rot;
			Vector3 scale;
			Matrix3x4_t poseToBone;
			Quaternion qAlignment;
			int32_t flags;
			int32_t proctype;
			int32_t procindex;
			int32_t physicsbone;
			int32_t surfacepropidx;
			int32_t contents;
			int32_t surfacepropLookup;
			uint8_t collisionIndex;
			uint8_t unk_B1[3];
		};

	}

	namespace v13 {
		struct studiohdr_t {
			int32_t id;
			int32_t version;
			int32_t checksum;
			int32_t sznameindex;
			char name[64];
			int32_t length;
			Vector3 eyeposition;
			Vector3 illumposition;
			Vector3 hull_min;
			Vector3 hull_max;
			Vector3 view_bbmin;
			Vector3 view_bbmax;
			int32_t flags;
			int32_t numbones;
			int32_t boneindex;
			int32_t numbonecontrollers;
			int32_t bonecontrollerindex;
			int32_t numhitboxsets;
			int32_t hitboxsetindex;
			int32_t numlocalanim;
			int32_t localanimindex;
			int32_t numlocalseq;
			int	localseqindex;
			int32_t activitylistversion;
			int32_t materialtypesindex;
			int32_t numtextures;
			int32_t textureindex;
			int32_t numcdtextures;
			int32_t cdtextureindex;
			int32_t numskinref;
			int32_t numskinfamilies;
			int32_t skinindex;
			int32_t numbodyparts;
			int32_t bodypartindex;
			int32_t numlocalattachments;
			int32_t localattachmentindex;
			int32_t numlocalnodes;
			int32_t localnodeindex;
			int32_t localnodenameindex;
			int32_t numunknodes;
			int32_t nodedataindexindex;
			int32_t numikchains;
			int32_t ikchainindex;
			int32_t numruimeshes;
			int32_t ruimeshindex;
			int32_t numlocalposeparameters;
			int32_t localposeparamindex;
			int32_t surfacepropindex;
			int32_t keyvalueindex;
			int32_t keyvaluesize;
			int32_t numlocalikautoplaylocks;
			int32_t localikautoplaylockindex;
			float mass;
			int32_t contents;
			int32_t numincludemodels;
			int32_t includemodelindex;
			uint32_t virtualModel;
			int32_t bonetablebynameindex;
			int32_t numVGMeshes;
			int32_t vgMeshIndex;
			int32_t boneStateIndex;
			int32_t numBoneStates;
			int32_t unk_v54_v121;
			int32_t vgSize;
			int16_t vgUnk;
			int16_t vgLODCount;
			int32_t vgNumUnknown;
			int32_t vgHeaderIndex;
			int32_t numVGHeaders;
			int32_t vgLODIndex;
			int32_t numVGLODs;
			float fadeDistance;
			float gathersize;
			float flVertAnimFixedPointScale;
			int32_t surfacepropLookup;
			int32_t unk_v54_v122;
			int32_t sourceFilenameOffset;
			int32_t numsrcbonetransform;
			int32_t srcbonetransformindex;
			int	illumpositionattachmentindex;
			int32_t linearboneindex;
			int32_t numprocbonesunk;
			int32_t procbonearrayindex;
			int32_t procbonemaskindex;
			int32_t unkstringindex;
			int32_t vtxindex;
			int32_t vvdindex;
			int32_t vvcindex;
			int32_t vphyindex;
			int32_t vtxsize;
			int32_t vvdsize;
			int32_t vvcsize;
			int32_t vphysize;
			int32_t numbonefollowers;
			int32_t bonefollowerindex;
			Vector3 mins;
			Vector3 maxs;
			int32_t bvh4index;
			int16_t unk4_v54[2];
			int32_t vvwindex;
			int32_t vvwsize;
			int32_t unk1_v54_v13[3];
		};
	}

	namespace v14 {
		struct studiohdr_t {
			int32_t id;
			int32_t version;
			int32_t checksum;
			int32_t sznameindex;
			char name[64];
			int32_t length;
			Vector3 eyeposition;
			Vector3 illumposition;
			Vector3 hull_min;
			Vector3 hull_max;
			Vector3 view_bbmin;
			Vector3 view_bbmax;
			int32_t flags;
			int32_t numbones;
			int32_t boneindex;
			int32_t numbonecontrollers;
			int32_t bonecontrollerindex;
			int32_t numhitboxsets;
			int32_t hitboxsetindex;
			int32_t numlocalanim;
			int32_t localanimindex;
			int32_t numlocalseq;
			int	localseqindex;
			int32_t unk_C8[2];
			int32_t activitylistversion;
			int32_t materialtypesindex;
			int32_t numtextures;
			int32_t textureindex;
			int32_t numcdtextures;
			int32_t cdtextureindex;
			int32_t numskinref;
			int32_t numskinfamilies;
			int32_t skinindex;
			int32_t numbodyparts;
			int32_t bodypartindex;
			int32_t numlocalattachments;
			int32_t localattachmentindex;
			int32_t numlocalnodes;
			int32_t localnodeindex;
			int32_t localnodenameindex;
			int32_t localNodeUnk;
			int32_t localNodeDataOffset;
			int32_t numikchains;
			int32_t ikchainindex;
			int32_t uiPanelCount;
			int32_t uiPanelOffset;
			int32_t numlocalposeparameters;
			int32_t localposeparamindex;
			int32_t surfacepropindex;
			int32_t keyvalueindex;
			int32_t keyvaluesize;
			int32_t numlocalikautoplaylocks;
			int32_t localikautoplaylockindex;
			float mass;
			int32_t contents;
			int32_t numincludemodels;
			int32_t includemodelindex;
			int32_t virtualModel;
			int32_t bonetablebynameindex;
			int32_t meshCount;
			int32_t meshOffset;
			int32_t boneStateOffset;
			int32_t boneStateCount;
			int32_t unk_16C;
			int32_t hwDataSize;
			int16_t vgUnk;
			int16_t vgLODCount;
			int32_t lodMap;
			int32_t groupHeaderOffset;
			int32_t groupHeaderCount;
			int32_t lodOffset;
			int32_t lodCount;
			float fadeDistance;
			float gathersize;
			float flVertAnimFixedPointScale;
			int32_t surfacepropLookup;
			int32_t unk_19C;
			int32_t sourceFilenameOffset;
			int32_t numsrcbonetransform;
			int32_t srcbonetransformindex;
			int	illumpositionattachmentindex;
			int32_t linearboneindex;
			int32_t procBoneCount;
			int32_t procBoneOffset;
			int32_t linearProcBoneOffset;
			int32_t unkStringOffset;
			int32_t vtxOffset;
			int32_t vvdOffset;
			int32_t vvcOffset;
			int32_t phyOffset;
			int32_t vtxSize;
			int32_t vvdSize;
			int32_t vvcSize;
			int32_t phySize;
			int32_t boneFollowerCount;
			int32_t boneFollowerOffset;
			Vector3 bvhMin;
			Vector3 bvhMax;
			int32_t bvhOffset;
			int16_t bvhUnk[2];
			int32_t vvwOffset;
			int32_t vvwSize;
			int32_t unk_214[2];
			int32_t unk_21C;
		};
	}

	namespace v16 {
		struct studiohdr_t {
			int32_t flags;
			int32_t checksum;
			int16_t sznameindex;
			char name[32];
			char unk_v16;
			char surfacepropLookup;
			float mass;
			int32_t version;
			uint16_t hitboxsetindex;
			char numhitboxsets;
			char illumpositionattachmentindex;
			Vector3 illumposition;
			Vector3 hull_min;
			Vector3 hull_max;
			Vector3 view_bbmin;
			Vector3 view_bbmax;
			int16_t numbones;
			uint16_t boneindex;
			uint16_t boneDataOffset;
			int16_t numlocalseq;
			uint16_t localseqindex;
			int16_t unk_v54_v14[2];
			char activitylistversion;
			char numlocalattachments;
			uint16_t localattachmentindex;
			int16_t numlocalnodes;
			uint16_t localnodenameindex;
			uint16_t nodedataindexindex;
			int16_t numikchains;
			uint16_t ikchainindex;
			int16_t numtextures;
			uint16_t textureindex;
			int16_t numskinref;
			int16_t numskinfamilies;
			uint16_t skinindex;
			int16_t numbodyparts;
			uint16_t bodypartindex;
			int16_t numruimeshes;
			uint16_t ruimeshindex;
			int16_t numlocalposeparameters;
			uint16_t localposeparamindex;
			uint16_t surfacepropindex;
			uint16_t keyvalueindex;
			uint16_t numVGMeshes;
			int16_t vgMeshIndex;
			int16_t bonetablebynameindex;
			uint16_t boneStateIndex;
			int16_t numBoneStates;
			uint16_t vgHeaderIndex;
			int16_t numVGHeaders;
			uint16_t vgLODIndex;
			int16_t numVGLODs;
			float fadeDistance;
			float gathersize;
			int16_t numsrcbonetransform;
			uint16_t srcbonetransformindex;
			int16_t sourceFilenameOffset;
			uint16_t linearboneindex;
			uint16_t numprocbones;
			uint16_t procbonetableindex;
			uint16_t linearprocboneindex;
			int16_t numbonefollowers;
			uint16_t bonefollowerindex;
			uint16_t bvh4index;
			char unk5_v16;
			char unk6_v16;
			int16_t unk7_v16;
			int16_t unk8_v16;
			int16_t unk9_v16;
		};

		struct mstudioposeparamdesc_t {
			uint16_t sznameindex;
			uint16_t flags;
			float start;
			float end;
			float loop;
		};

		struct mstudioattachment_t {
			int16_t sznameindex;
			int16_t localbone;
			int32_t flags;
			Matrix3x4_t localmatrix;
		};

		struct mstudiomodel_t {
			int16_t unkindex2;
			int16_t nummeshes;
			int16_t unk_v14;
			int16_t unk1_v14;
			int16_t meshindex;
		};

		struct mstudiobonehdr_t {
			int32_t contents;
			uint8_t unk_4;
			uint8_t surfacepropLookup;
			uint16_t surfacepropidx;
			uint16_t physicsbone;
			uint16_t sznameindex;
		};

		struct mstudiobonedata_t {
			Matrix3x4_t poseToBone;
			Quaternion qAlignment;
			Vector3 pos;
			Quaternion quat;
			Vector3 rot;
			Vector3 scale;
			int16_t parent;
			uint16_t unk_76;
			int32_t flags;
			uint8_t collisionIndex;
			uint8_t proctype;
			uint16_t procindex;
		};

		struct mstudioikchain_t {
			uint16_t sznameindex;
			uint16_t linktype;
			uint16_t numlinks;
			uint16_t linkindex;
			float unk;
		};

		struct mstudioseqdesc_t {
			int16_t szlabelindex;
			int16_t szactivitynameindex;
			int32_t flags;
			int16_t activity;
			int16_t actweight;
			int16_t numevents;
			int16_t eventindex;
			Vector3 bbmin;
			Vector3 bbmax;
			int16_t numblends;
			int16_t animindexindex;
			int16_t paramindex[2];
			float paramstart[2];
			float paramend[2];
			float fadeintime;
			float fadeouttime;
			int16_t localentrynode;
			int16_t localexitnode;
			int16_t numikrules;
			int16_t numautolayers;
			uint16_t autolayerindex;
			uint16_t weightlistindex;
			char groupsize[2];
			uint16_t posekeyindex;
			int16_t numiklocks;
			int16_t iklockindex;
			uint16_t unk_5C;
			int16_t cycleposeindex;
			int16_t activitymodifierindex;
			int16_t numactivitymodifiers;
			int32_t ikResetMask;
			int32_t unk1;
			uint16_t unkindex;
			int16_t unkcount;
		};

		struct mstudiocompressedikerror_t {
			int16_t sectionframes;
			float scale[6];
		};

		struct mstudioikrule_t {
			int16_t index;
			int16_t bone;
			char type;
			char slot;
			mstudiocompressedikerror_t compressedikerror;
			int32_t compressedikerrorindex;
			int16_t iStart;
			int16_t ikerrorindex;
			float start;
			float peak;
			float tail;
			float end;
			float contact;
			float drop;
			float top;
			float endHeight;
			float height;
			float radius;
			float floor;
			Vector3 pos;
			Quaternion q;
		};

		struct mstudioanimdesc_t {
			float fps;
			int32_t flags;
			int32_t numframes;
			int16_t sznameindex;
			uint16_t framemovementindex;
			int32_t animindex;
			int16_t numikrules;
			uint16_t ikruleindex;
			__int64 unk2;
			uint16_t unk1;
			int16_t sectionindex;
			int16_t sectionstallframes;
			int16_t sectionframes;
		};

		struct mstudioactivitymodifier_t {
			int16_t sznameindex;
			bool negate;
		};

		struct mstudioevent_t {
			float cycle;
			int	event;
			int32_t type;
			int32_t unk;
			int16_t szoptionsindex;
			int16_t szeventindex;
		};

		struct mstudiolinearbone_t {
			uint16_t numbones;
			uint16_t flagsindex;
			uint16_t parentindex;
			uint16_t posindex;
			uint16_t quatindex;
			uint16_t rotindex;
			uint16_t posetoboneindex;
		};

		struct mstudiobbox_t {
			int16_t bone;
			int16_t group;
			Vector3 bbmin;
			Vector3 bbmax;
			uint16_t szhitboxnameindex;
			uint16_t hitdataGroupOffset;
		};

		struct mstudiohitboxset_t {
			uint16_t sznameindex;
			uint16_t numhitboxes;
			uint16_t hitboxindex;
		};
	}

	namespace v17 {
		struct studiohdr_t {
			int flags;
			int checksum;
			uint16_t sznameindex; 
			char name[33];
			uint8_t surfacepropLookup;
			float mass;
			int contents;
			uint16_t hitboxsetindex;
			uint8_t numhitboxsets;
			uint8_t illumpositionattachmentindex;
			Vector3 illumposition;
			Vector3 hull_min;
			Vector3 hull_max;
			Vector3 view_bbmin;
			Vector3 view_bbmax;
			uint16_t boneCount; 
			uint16_t boneHdrOffset;
			uint16_t boneDataOffset;
			uint16_t numlocalseq;
			uint16_t localseqindex;
			uint16_t unk_7E[2];
			char activitylistversion;
			uint8_t numlocalattachments;
			uint16_t localattachmentindex;
			uint16_t numlocalnodes;
			uint16_t localnodenameindex;
			uint16_t localNodeDataOffset;
			uint16_t numikchains;
			uint16_t ikchainindex;
			uint16_t numtextures;
			uint16_t textureindex;
			uint16_t numskinref;
			uint16_t numskinfamilies;
			uint16_t skinindex;
			uint16_t numbodyparts;
			uint16_t bodypartindex;
			uint16_t uiPanelCount;
			uint16_t uiPanelOffset;
			uint16_t numlocalposeparameters;
			uint16_t localposeparamindex;
			uint16_t surfacepropindex;
			uint16_t keyvalueindex;
			uint16_t virtualModel;
			uint16_t meshCount;
			uint16_t bonetablebynameindex;
			uint16_t boneStateOffset;
			uint16_t boneStateCount;
			uint16_t groupHeaderOffset;
			uint16_t groupHeaderCount;
			uint16_t lodOffset;
			uint16_t lodCount;
			float fadeDistance;
			float gathersize;
			uint16_t numsrcbonetransform;
			uint16_t srcbonetransformindex;
			uint16_t sourceFilenameOffset;
			uint16_t linearboneindex;
			uint16_t procBoneCount;
			uint16_t procBoneOffset;
			uint16_t linearProcBoneOffset;
			uint16_t boneFollowerCount;
			uint16_t boneFollowerOffset;
			uint16_t bvhOffset;
			char bvhUnk[2]; 
			uint16_t unkDataCount;
			uint16_t unkDataOffset;
			uint16_t unkStrcOffset; 
			int unk_E0;
		};
	}
	namespace v19 {
		struct studiohdr_t {
			int32_t flags;
			int32_t checksum;
			uint16_t sznameindex;
			char name[33];
			uint8_t surfacepropLookup;
			float mass;
			int32_t contents;
			uint16_t hitboxsetindex;
			uint8_t numhitboxsets;
			uint8_t illumpositionattachmentindex;
			Vector3 illumposition;
			Vector3 hull_min;
			Vector3 hull_max;
			Vector3 view_bbmin;
			Vector3 view_bbmax;
			uint16_t boneCount;
			uint16_t boneHdrOffset;
			uint16_t boneDataOffset;
			uint16_t numlocalseq;
			uint16_t localseqindex;
			uint16_t unk_7E[2];
			char activitylistversion;
			uint8_t numlocalattachments;
			uint16_t localattachmentindex;
			uint16_t numlocalnodes;
			uint16_t localnodenameindex;
			uint16_t localNodeDataOffset;
			uint16_t numikchains;
			uint16_t ikchainindex;
			uint16_t numtextures;
			uint16_t textureindex;
			uint16_t numskinref;
			uint16_t numskinfamilies;
			uint16_t skinindex;
			uint16_t numbodyparts;
			uint16_t bodypartindex;
			uint16_t uiPanelCount;
			uint16_t uiPanelOffset;
			uint16_t numlocalposeparameters;
			uint16_t localposeparamindex;
			uint16_t surfacepropindex;
			uint16_t keyvalueindex;
			uint16_t virtualModel;
			uint16_t meshCount;
			uint16_t bonetablebynameindex;
			uint16_t boneStateOffset;
			uint16_t boneStateCount;
			uint16_t groupHeaderOffset;
			uint16_t groupHeaderCount;
			uint16_t lodOffset;
			uint16_t lodCount;
			float fadeDistance;
			float gathersize;
			uint16_t numsrcbonetransform;
			uint16_t srcbonetransformindex;
			uint16_t sourceFilenameOffset;
			uint16_t linearboneindex;
			uint16_t procBoneOffset;
			uint16_t linearProcBoneOffset;
			uint16_t boneFollowerCount;
			uint16_t boneFollowerOffset;
			uint16_t bvhOffset;
			char bvhUnk[2];
			uint16_t unkDataCount;
			uint16_t unkDataOffset;
			uint16_t unkStrcOffset;
			int32_t unk_E0;
		};


		struct mstudiobonedata_t {
			int16_t parent;
			uint16_t unk_76;
			int32_t flags;
			uint8_t collisionIndex;
			uint8_t proctype;
			uint16_t procindex;
			int32_t unk_C;
		};

		struct mstudiolinearbone_t {
			uint16_t numbones;
			uint16_t flagsindex;
			uint16_t parentindex;
			uint16_t posindex;
			uint16_t quatindex;
			uint16_t rotindex;
			uint16_t posetoboneindex;
			uint16_t qalignmentindex;
			uint16_t scaleindex;
		};
	};
}