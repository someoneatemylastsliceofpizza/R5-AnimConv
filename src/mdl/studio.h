// Copyright (c) 2023, rexx
// See LICENSE.txt for licensing information (GPL v3)

#pragma once

#include <vector>
#include <define.h>

#define MAX_NUM_LODS 8


// mstudiomovement_t flags
#define STUDIO_X		0x00000001
#define STUDIO_Y		0x00000002	
#define STUDIO_Z		0x00000004
#define STUDIO_XR		0x00000008
#define STUDIO_YR		0x00000010
#define STUDIO_ZR		0x00000020

#define STUDIO_LX		0x00000040
#define STUDIO_LY		0x00000080
#define STUDIO_LZ		0x00000100
#define STUDIO_LXR		0x00000200
#define STUDIO_LYR		0x00000400
#define STUDIO_LZR		0x00000800

#define STUDIO_LINEAR	0x00001000

#define STUDIO_TYPES	0x0003FFFF
#define STUDIO_RLOOP	0x00040000	// controller that wraps shortest distance


// mstudioikrule_t flags
#define IK_SELF 1
#define IK_WORLD 2
#define IK_GROUND 3
#define IK_RELEASE 4
#define IK_ATTACHMENT 5
#define IK_UNLATCH 6


// sequence and autolayer flags
// applies to: mstudioanimdesc_t, mstudioseqdesc_t, mstudioautolayer_t
#define STUDIO_LOOPING	0x0001		// ending frame should be the same as the starting frame
#define STUDIO_SNAP		0x0002		// do not interpolate between previous animation and this one
#define STUDIO_DELTA	0x0004		// this sequence "adds" to the base sequences, not slerp blends
#define STUDIO_AUTOPLAY	0x0008		// temporary flag that forces the sequence to always play
#define STUDIO_POST		0x0010		// 
#define STUDIO_ALLZEROS	0x0020		// this animation/sequence has no real animation data
#define STUDIO_FRAMEANIM 0x0040		// animation is encoded as by frame x bone instead of RLE bone x frame
#define STUDIO_ANIM_UNK3 0x0040
#define STUDIO_CYCLEPOSE 0x0080		// cycle index is taken from a pose parameter index
#define STUDIO_REALTIME	0x0100		// cycle index is taken from a real-time clock, not the animations cycle index
#define STUDIO_LOCAL	0x0200		// sequence has a local context sequence
#define STUDIO_HIDDEN	0x0400		// don't show in default selection views
#define STUDIO_OVERRIDE	0x0800		// a forward declared sequence (empty)
#define STUDIO_ACTIVITY	0x1000		// Has been updated at runtime to activity index
#define STUDIO_EVENT	0x2000		// Has been updated at runtime to event index on server
#define STUDIO_WORLD	0x4000		// sequence blends in worldspace
#define STUDIO_NOFORCELOOP 0x8000	// do not force the animation loop
#define STUDIO_EVENT_CLIENT 0x10000	// Has been updated at runtime to event index on client
#define STUDIO_ANIM_UNK		    0x20000 // actually first in v52, from where??
#define STUDIO_FRAMEMOVEMENT    0x40000 // framemovements are only read if this flag is present
#define STUDIO_ANIM_UNK2	    0x80000 // cherry blossom v53, levi in v54
#define STUDIO_BPANIM			0x100000 // bluepoint's custom animation format, unsure if this exists in the other games

// mstudioevent_t flags
#define NEW_EVENT_STYLE ( 1 << 10 )


// autolayer flags
// mstudioautolayer_t flags
//							0x0001
//							0x0002
//							0x0004
//							0x0008
#define STUDIO_AL_POST		0x0010		// 
//							0x0020
#define STUDIO_AL_SPLINE	0x0040		// convert layer ramp in/out curve is a spline instead of linear
#define STUDIO_AL_XFADE		0x0080		// pre-bias the ramp curve to compense for a non-1 weight, assuming a second layer is also going to accumulate
//							0x0100
#define STUDIO_AL_NOBLEND	0x0200		// animation always blends at 1.0 (ignores weight)
//							0x0400
//							0x0800
#define STUDIO_AL_LOCAL		0x1000		// layer is a local context sequence
#define STUDIO_AL_UNK		0x2000		// observed in v52
#define STUDIO_AL_POSE		0x4000		// layer blends using a pose parameter instead of parent cycle
#define STUDIO_AL_UNK1		0x8000		// added in v53 (probably)


#define RTECH_ANIM_RAWPOS		0x01 // Vector48
#define RTECH_ANIM_RAWROT		0x02 // Quaternion48
#define RTECH_ANIM_ANIMPOS		0x04 // mstudioanim_valueptr_t
#define RTECH_ANIM_ANIMROT		0x08 // mstudioanim_valueptr_t
#define RTECH_ANIM_DELTA	    0x10
#define STUDIO_ANIM_RAWROT  	0x20 // Quaternion64
#define RTECH_ANIM_RAWSCALE 	0x40 // Vector48
#define RTECH_ANIM_ANIMSCALE	0x80 // mstudioanim_valueptr_t 


