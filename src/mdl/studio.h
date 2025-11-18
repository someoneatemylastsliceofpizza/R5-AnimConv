#pragma once

#include <vector>
#include <define.h>

namespace p2 {
	struct mstudioanimsections_t {
		int animblock;
		int animindex;
	};
	struct mstudio_rle_anim_t {
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

	struct mstudioposeparamdesc_t {
		int	sznameindex;
		int flags;
		float start;
		float end;
		float loop;
	};

	struct mstudioiklink_t {
		int bone;
		Vector3	kneeDir;
		Vector3	unused0;
	};

	struct mstudiolinearbone_t {
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

	struct mstudioikchain_t {
		int sznameindex;
		int linktype;
		int numlinks;
		int linkindex;
	};

	struct mstudioautolayer_t {
		short iSequence;
		short iPose;
		int flags;
		float start;
		float peak;
		float tail;
		float end;
	};

	struct mstudiobone_t {
		int sznameindex;
		int parent;
		int bonecontroller[6];
		Vector3 pos;
		Quaternion quat;
		Vector3 rot;
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

		inline short& operator[](int i) {
			return ((short*)this)[i];
		}

		inline short operator[](int i) const {
			return ((short*)this)[i];
		}
	};

	struct studiohdr_t {
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
		char constdirectionallightdot;
		char rootLOD;
		char numAllowedRootLODs;
		char unused[1];
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


	struct mstudioseqdesc_t {
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

	struct mstudioevent_t {
		float cycle;
		int event;
		int type;
		char options[64];
		int szeventindex;

	};

	struct mstudioanimdesc_t {
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

namespace r2 {
	struct studiohdr_t {
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

	struct mstudiobone_t {
		int sznameindex;
		int parent;
		int bonecontroller[6];
		Vector3 pos;
		Quaternion quat;
		Vector3 rot;
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
		int flags;
		float start;
		float end;
		float loop;
	};

	struct mstudioactivitymodifier_t {
		int sznameindex;
		bool negate;
	};

	struct mstudioseqdesc_t {
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

	struct mstudioanimdesc_t {
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

