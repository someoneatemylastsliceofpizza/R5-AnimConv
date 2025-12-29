#pragma once
#include <cstdint> 

#define STRING_FROM_IDX(base, idx) reinterpret_cast<const char*>((char*)base + idx)
#define PTR_FROM_IDX(type, base, idx) reinterpret_cast<type*>((char*)base + idx)
#define OFFSET(x) static_cast<uint32_t>((x & 0xFFFE) << (4 * (x & 1)))
#define PRINTANDTHROW(file, msg) {printf("[!] Error at %s\n", file.c_str());printf("--> %s\n", msg);throw std::runtime_error(msg);}

#define ALIGN2( a ) a = (char *)((__int64)((char *)a + 1) & ~ 1)
#define ALIGN4( a ) a = (char *)((__int64)((char *)a + 3) & ~ 3)
#define ALIGN16( a ) a = (char *)((__int64)((char *)a + 15) & ~ 15)

constexpr auto M_PI = 3.14159265358979323846;


struct Vector3 {
	float x, y, z;

	inline Vector3 operator + (Vector3& a) { return { x + a.x, y + a.y, z + a.z }; }
	//inline Vector3 operator - (Vector3& a) {	return { x - a.x, y - a.y, z - a.z };}
	inline Vector3 operator + (Vector3 a) { return { x + a.x, y + a.y, z + a.z }; }
	inline Vector3 operator - (Vector3 a) { return { x - a.x, y - a.y, z - a.z }; }
	inline Vector3 operator + (int& a) { return { x + a, y + a, z + a }; }
	inline Vector3 operator + (int a) { return { x + a, y + a, z + a }; }
	inline Vector3 operator * (int& a) { return { x * a, y * a, z * a }; }
	inline Vector3 operator * (int a) { return { x * a, y * a, z * a }; }
	inline Vector3 operator*(Vector3& a) { return { x * a.x, y * a.y, z * a.z }; }
	inline bool operator==(Vector3 a) { return (x == a.x && y == a.y && z == a.z); }
	inline bool operator!=(Vector3 a) { return (x != a.x || y != a.y || z != a.z); }

	inline void operator += (Vector3 a) {
		x += a.x;
		y += a.y;
		z += a.z;
	}

	inline void operator += (float a) {
		x += a;
		y += a;
		z += a;
	}

	inline void operator -= (Vector3 a) {
		x -= a.x;
		y -= a.y;
		z -= a.z;
	}

	inline void operator -= (float a) {
		x -= a;
		y -= a;
		z -= a;
	}

	inline void operator*=(float a) {
		x *= a;
		y *= a;
		z *= a;
	}

	inline float& operator[](int i) { return ((float*)this)[i]; }
	inline float operator[](int i) const { return ((float*)this)[i]; }

	inline float Min() const { return std::min({ x, y, z }); }
	inline float Max() const { return std::max({ x, y, z }); }
	bool approx_equal(const Vector3 other, float eps = 1e-6f) const {
		return std::fabs(x - other.x) < eps &&
			std::fabs(y - other.y) < eps &&
			std::fabs(z - other.z) < eps;
	}
};

struct Vector4 {
	float x, y, z, w;

	inline float& operator[](int i) { return ((float*)this)[i]; }
	inline float operator[](int i) const { return ((float*)this)[i]; }
};

struct Vector64
{
	uint64_t x : 21;
	uint64_t y : 21;
	uint64_t z : 22;
};

struct Vector48
{
	int16_t x;
	int16_t y;
	int16_t z;
};

struct Matrix3x4_t
{
	float m[3][4];

	float* operator[](int i) {
		return m[i];
	}
	const float* operator[](int i) const {
		return m[i];
	}
};

//struct RadianEuler {
//	float x, y, z;
//};

struct Quaternion {
	float x, y, z, w;

	inline Vector3 ToRad() {
		double norm = std::sqrt(w * w + x * x + y * y + z * z);
		double qw = w / norm;
		double qx = x / norm;
		double qy = y / norm;
		double qz = z / norm;

		Vector3 euler;

		double sinr_cosp = 2.0 * (qw * qx + qy * qz);
		double cosr_cosp = 1.0 - 2.0 * (qx * qx + qy * qy);
		euler.x = (float)std::atan2(sinr_cosp, cosr_cosp);

		double sinp = 2.0 * (qw * qy - qz * qx);
		if (std::abs(sinp) >= 1.0)
			euler.y = (float)std::copysign(M_PI / 2.0, sinp);
		else
			euler.y = (float)std::asin(sinp);

		double siny_cosp = 2.0 * (qw * qz + qx * qy);
		double cosy_cosp = 1.0 - 2.0 * (qy * qy + qz * qz);
		euler.z = (float)std::atan2(siny_cosp, cosy_cosp);
		return euler;
	}
};