struct studiohdrshort_t
{
	int id; // Model format ID, such as "IDST" (0x49 0x44 0x53 0x54)
	int version; // Format version number, 52 to 54 for respawn models, 49 and under are valve
	int checksum; // This has to be the same in the phy and vtx files to load!
};

// Garry's Mod & Portal 2, versions 48 & 49 respectively
// not up to the same standards as respawn structs, needs cleaning up
struct mstudioanimsections_t
{
	int animblock; // index of animblock
	int animindex;
};
struct mstudio_rle_anim_t
{
	unsigned char bone;
	char flags ;
	short nextoffset;
};

struct studiohdr2_t
{
	int numsrcbonetransform;
	int srcbonetransformindex;
	int	illumpositionattachmentindex;
	float flMaxEyeDeflection; // default to cos(30) if not set
	int linearboneindex;
	int sznameindex;
	int m_nBoneFlexDriverCount;
	int m_nBoneFlexDriverIndex;
	int reserved[56];
};

// used for piecewise loading of animation data
struct mstudioanimblock_t
{
	int datastart;
	int dataend;
};

struct mstudiojigglebone_t
{
	int flags;

	// general params
	float length; // how far from bone base, along bone, is tip
	float tipMass;

	// flexible params
	float yawStiffness;
	float yawDamping;
	float pitchStiffness;
	float pitchDamping;
	float alongStiffness;
	float alongDamping;

	// angle constraint
	float angleLimit; // maximum deflection of tip in radians

	// yaw constraint
	float minYaw; // in radians
	float maxYaw; // in radians
	float yawFriction;
	float yawBounce;

	// pitch constraint
	float minPitch; // in radians
	float maxPitch; // in radians
	float pitchFriction;
	float pitchBounce;

	// base spring
	float baseMass;
	float baseStiffness;
	float baseDamping;
	float baseMinLeft;
	float baseMaxLeft;
	float baseLeftFriction;
	float baseMinUp;
	float baseMaxUp;
	float baseUpFriction;
	float baseMinForward;
	float baseMaxForward;
	float baseForwardFriction;
};

struct mstudioattachment_t
{
	int sznameindex;
	int flags;

	int localbone; // parent bone

	Matrix3x4_t localmatrix; // attachment point

	int	unused[8];
};

struct mstudiohitboxset_t
{
	int sznameindex;
	int numhitboxes;
	int hitboxindex;
};

struct mstudioposeparamdesc_t
{
	int					sznameindex;
	int					flags;	// ????
	float				start;	// starting value
	float				end;	// ending value
	float				loop;	// looping range, 0 for no looping, 360 for rotations, etc.
};

struct mstudioiklink_t
{
	int bone;
	Vector3	kneeDir; // ideal bending direction (per link, if applicable)
	Vector3	unused0; // unused in v49
};

struct mstudiotexturedir_t
{
	int sznameindex;
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

	int	posscaleindex; // unused in v53

	int	rotscaleindex;

	int	qalignmentindex;

	int unused[6];
};

struct mstudiosrcbonetransform_t
{
	int sznameindex;

	Matrix3x4_t	pretransform;
	Matrix3x4_t	posttransform;
};

struct mstudioikchain_t
{
	int sznameindex;
	int linktype;
	int numlinks;
	int linkindex;
};

struct mstudiomodelgroup_t
{
	int					szlabelindex;	// textual name
	int					sznameindex;	// file name
};

struct mstudio_modelvertexdata_t
{
	// both void* but studiomdl is 32-bit
	int pVertexData;
	int pTangentData;
};

struct mstudio_meshvertexdata_t
{
	// indirection to this mesh's model's vertex data
	int modelvertexdata;

	// used for fixup calcs when culling top level lods
	// expected number of mesh verts at desired lod
	int numLODVertexes[MAX_NUM_LODS];
};

struct mstudiomesh_t
{
	int material;

	int modelindex;

	int numvertices;		// number of unique vertices/normals/texcoords
	int vertexoffset;		// vertex mstudiovertex_t

	int numflexes;			// vertex animation
	int flexindex;

	// special codes for material operations
	int	materialtype;
	int	materialparam;

	// a unique ordinal for this mesh
	int	meshid;

	Vector3	center;

	mstudio_meshvertexdata_t vertexdata;

	int	unused[8]; // remove as appropriate
};

struct mstudiobbox_t
{
	int					bone;
	int					group;				// intersection group
	Vector3				bbmin;				// bounding box
	Vector3				bbmax;
	int					szhitboxnameindex;	// offset to the name of the hitbox.
	int					unused[8];
};

struct mstudiobone_t
{
	int					sznameindex;

	int		 			parent;		// parent bone
	int					bonecontroller[6];	// bone controller index, -1 == none