		inline bool IsAllZero() {
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

	struct mstudioframemovement_t {
		Vector4 scale;
		short offset[4];
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
			int animidx;
			int isExternal;
		};

		namespace v7 {
			struct mstudioframemovement_t {
				Vector4 scale;
				int sectionframes;
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

				float scale[6];
				int sectionframes;

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

			struct mstudioanimdesc_t {
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

			struct mstudioevent_t {
				float cycle;
				int event;
				int type;
				char options[256];
				int szeventindex;
			};

			struct mstudioautolayer_t {
				uint64_t guidSequence;
				short iSequence;
				short iPose;
				int flags;
				float start;
				float peak;
				float tail;
				float end;
			};

			struct mstudioactivitymodifier_t {
				int sznameindex;
				bool negate;
			};

			struct mstudioseqdesc_t {
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
				int weightFixupOffset;
				int weightFixupCount;
			};
		}

		namespace v10 {
			struct mstudioanim_valueptr_t {
				short offset : 13;
				short flags : 3;
				unsigned char axisIdx1;
				unsigned char axisIdx2;
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

				float scale[6];
				int sectionframes;

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

			struct mstudioanimdesc_t {
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
				int sectionstallframes;
				int sectionframes;
				int unk1;
				int unk2;
			};

			struct mstudioevent_t {
				float cycle;
				int	event;
				int type;
				char options[256];
				int szeventindex;
			};

			struct mstudioevent_2_t {
				float cycle;
				int	event;
				int type;
				int unk1;
				char options[256];
				int szeventindex;
			};

			struct mstudioactivitymodifier_t {
				int sznameindex;
				bool negate;
			};

			struct mstudioseqdesc_t {
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
				int weightFixupOffset;
				int weightFixupCount;
			};
		}


		namespace v12 {
			struct mstudioseqdesc_t {
				short szlabelindex;
				short szactivitynameindex;
				int flags;
				short activity;
				short actweight;
				short numevents;
				short eventindex;
				Vector3 bbmin;
				Vector3 bbmax;
				short numblends;
				short animindexindex;
				short paramindex[2];
				float paramstart[2];
				float paramend[2];
				float fadeintime;
				float fadeouttime;
				short localentrynode;
				short localexitnode;
				short numikrules;
				short numautolayers;
				unsigned short autolayerindex;
				unsigned short weightlistindex;
				uint8_t groupsize[2];
				unsigned short posekeyindex;
				short numiklocks;
				short iklockindex;
				unsigned short unk_5C;
				short cycleposeindex;
				short activitymodifierindex;
				short numactivitymodifiers;
				int ikResetMask;
				int unk1;
				unsigned short unkindex;
				short unkcount;
			};

			struct mstudiocompressedikerror_t {
				short sectionframes;
				float scale[6];
			};

			struct mstudioikrule_t {
				short index;
				short bone;
				uint8_t type;
				uint8_t slot;
				mstudiocompressedikerror_t compressedikerror;
				int compressedikerrorindex;
				short iStart;
				short ikerrorindex;
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
				int flags;
				int numframes;
				short sznameindex;
				unsigned short framemovementindex;
				int animindex;
				short numikrules;
				unsigned short ikruleindex;
				__int64 unk2;
				uint16_t unk1;
				short sectionindex;
				short sectionstallframes;
				short sectionframes;
			};

			struct mstudioactivitymodifier_t {
				short sznameindex;
				bool negate;
			};

			struct mstudioevent_t {
				float cycle;
				int	event;
				int type;
				int unk;
				short szoptionsindex;
				short szeventindex;
			};
		}

		namespace v121 {
			struct mstudioanimdesc_t {
				float fps;
				int flags;
				int numframes;
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
			int numruimeshes;
			int ruimeshindex;
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
			float gathersize;
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

		struct mstudiobone_t {
			int sznameindex;
			int parent;
			int bonecontroller[6];
			Vector3 pos;
			Quaternion quat;
			Vector3 rot;
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

		struct mstudioiklink_t {
			int bone;
			Vector3	kneeDir;
		};

		struct mstudioikchain_t {
			int sznameindex;
			int linktype;
			int numlinks;
			int linkindex;
			float unk;
		};

		struct mstudioposeparamdesc_t {
			int sznameindex;
			int flags;
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
			int numunknodes;
			int nodedataindexindex;
			int numikchains;
			int ikchainindex;
			int numruimeshes;
			int ruimeshindex;
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
			unsigned int virtualModel;
			int bonetablebynameindex;
			int numVGMeshes;
			int vgMeshIndex;
			int boneStateIndex;
			int numBoneStates;
			int unk_v54_v121;
			int vgSize;
			short vgUnk;
			short vgLODCount;
			int vgNumUnknown;
			int vgHeaderIndex;
			int numVGHeaders;
			int vgLODIndex;
			int numVGLODs;
			float fadeDistance;
			float gathersize;
			float flVertAnimFixedPointScale;
			int surfacepropLookup;
			int sourceFilenameOffset;
			int numsrcbonetransform;
			int srcbonetransformindex;
			int	illumpositionattachmentindex;
			int linearboneindex;
			int numprocbonesunk;
			int procbonearrayindex;
			int procbonemaskindex;
			int unkstringindex;
			int vtxindex;
			int vvdindex;
			int vvcindex;
			int vphyindex;
			int vtxsize;
			int vvdsize;
			int vvcsize;
			int vphysize;
			int numbonefollowers;
			int bonefollowerindex;
			Vector3 mins;
			Vector3 maxs;
			int bvh4index;
			short unk4_v54[2];
			int vvwindex;
			int vvwsize;
		};

		struct mstudiomodel_t {
			char name[64];
			int unkindex2;
			int type;
			float boundingradius;
			int nummeshes;
			int meshindex;
			int numvertices;
			int vertexindex;
			int tangentsindex;
			int numattachments;
			int attachmentindex;
			int colorindex;
			int uv2index;
		};

		struct mstudiobone_t {
			int sznameindex;
			int parent;
			int bonecontroller[6];
			Vector3 pos;
			Quaternion quat;
			Vector3 rot;
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
			uint8_t collisionIndex;
			uint8_t unk_B1[3];
		};

	}

	namespace v13 {
		struct studiohdr_t {
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
			int numunknodes;
			int nodedataindexindex;
			int numikchains;
			int ikchainindex;
			int numruimeshes;
			int ruimeshindex;
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
			unsigned int virtualModel;
			int bonetablebynameindex;
			int numVGMeshes;
			int vgMeshIndex;
			int boneStateIndex;
			int numBoneStates;
			int unk_v54_v121;
			int vgSize;
			short vgUnk;
			short vgLODCount;
			int vgNumUnknown;
			int vgHeaderIndex;
			int numVGHeaders;
			int vgLODIndex;
			int numVGLODs;
			float fadeDistance;
			float gathersize;
			float flVertAnimFixedPointScale;
			int surfacepropLookup;
			int unk_v54_v122;
			int sourceFilenameOffset;
			int numsrcbonetransform;
			int srcbonetransformindex;
			int	illumpositionattachmentindex;
			int linearboneindex;
			int numprocbonesunk;
			int procbonearrayindex;
			int procbonemaskindex;
			int unkstringindex;
			int vtxindex;
			int vvdindex;
			int vvcindex;
			int vphyindex;
			int vtxsize;
			int vvdsize;
			int vvcsize;
			int vphysize;
			int numbonefollowers;
			int bonefollowerindex;
			Vector3 mins;
			Vector3 maxs;
			int bvh4index;
			short unk4_v54[2];
			int vvwindex;
			int vvwsize;
			int unk1_v54_v13[3];
		};
	}

	namespace v14 {
		struct studiohdr_t {
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
			int unk_C8[2];
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
			int localNodeUnk;
			int localNodeDataOffset;
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
			int meshCount;
			int meshOffset;
			int boneStateOffset;
			int boneStateCount;
			int unk_16C;
			int hwDataSize;
			short vgUnk;
			short vgLODCount;
			int lodMap;
			int groupHeaderOffset;
			int groupHeaderCount;
			int lodOffset;
			int lodCount;
			float fadeDistance;
			float gathersize;
			float flVertAnimFixedPointScale;
			int surfacepropLookup;
			int unk_19C;
			int sourceFilenameOffset;
			int numsrcbonetransform;
			int srcbonetransformindex;
			int	illumpositionattachmentindex;
			int linearboneindex;
			int procBoneCount;
			int procBoneOffset;
			int linearProcBoneOffset;
			int unkStringOffset;
			int vtxOffset;
			int vvdOffset;
			int vvcOffset;
			int phyOffset;
			int vtxSize;
			int vvdSize;
			int vvcSize;
			int phySize;
			int boneFollowerCount;
			int boneFollowerOffset;
			Vector3 bvhMin;
			Vector3 bvhMax;
			int bvhOffset;
			short bvhUnk[2];
			int vvwOffset;
			int vvwSize;
			int unk_214[2];
			int unk_21C;
		};
	}

	namespace v16 {
		struct studiohdr_t {
			int flags;
			int checksum;
			short sznameindex;
			char name[32];
			char unk_v16;
			char surfacepropLookup;
			float mass;
			int version;
			unsigned short hitboxsetindex;
			char numhitboxsets;
			char illumpositionattachmentindex;
			Vector3 illumposition;
			Vector3 hull_min;
			Vector3 hull_max;
			Vector3 view_bbmin;
			Vector3 view_bbmax;
			short numbones;
			unsigned short boneindex;
			unsigned short bonedataindex;
			short numlocalseq;
			unsigned short localseqindex;
			short unk_v54_v14[2];
			char activitylistversion;
			char numlocalattachments;
			unsigned short localattachmentindex;
			short numlocalnodes;
			unsigned short localnodenameindex;
			unsigned short nodedataindexindex;
			short numikchains;
			unsigned short ikchainindex;
			short numtextures;
			unsigned short textureindex;
			short numskinref;
			short numskinfamilies;
			unsigned short skinindex;
			short numbodyparts;
			unsigned short bodypartindex;
			short numruimeshes;
			unsigned short ruimeshindex;
			short numlocalposeparameters;
			unsigned short localposeparamindex;
			unsigned short surfacepropindex;
			unsigned short keyvalueindex;
			unsigned short numVGMeshes;
			short vgMeshIndex;
			short bonetablebynameindex;
			unsigned short boneStateIndex;
			short numBoneStates;
			unsigned short vgHeaderIndex;
			short numVGHeaders;
			unsigned short vgLODIndex;
			short numVGLODs;
			float fadeDistance;
			float gathersize;
			short numsrcbonetransform;
			unsigned short srcbonetransformindex;
			short sourceFilenameOffset;
			unsigned short linearboneindex;
			unsigned short numprocbones;
			unsigned short procbonetableindex;
			unsigned short linearprocboneindex;
			short numbonefollowers;
			unsigned short bonefollowerindex;
			unsigned short bvh4index;
			char unk5_v16;
			char unk6_v16;
			short unk7_v16;
			short unk8_v16;
			short unk9_v16;
		};

		struct mstudioposeparamdesc_t {
			uint16_t sznameindex;
			uint16_t flags;
			float start;
			float end;
			float loop;
		};

		struct mstudioattachment_t {
			short sznameindex;
			short localbone;
			int flags;
			Matrix3x4_t localmatrix;
		};

		struct mstudiomodel_t {
			short unkindex2;
			short nummeshes;
			short unk_v14;
			short unk1_v14;
			short meshindex;
		};

		struct mstudiobonehdr_t {
			int contents;
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
			short parent;
			uint16_t unk_76;
			int flags;
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
			short szlabelindex;
			short szactivitynameindex;
			int flags;
			short activity;
			short actweight;
			short numevents;
			short eventindex;
			Vector3 bbmin;
			Vector3 bbmax;
			short numblends;
			short animindexindex;
			short paramindex[2];
			float paramstart[2];
			float paramend[2];
			float fadeintime;
			float fadeouttime;
			short localentrynode;
			short localexitnode;
			short numikrules;
			short numautolayers;
			unsigned short autolayerindex;
			unsigned short weightlistindex;
			char groupsize[2];
			unsigned short posekeyindex;
			short numiklocks;
			short iklockindex;
			unsigned short unk_5C;
			short cycleposeindex;
			short activitymodifierindex;
			short numactivitymodifiers;
			int ikResetMask;
			int unk1;
			unsigned short unkindex;
			short unkcount;
		};

		struct mstudiocompressedikerror_t {
			short sectionframes;
			float scale[6];
		};

		struct mstudioikrule_t {
			short index;
			short bone;
			char type;
			char slot;
			mstudiocompressedikerror_t compressedikerror;
			int compressedikerrorindex;
			short iStart;
			short ikerrorindex;
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
			int flags;
			int numframes;
			short sznameindex;
			unsigned short framemovementindex;
			int animindex;
			short numikrules;
			unsigned short ikruleindex;
			__int64 unk2;
			uint16_t unk1;
			short sectionindex;
			short sectionstallframes;
			short sectionframes;
		};

		struct mstudioactivitymodifier_t {
			short sznameindex;
			bool negate;
		};

		struct mstudioevent_t {
			float cycle;
			int	event;
			int type;
			int unk;
			short szoptionsindex;
			short szeventindex;
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
			short bone;
			short group;
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

	namespace v19 {
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


		struct mstudiobonedata_t {
			short parent;
			uint16_t unk_76;
			int flags;
			uint8_t collisionIndex;
			uint8_t proctype;
			uint16_t procindex;
			int unk_C;
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