struct Quaternion64 {
	uint64_t x : 21;
	uint64_t y : 21;
	uint64_t z : 21;
	uint64_t wneg : 1;

	inline float decode(uint64_t v1) const {
		int32_t v2 = static_cast<int32_t>(v1) - 1048576;
		return static_cast<float>(v2) / 1048576.5f;
	}
	inline static uint64_t encode(float v1) {
		int32_t v2 = static_cast<int32_t>(roundf(v1 * 1048576.5f));
		return static_cast<uint64_t>(v2 + 1048576);
	}

	inline Quaternion64& operator=(const Quaternion64& other) {
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;
		this->wneg = other.wneg;
		return *this;
	}
};

inline Quaternion UnpackQuat64(Quaternion64 q64) {
	Quaternion q;
	q.x = q64.decode(q64.x);
	q.y = q64.decode(q64.y);
	q.z = q64.decode(q64.z);
	float w_sq = 1.0f - (q.x * q.x + q.y * q.y + q.z * q.z);
	q.w = sqrt(w_sq);

	if (q64.wneg)
		q.w = -q.w;

	return q;
}

inline Quaternion64 PackQuat64(Quaternion q) {
	Quaternion64 q64;
	q64.x = Quaternion64::encode(q.x);
	q64.y = Quaternion64::encode(q.y);
	q64.z = Quaternion64::encode(q.z);
	q64.wneg = (q.w < 0.0f) ? 1 : 0;

	return q64;
}

namespace temp {
	struct stringentry_t {
		char* base;
		char* addr;
		int* ptr;
		std::string string;
		int dupindex;
	};

	class StringTable {
	public:
		std::vector<stringentry_t> stringTable;

		void Init() {
			stringTable.clear();
			stringTable.emplace_back(stringentry_t{ NULL, NULL, NULL, "", -1 });
		}

		template<typename T>
		void Add(T* base, int* ptr, std::string str) {
			if (str.empty()) str = "";
			stringentry_t newString{};

			int i = 0;
			for (auto& it : stringTable) {
				if ((str == it.string)) {
					newString.base = (char*)base;
					newString.ptr = ptr;
					newString.string = str;
					newString.dupindex = i;
					stringTable.emplace_back(newString);
					return;
				}
				i++;
			}

			newString.base = (char*)base;
			newString.ptr = ptr;
			newString.string = str;
			newString.dupindex = -1;

			stringTable.emplace_back(newString);
		}

		template<typename T>
		void Add(T* base, int* ptr, const char* str) {
			if (!str) str = "";
			stringentry_t newString{};

			int i = 0;
			for (auto& it : stringTable) {
				if (!strcmp(str, it.string.c_str())) {
					newString.base = (char*)base;
					newString.ptr = ptr;
					newString.string = str;
					newString.dupindex = i;
					stringTable.emplace_back(newString);
					return;
				}
				i++;
			}

			newString.base = (char*)base;
			newString.ptr = ptr;
			newString.string = str;
			newString.dupindex = -1;

			stringTable.emplace_back(newString);
		}

		char* Write(char* pData) {
			for (auto& it : stringTable) {
				if (it.dupindex == -1) {
					it.addr = pData;
					if (it.ptr) {
						*it.ptr = int(pData - it.base);
						size_t length = it.string.length();
						strcpy_s(pData, length + 1, it.string.c_str());

						pData += length;
					}
					*pData = '\0';
					pData++;
				}
				else {
					*it.ptr = int(stringTable[it.dupindex].addr - it.base);
				}
			}
			return pData;
		}
	};

	struct animdata_t {
		std::vector<Vector3> pos;
		std::vector<Vector3> rot;
		std::vector<Vector3> scl;
		void resize(size_t size) {
			pos.resize(size);
			rot.resize(size);
			scl.resize(size);
		}
	};