	// default values
	Vector3				pos;
	Quaternion			quat;
	RadianEuler			rot;
	// compression scale
	Vector3				posscale;
	Vector3				rotscale;

	Matrix3x4_t			poseToBone;
	Quaternion			qAlignment;
	int					flags;
	int					proctype;
	int					procindex;		// procedural rule
	int			physicsbone;	// index into physically simulated bone

	int					surfacepropidx;	// index into string tablefor property name

	int					contents;		// See BSPFlags.h for the contents flags
	int					surfacepropLookup;	// this index must be cached by the loader, not saved in the file
	int					unused[7];		// remove as appropriate
};

struct studiohdr_t
{
	int					id;
	int					version;
	long				checksum;
	char				name[64];
	int					length;
	Vector3				eyeposition;
	Vector3				illumposition;
	Vector3				hull_min;
	Vector3				hull_max;
	Vector3				view_bbmin;	
	Vector3				view_bbmax;
	int					flags;
	int					numbones;
	int					boneindex;
	int					numbonecontrollers;
	int					bonecontrollerindex;
	int					numhitboxsets;
	int					hitboxsetindex;
	int					numlocalanim;
	int					localanimindex;
	int					numlocalseq;
	int					localseqindex;
	int			activitylistversion;
	int			eventsindexed;
	// raw textures
	int					numtextures;
	int					textureindex;
	// raw textures search paths
	int					numcdtextures;
	int					cdtextureindex;
	// replaceable textures tables
	int					numskinref;
	int					numskinfamilies;
	int					skinindex;
	int					numbodyparts;
	int					bodypartindex;
	// queryable attachable points
	int					numlocalattachments;
	int					localattachmentindex;
	// animation node to animation node transition graph
	int					numlocalnodes;
	int					localnodeindex;
	int					localnodenameindex;
	int					numflexdesc;
	int					flexdescindex;
	int					numflexcontrollers;
	int					flexcontrollerindex;
	int					numflexrules;
	int					flexruleindex;
	int					numikchains;
	int					ikchainindex;
	int					nummouths;
	int					mouthindex;
	int					numlocalposeparameters;
	int					localposeparamindex;
	int					surfacepropindex;
	int					keyvalueindex;
	int					keyvaluesize;
	int					numlocalikautoplaylocks;
	int					localikautoplaylockindex;
	// The collision model mass that jay wanted
	float				mass;
	int					contents;
	// external animations, models, etc.
	int					numincludemodels;
	int					includemodelindex;
	// implementation specific call to get a named model

	// implementation specific back pointer to virtual data
	int /* mutable void* */ virtualModel;

	// for demand loaded animation blocks
	int					szanimblocknameindex;

	int					numanimblocks;
	int					animblockindex;

	int /* mutable void* */ animblockModel;

	int					bonetablebynameindex;

	// used by tools only that don't cache, but persist mdl's peer data
	// engine uses virtualModel to back link to cache pointers
	int /* void* */ pVertexBase;
	int /* void* */ pIndexBase;

	// if STUDIOHDR_FLAGS_CONSTANT_DIRECTIONAL_LIGHT_DOT is set,
	// this value is used to calculate directional components of lighting 
	// on static props
	byte				constdirectionallightdot;

	// set during load of mdl data to track *desired* lod configuration (not actual)
	// the *actual* clamped root lod is found in studiohwdata
	// this is stored here as a global store to ensure the staged loading matches the rendering
	byte				rootLOD;

	// set in the mdl data to specify that lod configuration should only allow first numAllowRootLODs
	// to be set as root LOD:
	//	numAllowedRootLODs = 0	means no restriction, any lod can be set as root lod.
	//	numAllowedRootLODs = N	means that lod0 - lod(N-1) can be set as root lod, but not lodN or lower.
	byte				numAllowedRootLODs;
	//float				defaultFadeDist;
	byte				unused[1];

	int					unused4;

	int					numflexcontrollerui;
	int					flexcontrolleruiindex;

	float				flVertAnimFixedPointScale;
	mutable int			surfacepropLookup;
	int					studiohdr2index;
	int					unused2[1];
};


struct mstudioseqdesc_t
{
	int baseptr;
	int	sznameindex;
	inline char* const pszName() const { return ((char*)this - baseptr + sznameindex); }
	int szactivitynameindex;
	int flags; // looping/non-looping flags
	int activity; // initialized at loadtime to game DLL values
	int actweight;
	int numevents;
	int eventindex;
	Vector3 bbmin; // per sequence bounding box
	Vector3 bbmax;
	int numblends;
	// Index into array of shorts which is groupsize[0] x groupsize[1groupsize[1] in length
	int animindexindex;
	int movementindex; // [blend] float array for blended movement
	int groupsize[2];
	int paramindex[2]; // X, Y, Z, XR, YR, ZR
	float paramstart[2]; // local (0..1) starting value
	float paramend[2]; // local (0..1) ending value
	int paramparent;
	float fadeintime; // ideal cross fate in time (0.2 default)
	float fadeouttime; // ideal cross fade out time (0.2 default)
	int localentrynode; // transition node at entry
	int localexitnode; // transition node at exit
	int nodeflags; // transition rules
	float entryphase; // used to match entry gait
	float exitphase; // used to match exit gait
	float lastframe; // frame that should generation EndOfSequence
	int nextseq; // auto advancing sequences
	int pose; // index of delta animation between end and nextseq
	int numikrules;
	int numautolayers;
	int autolayerindex;
	int weightlistindex;
	int posekeyindex;
	int numiklocks;
	int iklockindex;
	// Key values
	int	keyvalueindex;
	int keyvaluesize;
	int cycleposeindex; // index of pose parameter to use as cycle index
	int activitymodifierindex;
	int numactivitymodifiers;
	int unused[5];
};

struct mstudioevent_t
{
	float				cycle;
	int					event;
	int					type;
	char				options[64];
	int					szeventindex;

};


struct mstudioanimdesc_t
{
	int baseptr;

	int sznameindex;

	float fps; // frames per second	
	int flags; // looping/non-looping flags

	int numframes;

	// piecewise movement
	int	nummovements;
	int movementindex;

	int ikrulezeroframeindex;

	int unused1[5]; // remove as appropriate (and zero if loading older versions)	

	int animblock;
	int animindex; // non-zero when anim data isn't in sections

	int numikrules;
	int ikruleindex; // non-zero when IK data is stored in the mdl
	int animblockikruleindex; // non-zero when IK data is stored in animblock file

	int numlocalhierarchy;
	int localhierarchyindex;;

	int sectionindex;
	int sectionframes; // number of frames used in each fast lookup section, zero if not used

	short zeroframespan; // frames per span
	short zeroframecount; // number of spans
	int zeroframeindex;

	float zeroframestalltime; // saved during read stalls
};

// Apex Legends, version '54'
namespace r5{
	union mstudioanimvalue_t{
		struct{
			char	valid;
			char	total;
		} num;
		short		value;
	};

	#define STUDIO_ANIMPTR_Z 0x01
	#define STUDIO_ANIMPTR_Y 0x02
	#define STUDIO_ANIMPTR_X 0x04

	struct mstudioanim_valueptr_t
	{
		short offset : 13;
		short flags : 3;
		unsigned char axisIdx1;
		unsigned char axisIdx2;
	};

	struct mstudio_rle_anim_t
	{
		short size : 13; // total size of all animation data, not next offset because even the last one has it
		short flags : 3;
	};

	struct unkseqdata_t{
		float unkfloat; // generally 0-1
		int unk;
		float unkfloat1;
		float unkfloat2;
		float unkfloat3;
		float unkfloat4;
	};

	struct boneflagarray {
		char flag;
	};
	namespace v8
	{
		struct mstudiobone_t
		{
			int sznameindex;
			inline char* const pszName() const { return ((char*)this + sznameindex); }
			int parent; // parent bone
			int bonecontroller[6]; // bone controller index, -1 == none
			Vector3 pos; // base bone position
			Quaternion quat;
			RadianEuler rot; // base bone rotation
			Vector3 scale; // base bone scale
			Matrix3x4_t poseToBone;
			Quaternion qAlignment;
			int flags;
			int proctype;
			int procindex; // procedural rule offset
			int physicsbone; // index into physically simulated bone
			// from what I can tell this is the section that is parented to this bone, and if this bone is not the parent of any sections, it goes up the bone chain to the nearest bone that does and uses that section index
			int surfacepropidx; // index into string tablefor property name
			inline char* const pszSurfaceProp() const { return ((char*)this + surfacepropidx); }
			int contents; // See BSPFlags.h for the contents flags
			int surfacepropLookup; // this index must be cached by the loader, not saved in the file
			int unk;
			int unkid; // physics index (?)
		};

		// apex changed this a bit, 'is_rigid' cut
		struct mstudiojigglebone_t
		{
			char flags;
			unsigned char bone; // id of bone, might be single byte
			short pad; // possibly unused, possibly struct packing
			// general params
			float length; // how far from bone base, along bone, is tip
			float tipMass;
			float tipFriction; // friction applied to tip velocity, 0-1
			// flexible params
			float yawStiffness;
			float yawDamping;
			float pitchStiffness;
			float pitchDamping;
			float alongStiffness;
			float alongDamping;
			// angle constraint
			float angleLimit; // maximum deflection of tip in radians
			// yaw constraint
			float minYaw; // in radians
			float maxYaw; // in radians
			float yawFriction;
			float yawBounce;
			// pitch constraint
			float minPitch; // in radians
			float maxPitch; // in radians
			float pitchFriction;
			float pitchBounce;
			// base spring
			float baseMass;
			float baseStiffness;
			float baseDamping;
			float baseMinLeft;
			float baseMaxLeft;
			float baseLeftFriction;
			float baseMinUp;
			float baseMaxUp;
			float baseUpFriction;
			float baseMinForward;
			float baseMaxForward;
			float baseForwardFriction;
		};