	struct ikrule_t {
		int index = 0;
		int type = 4;
		int chain = 0;
		int bone = 0;
		int slot = 0;
		float height = 50.f;
		float radius = 15.f;
		float floor = 0.f;
		Vector3 pos{};
		Quaternion q{};
		float scale[6] = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f};
		uint32_t sectionframes = 0;
		int iStart = 0;
		float start = 0.f;
		float peak = 0.f;
		float tail = 1.f;
		float end = 1.f;
		float contact = 0.f;
		float drop = 0.f;
		float top = 0.f;
		std::string attachmentname = "";
		float endHeight = -100.f;
		temp::animdata_t ikruledata{};
	};

	struct framemovement_t {
		Vector4 scale{};
		int sectionframes = 0;
		std::vector<Vector4> movementdata{};
	};

	struct rig_t;
	struct animdesc_t {
		std::string name = "";
		float fps = 0;
		int32_t flags = 0;
		int32_t numframes = 0;
		std::vector <animdata_t> animdata;
		std::vector<temp::ikrule_t> ikrules{};
		framemovement_t movement{};
		int32_t sectionstallframes = 0;
		int32_t sectionframes = 0;

		void InitData(int32_t numbones, const temp::rig_t& rig, bool badditive);
		void SubtractBase(int32_t numbones, const temp::rig_t& rig, bool badditive);
		bool IsAdditive() const { return flags & 0x4; }
	};

	struct seqevent_t {
		std::string name;
		float cycle;
		int32_t event;
		int32_t type;
		std::string options;

	};

	struct autolayer_t {
		uint64_t guidSequence;
		int16_t iSequence;
		int16_t iPose;
		int flags;
		float start;
		float peak;
		float tail;
		float end;
	};

	struct actmod_t {
		std::string name;
		bool negate;
	};

	class Sequence {
	public:
		std::string path;
		std::string name;
		int32_t numbones = 0;
		uint32_t flags = 0u;

		std::string activityname;
		int32_t activity = 0u;
		int32_t actweight = 0u;

		std::vector<float> posekeys;
		std::vector<temp::seqevent_t> events;
		std::vector<temp::autolayer_t> autolayers;
		std::vector<temp::actmod_t> actmods;
		Vector3 bbmin{};
		Vector3 bbmax{};
		int32_t groupsize[2] = { 0, 0 };
		int32_t paramindex[2] = { 0, 0 };
		float paramstart[2] = { 0, 0 };
		float paramend[2] = { 0, 0 };
		int32_t paramparent = 0;
		float fadeintime = 0.f;
		float fadeouttime = 0.f;
		int32_t localentrynode = 0;
		int32_t localexitnode = 0;
		int32_t nodeflags = 0;
		float entryphase = 0.f;
		float exitphase = 0.f;
		float lastframe = 0.f;
		int32_t nextseq = 0;
		int32_t pose = 0;
		// iklocks
		// keyvalue
		int32_t ikResetMask = 0;
		int32_t unk1 = 0;
		// weightfixup

		std::vector<float> weightlist;
		int32_t numuniqueblends = 0u;
		std::vector<uint32_t> blends;
		std::vector<temp::animdesc_t> anims;

		Sequence(const std::string name, const int32_t numbones) : name(name), numbones(numbones) {
			weightlist.resize(numbones);
		}

		inline bool IsAdditive() const { return flags & 0x4; }
	};

	struct rigdesc_t {
		Vector3 eyeposition{};
		Vector3 illumposition{};
		Vector3 hull_min{};
		Vector3 hull_max{};
		Vector3 view_bbmin{};
		Vector3 view_bbmax{};
		int flags;
		int numbones;
		int activitylistversion;
		float mass;
		int contents;
		float defaultFadeDist;
		float gathersize;
		std::string surfaceprop;
		Vector3 mins{};
		Vector3 maxs{};
	};

	struct bone_t {
		std::string name;
		int32_t parent = -1;
		uint32_t flags = 0u;
		int32_t bonecontroller[6] = { -1, -1, -1, -1, -1, -1 };
		int32_t proctype = 0;
		int32_t procindex = 0;
		int32_t physicsbone = 0;
		std::string surfaceprop;
		int32_t contents = 0;
		int32_t surfacepropLookup = 0;

		Vector3 pos{};
		Quaternion q{};
		Vector3 rot{};
		Vector3 scl{};
		Matrix3x4_t poseToBone{};
		Quaternion qAlignment{};
	};

	struct bbox_t {
		std::string name{};
		int bone;
		int group;
		Vector3 bbmin{};
		Vector3 bbmax{};
		int critShotOverride;
		std::string hitdataGroupOffset;
	};

	struct hitboxsets_t {
		std::string name;
		std::vector<temp::bbox_t> hitboxes;
	};

	struct nodedata_t {
		uint16_t tonode = 0;
		uint16_t seq = 0;
	};

	struct node_t {
		std::string name;
		std::vector<temp::nodedata_t> nodedatas;
	};

	struct poseparam_t {
		std::string name;
		int flags;
		float start;
		float end;
		float loop;
	};

	struct iklink_t {
		int bone;
		Vector3	kneeDir;
	};

	struct ikchain_t {
		std::string name;
		int linktype;
		std::vector<temp::iklink_t> iklinks;
		float unk;
	};


	struct rig_t {
		std::string rrigpath;
		std::string rsonpath;
		std::vector<std::string> rseqpaths;
		std::vector<std::string> rigpaths;
		//std::vector<std::string> materialpaths;

		temp::rigdesc_t hdr{};
		std::string name = "CANNOT LOAD RRIG NAME";
		std::vector<temp::bone_t> bones;
		std::vector<temp::hitboxsets_t> hitboxsets;
		std::vector<uint8_t> bonebyname;
		uint16_t ignorenode = 0;
		std::vector<temp::node_t> nodes;
		std::vector<temp::poseparam_t> poseparams;
		std::vector<temp::ikchain_t> ikchains;
		std::vector<temp::Sequence> sequences;
	};

	struct animblock_t {
		uint8_t comptype;
		uint32_t startframe;
		uint32_t endframe;
	};

}