		// this struct is the same in r1 and r2
		struct mstudiolinearbone_t
		{
			int numbones;

			int flagsindex;
			inline int* pFlags(int i) const {return reinterpret_cast<int*>((char*)this + flagsindex) + i; }
			inline int flags(int i) const { return *pFlags(i); }

			int	parentindex;
			inline int* pParent(int i)
				const {
				return reinterpret_cast<int*>((char*)this + parentindex) + i;
			}

			int	posindex;
			inline const Vector3* pPos(int i)
				const {
				return reinterpret_cast<Vector3*>((char*)this + posindex) + i;
			}

			int quatindex;
			inline const Quaternion* pQuat(int i)
				const {
				return reinterpret_cast<Quaternion*>((char*)this + quatindex) + i;
			}

			int rotindex;
			inline const RadianEuler* pRot(int i)
				const {
				return reinterpret_cast<RadianEuler*>((char*)this + rotindex) + i;
			}

			int posetoboneindex;
			inline const Matrix3x4_t* pPoseToBone(int i)
				const {
				return reinterpret_cast<Matrix3x4_t*>((char*)this + posetoboneindex) + i;
			}
		};

		// this struct is unchanged from p2
		struct mstudiosrcbonetransform_t
		{
			int			sznameindex;
			Matrix3x4_t	pretransform;
			Matrix3x4_t	posttransform;
		};

		struct mstudioattachment_t
		{
			int sznameindex;
			int flags;
			int localbone; // parent bone
			Matrix3x4_t localmatrix; // attachment point
		};

		// this struct is the same in r1, r2, and early r5, and unchanged from p2
		struct mstudiohitboxset_t
		{
			int sznameindex;
			int numhitboxes;
			int hitboxindex;
		};

		struct mstudiobbox_t
		{
			int bone;
			int group; // intersection group
			Vector3 bbmin; // bounding box
			Vector3 bbmax;
			int szhitboxnameindex; // offset to the name of the hitbox.
			int critShotOverride;	// value of one causes this to act as if it was group of 'head', may either be crit override or group override. mayhaps aligned boolean, look into this
			int hitdataGroupOffset;	// hit_data group in keyvalues this hitbox uses.
		};

		// rle stuff here

		struct mstudioanimsections_t{
			int					animindex;
		};

		struct mstudiomovement_t {
			int					endframe;
			int					motionflags;
			float				v0;			// velocity at start of block
			float				v1;			// velocity at end of block
			float				angle;		// YAW rotation at end of this blocks movement
			Vector3				movement;   // movement Vector3 relative to this blocks initial angle
			Vector3				position;	// relative to start of animation???
		};

		// new in Titanfall 1
		// translation track for origin bone, used in lots of animated scenes, requires STUDIO_FRAMEMOVEMENT
		// pos_x, pos_y, pos_z, yaw
		struct mstudioframemovement_t
		{
			float scale[4];
			int sectionFrames;
			//inline mstudioanimvalue_t* pAnimvalue(int i) const { return (offset[i] > 0) ? reinterpret_cast<mstudioanimvalue_t*>((char*)this + offset[i]) : nullptr; }

			int* pSectionOffsets(int i)
			{
				return reinterpret_cast<int*>((char*)this + sizeof(mstudioframemovement_t)) + i;
			}
		};

		struct mstudiocompressedikerror_t
		{
			float scale[6]; // these values are the same as what posscale (if it was used) and rotscale are.
			int sectionFrames; // frames per section, may not match animdesc
		};

		struct mstudioikrule_t
		{
			int index;
			int type;
			int chain;
			int bone; // gets it from ikchain now pretty sure

			int slot; // iktarget slot. Usually same as chain.
			float height;
			float radius;
			float floor;
			Vector3 pos;
			Quaternion q;

			// apex does this oddly
			mstudiocompressedikerror_t compressedIkError;
			int compressedikerrorindex;

			int iStart;
			int ikerrorindex;

			float start; // beginning of influence
			float peak; // start of full influence
			float tail; // end of full influence
			float end; // end of all influence

			float contact; // frame footstep makes ground concact
			float drop; // how far down the foot should drop when reaching for IK
			float top; // top of the foot box

			int szattachmentindex; // name of world attachment

			float endHeight; // new in v52   
		};

		struct mstudioanimdesc_t
		{
			int baseptr;
			int sznameindex;
			float fps; // frames per second	
			int flags; // looping/non-looping flags
			int numframes;
			// piecewise movement
			int nummovements;
			int movementindex;
			inline mstudiomovement_t* const pMovement(int i) const { return reinterpret_cast<mstudiomovement_t*>((char*)this + movementindex) + i; };
			int framemovementindex; // new in v52
			inline const mstudioframemovement_t* pFrameMovement() const { return reinterpret_cast<mstudioframemovement_t*>((char*)this + framemovementindex); }
			int animindex; // non-zero when anim data isn't in sections
			mstudio_rle_anim_t* pAnim(int* piFrame) const; // returns pointer to data and new frame index
			int numikrules;
			int ikruleindex; // non-zero when IK data is stored in the mdl
			int sectionindex;
			int sectionframes; // number of frames used in each fast lookup section, zero if not used
			inline const mstudioanimsections_t* pSection(int i) const { return reinterpret_cast<mstudioanimsections_t*>((char*)this + sectionindex) + i; }
		};

		struct mstudioevent_t
		{
			float				cycle;
			int					event;
			int					type;
			char				options[256];

			int					szeventindex;
		};

		struct mstudioautolayer_t
		{
			unsigned __int64	assetSequence; // hashed aseq guid asset, this needs to have a guid descriptor in rpak
			short				iSequence;
			short				iPose;

			int					flags;
			float				start;	// beginning of influence
			float				peak;	// start of full influence
			float				tail;	// end of full influence
			float				end;	// end of all influence
		};

		struct mstudioactivitymodifier_t
		{
			int sznameindex;
			bool negate; // negate all other activity modifiers when this one is active?
		};

		struct mstudioseqdesc_t
		{
			int baseptr;

			int	szlabelindex;

			int szactivitynameindex;

			int flags; // looping/non-looping flags

			int activity; // initialized at loadtime to game DLL values
			int actweight;

			int numevents;
			int eventindex;

			Vector3 bbmin; // per sequence bounding box
			Vector3 bbmax;

			int numblends;

			// Index into array of shorts which is groupsize[0] x groupsize[1] in length
			int animindexindex;

			int movementindex; // [blend] float array for blended movement
			int groupsize[2];
			int paramindex[2]; // X, Y, Z, XR, YR, ZR
			float paramstart[2]; // local (0..1) starting value
			float paramend[2]; // local (0..1) ending value
			int paramparent;

			float fadeintime; // ideal cross fate in time (0.2 default)
			float fadeouttime; // ideal cross fade out time (0.2 default)

			int localentrynode; // transition node at entry
			int localexitnode; // transition node at exit
			int nodeflags; // transition rules

			float entryphase; // used to match entry gait
			float exitphase; // used to match exit gait

			float lastframe; // frame that should generation EndOfSequence

			int nextseq; // auto advancing sequences
			int pose; // index of delta animation between end and nextseq

			int numikrules;

			int numautolayers;
			int autolayerindex;

			int weightlistindex;

			int posekeyindex;

			int numiklocks;
			int iklockindex;

			// Key values
			int keyvalueindex;
			int keyvaluesize;

			int cycleposeindex; // index of pose parameter to use as cycle index

			int activitymodifierindex;
			int numactivitymodifiers;

			int ikResetMask;	// new in v52
			// titan_buddy_mp_core.mdl
			// reset all ikrules with this type???
			int unk1;	// count? STUDIO_ANIMDESC_52_UNK??
			// mayhaps this is the ik type applied to the mask if what's above it true

			int unkOffset;
			int unkCount;
		};

		// pack here for our silly little unknown pointer
#pragma pack(push, 4)
		struct mstudiomesh_t
		{
			int material;

			int modelindex;

			int numvertices;	// number of unique vertices/normals/texcoords
			int vertexoffset;	// vertex mstudiovertex_t
			// offset by vertexoffset number of verts into vvd vertexes, relative to the models offset

// Access thin/fat mesh vertex data (only one will return a non-NULL result)

			int deprecated_numflexes; // vertex animation
			int deprecated_flexindex;

			// special codes for material operations
			int deprecated_materialtype;
			int deprecated_materialparam;

			// a unique ordinal for this mesh
			int meshid;

			Vector3 center;

			mstudio_meshvertexdata_t vertexloddata;

			void* pUnknown; // unknown memory pointer, probably one of the older vertex pointers but moved
		};
#pragma pack(pop)

		struct mstudiomodel_t
		{
			char name[64];

			int unkStringOffset; // goes to bones sometimes

			int type;

			float boundingradius;

			int nummeshes;
			int meshindex;

			mstudiomesh_t* pMesh(int i)
			{
				return reinterpret_cast<mstudiomesh_t*>((char*)this + meshindex) + i;
			}

			// cache purposes
			int numvertices;	// number of unique vertices/normals/texcoords
			int vertexindex;	// vertex Vector3
			// offset by vertexindex number of chars into vvd verts
			int tangentsindex;	// tangents Vector3
			// offset by tangentsindex number of chars into vvd tangents

			int numattachments;
			int attachmentindex;

			int deprecated_numeyeballs;
			int deprecated_eyeballindex;

			int pad[4];