inline void temp::animdesc_t::InitData(int32_t numbones, const temp::rig_t& rig, bool badditive) {
	const Vector3 zero(0, 0, 0);
	const Vector3 one(1, 1, 1);
	animdata.resize(numbones);

	for (int i = 0; i < numbones; ++i) {
		auto& ad = animdata[i];
		ad.resize(numframes);

		if (badditive) {
			std::fill_n(ad.pos.begin(), numframes, zero);
			std::fill_n(ad.rot.begin(), numframes, zero);
			std::fill_n(ad.scl.begin(), numframes, one);
		}
		else {
			std::fill_n(ad.pos.begin(), numframes, rig.bones[i].pos);
			std::fill_n(ad.rot.begin(), numframes, rig.bones[i].rot);
			std::fill_n(ad.scl.begin(), numframes, rig.bones[i].scl);
		}
	}
}

inline void temp::animdesc_t::SubtractBase(int32_t numbones, const temp::rig_t& rig, bool badditive) {
	for (int i = 0; i < numbones; ++i) {
		auto& ad = animdata[i];

		const Vector3& basePos = rig.bones[i].pos;
		const Vector3& baseRot = rig.bones[i].rot;
		const Vector3& baseScl = rig.bones[i].scl;
		if (!badditive) {
			for (auto& v : ad.pos) v -= basePos;
			//for (auto& v : ad.rot) v -= baseRot;
		}
		for (auto& v : ad.scl) v -= baseScl;
	}
}

bool allEqualVector(const std::vector<Vector3>& v, size_t start, size_t end);
bool allEqualVector(const std::vector<Vector3>& v, size_t start, size_t end, int axis);
bool allEqualVector(const std::vector<Vector3>& v, size_t start, size_t end, int axis, float scale);
bool allEqualVector(const std::vector<Vector4>& v, size_t start, size_t end, int axis, float scale);
void findMinMaxSIMD(const std::vector<Vector3>& v, size_t start, size_t end, Vector3& minOut, Vector3& maxOut);
void findMinMaxSIMD(const std::vector<Vector4>& v, size_t start, size_t end, Vector4& minOut, Vector4& maxOut);

float HalfToFloat(const uint16_t h);
uint16_t FloatToHalf(float value);
void SinCos(float radians, float* sine, float* cosine);

inline Vector48 Pack48(Vector3 a) {
	Vector48  out{};
	out.x = FloatToHalf(a.x);
	out.y = FloatToHalf(a.y);
	out.z = FloatToHalf(a.z);
	return out;
}

inline Vector3 Unpack48(Vector48 a) {
	Vector3 out{};
	out.x = HalfToFloat(static_cast<uint16_t>(a.x));
	out.y = HalfToFloat(static_cast<uint16_t>(a.y));
	out.z = HalfToFloat(static_cast<uint16_t>(a.z));
	return out;
}


// r5
uint64_t StringToGuid(const char* string);

// studio
void QuaternionMatrix(const Quaternion& q, Matrix3x4_t& matrix);
void MatrixRads(const Matrix3x4_t& matrix, float* angles);
void QuaternionRads(const Quaternion& q, Vector3& angles);
void AngleQuaternion(Vector3 angles, Quaternion& outQuat);