			int colorindex; // vertex color
			// offset by colorindex number of chars into vvc vertex colors
			int uv2index;	// vertex second uv map
			// offset by uv2index number of chars into vvc secondary uv map
		};

		struct mstudiobodyparts_t
		{
			int					sznameindex;
			inline char* const pszName() const { return ((char*)this + sznameindex); }

			int					nummodels;
			int					base;
			int					modelindex; // index into models array

			mstudiomodel_t* pModel(int i)
			{
				return reinterpret_cast<mstudiomodel_t*>((char*)this + modelindex) + i;
			};
		};

		struct mstudioiklink_t
		{
			int		bone;
			Vector3	kneeDir;	// ideal bending direction (per link, if applicable)
			// doesn't seem to be kneeDir, however, why would they keep the unused Vector3?
		};

		struct mstudioikchain_t
		{
			int				sznameindex;
			inline char* const pszName() const { return ((char*)this + sznameindex); }

			int				linktype;
			int				numlinks;
			int				linkindex;

			mstudioiklink_t* pLink(int i)
			{
				return reinterpret_cast<mstudioiklink_t*>((char*)this + linkindex) + i;
			}

			int	unk;		// no clue what this does tbh, tweaking it does nothing
			// default value: 0.707f
			// this value is similar to default source engine ikchain values
		};

		struct mstudioposeparamdesc_t
		{
			int					sznameindex;
			inline char* const pszName() const { return ((char*)this + sznameindex); }

			int					flags;	// ????
			float				start;	// starting value
			float				end;	// ending value
			float				loop;	// looping range, 0 for no looping, 360 for rotations, etc.
		};

		struct mstudiomodelgroup_t
		{
			int					szlabelindex;	// textual name
			inline char* const pszLabel() const { return ((char*)this + szlabelindex); }

			int					sznameindex;	// file name
			inline char* const pszName() const { return ((char*)this + sznameindex); }
		};

#pragma pack(push,4) // this caused issues
		struct mstudiotexture_t
		{
			int sznameindex;
			inline char* const pszName() const { return ((char*)this + sznameindex); }

			unsigned __int64 textureGuid; // guid/hash of this material


		};
#pragma pack(pop)

		struct mstudiocollmodel_t
		{
			int contentMasksIndex;
			int surfacePropsIndex;
			int surfaceNamesIndex;
			int headerCount;
		};

		struct mstudiocollheader_t
		{
			int unk;
			int bvhNodeIndex;
			int vertIndex;
			int bvhLeafIndex;
			float unk_10[4];
		};

		struct studiohdr_t
		{
			int id; // Model format ID, such as "IDST" (0x49 0x44 0x53 0x54)
			int version; // Format version number, such as 54 (0x36,0x00,0x00,0x00)
			int checksum; // This has to be the same in the phy and vtx files to load!
			int sznameindex; // This has been moved from studiohdr2 to the front of the main header.
			char name[64]; // The internal name of the model, padding with null chars.
			// Typically "my_model.mdl" will have an internal name of "my_model"
			int length; // Data size of MDL file in chars.
			Vector3 eyeposition;	// ideal eye position
			Vector3 illumposition;	// illumination center
			Vector3 hull_min;		// ideal movement hull size
			Vector3 hull_max;
			Vector3 view_bbmin;		// clipping bounding box
			Vector3 view_bbmax;
			int flags;
			int numbones; // bones
			int boneindex;
			int numbonecontrollers; // bone controllers
			int bonecontrollerindex;
			int numhitboxsets;
			int hitboxsetindex;
			// seemingly unused now, as animations are per sequence
			int numlocalanim; // animations/poses
			int localanimindex; // animation descriptions
			int numlocalseq; // sequences
			int	localseqindex;
			int activitylistversion; // initialization flag - have the sequences been indexed?
			// mstudiotexture_t
			// short rpak path
			// raw textures
			int materialtypesindex; // index into an array of char sized material type enums for each material used by the model
			int numtextures; // the material limit exceeds 128, probably 256.
			int textureindex;
			// this should always only be one, unless using vmts.
			// raw textures search paths
			int numcdtextures;
			int cdtextureindex;
			// replaceable textures tables
			int numskinref;
			int numskinfamilies;
			int skinindex;
			int numbodyparts;
			int bodypartindex;
			inline mstudiobodyparts_t* const pBodypart(int i) const {  return reinterpret_cast<mstudiobodyparts_t*>((char*)this + bodypartindex) + i; }
			int numlocalattachments;
			int localattachmentindex;
			int numlocalnodes;
			int localnodeindex;
			int localnodenameindex;
			int unkNodeCount; // ???
			int nodeDataOffsetsOffset; // index into an array of int sized offsets that read into the data for each node
			int meshOffset; // hard offset to the start of this models meshes
			// all flex related model vars and structs are stripped in respawn source
			int deprecated_numflexcontrollers;
			int deprecated_flexcontrollerindex;
			int deprecated_numflexrules;
			int deprecated_flexruleindex;
			int numikchains;
			int ikchainindex;
			// mesh panels for using rui on models, primarily for weapons
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
			// unused for packed models
			// technically still functional though I am unsure why you'd want to use it
			int numincludemodels;
			int includemodelindex;
			int /* mutable void* */ virtualModel;
			int bonetablebynameindex;
			// if STUDIOHDR_FLAGS_CONSTANT_DIRECTIONAL_LIGHT_DOT is set,
			// this value is used to calculate directional components of lighting 
			// on static props
			char constdirectionallightdot;
			// set during load of mdl data to track *desired* lod configuration (not actual)
			// the *actual* clamped root lod is found in studiohwdata
			// this is stored here as a global store to ensure the staged loading matches the rendering
			char rootLOD;
			// set in the mdl data to specify that lod configuration should only allow first numAllowRootLODs
			// to be set as root LOD:
			//	numAllowedRootLODs = 0	means no restriction, any lod can be set as root lod.
			//	numAllowedRootLODs = N	means that lod0 - lod(N-1) can be set as root lod, but not lodN or lower.
			char numAllowedRootLODs;
			char unused;
			float defaultFadeDist;	// set to -1 to never fade. set above 0 if you want it to fade out, distance is in feet.
			// player/titan models seem to inherit this value from the first model loaded in menus.
			// works oddly on entities, probably only meant for static props
			float gatherSize; // what. from r5r struct. no clue what this does, seemingly for early versions of apex bsp but stripped in release apex (season 0)
			// bad name, frustum culling
			int deprecated_numflexcontrollerui;
			int deprecated_flexcontrolleruiindex;
			float flVertAnimFixedPointScale; // to be verified
			int surfacepropLookup; // saved in the file
			// this is in most shipped models, probably part of their asset bakery.
			// doesn't actually need to be written pretty sure, only four chars when not present.
			// this is not completely true as some models simply have nothing, such as animation models.
			int sourceFilenameOffset;
			int numsrcbonetransform;
			int srcbonetransformindex;
			int	illumpositionattachmentindex;
			int linearboneindex;
			// unsure what this is for but it exists for jigglbones
			int procBoneCount;
			int procBoneTableOffset;
			int linearProcBoneOffset;
			// depreciated as they are removed in 12.1
			int deprecated_m_nBoneFlexDriverCount;
			int deprecated_m_nBoneFlexDriverIndex;
			int deprecated_m_nPerTriAABBIndex;
			int deprecated_m_nPerTriAABBNodeCount;
			int deprecated_m_nPerTriAABBLeafCount;
			int deprecated_m_nPerTriAABBVertCount;
			// always "" or "Titan"
			int unkStringOffset;
			// this is now used for combined files in rpak, vtx, vvd, and vvc are all combined while vphy is separate.
			// the indexes are added to the offset in the rpak mdl_ header.
			// vphy isn't vphy, looks like a heavily modified vphy.
			// as of s2/3 these are no unused except phy
			int vtxOffset; // VTX
			int vvdOffset; // VVD / IDSV
			int vvcOffset; // VVC / IDCV 
			int phyOffset; // VPHY / IVPS
			int vtxSize;
			int vvdSize;
			int vvcSize;
			int phySize; // still used in models using vg
			// unused in apex, gets cut in 12.1
			int deprecated_unkOffset; // deprecated_imposterIndex
			int deprecated_unkCount; // deprecated_numImposters
			// mostly seen on '_animated' suffixed models
			// manually declared bone followers are no longer stored in kvs under 'bone_followers', they are now stored in an array of ints with the bone index.
			int boneFollowerCount;
			int boneFollowerOffset;
			// BVH4 size (?)
			Vector3 mins;
			Vector3 maxs; // seem to be the same as hull size
			int unk3_v54[3];
			int bvhOffset; // bvh4 tree
			short unk4_v54[2]; // same as unk3_v54_v121, 2nd might be base for other offsets? these are related to bvh4 stuff. collision detail for bvh (?)
			// new in apex vertex weight file for verts that have more than three weights
			// vvw is a 'fake' extension name, we do not know the proper name.
			int vvwOffset; // index will come last after other vertex files
			int vvwSize;
		};
	}
}

struct stringentry_t
{
	char* base;
	char* addr;
	int* ptr;
	const char* string;
	int dupindex;
}; 

struct s_modeldata_t
{
	//r5::v8::studiohdr_t* pHdr;
	void* pHdr;
	inline r5::v8::studiohdr_t* hdrV54() { return reinterpret_cast<r5::v8::studiohdr_t*>(pHdr); }
	inline r5::v8::mstudioseqdesc_t* seqV7() { return reinterpret_cast<r5::v8::mstudioseqdesc_t*>(pHdr); }

	std::vector<stringentry_t> stringTable;
	char* pBase;
	char* pData;
};

inline s_modeldata_t g_model;
