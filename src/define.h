#pragma once

#include <cstdint> 

#define STRING_FROM_IDX(base, idx) reinterpret_cast<const char*>((char*)base + idx)
#define PTR_FROM_IDX(type, base, idx) reinterpret_cast<type*>((char*)base + idx)

#define ALIGN2( a ) a = (byte *)((__int64)((byte *)a + 1) & ~ 1)
#define ALIGN4( a ) a = (byte *)((__int64)((byte *)a + 3) & ~ 3)
#define ALIGN16( a ) a = (byte *)((__int64)((byte *)a + 15) & ~ 15)

#define MAX(a,b) ((a > b) ? a : b)
#define MIN(a,b) ((a < b) ? a : b)

typedef char byte;

struct Vector2{
	float x, y;
};
struct Vector4
{
	float x, y, z, w;

	Vector4& operator=(const Vector4& vecIn)
	{
		Vector4 vecOut;

		vecOut.x = vecIn.x;
		vecOut.y = vecIn.y;
		vecOut.z = vecIn.z;
		vecOut.w = vecIn.w;

		return vecOut;
	}

	inline bool IsAllZero()
	{
		return (x == 0 && y == 0 && z == 0 && w == 0);
	}
};
struct Vector64
{
	uint64_t x : 21;
	uint64_t y : 21;
	uint64_t z : 22;
};

struct Vector48  
{  
   short x;  
   short y;  
   short z; 
};

struct Vector3{
	float x, y, z;

	void operator-=(Vector3 a)
	{
		x -= a.x;
		y -= a.y;
		z -= a.z;
	}

	Vector3 operator*(Vector3& a){
		return { x * a.x, y * a.y, z * a.z };
	}

	Vector3 operator*(float a){
		return { x * a, y * a, z * a };
	}

	inline float& operator[](int i){
		return ((float*)this)[i];
	}

	inline float operator[](int i) const {
		return ((float*)this)[i];
	}

	inline void Print()
	{
		printf("<x: %f, y: %f, z: %f>\n", x, y, z);
	}

	inline void Print(int idx)
	{
		printf("%d <x: %f, y: %f, z: %f>\n", idx, x, y, z);
	}

	inline bool IsAllZero()
	{
		return (x == 0 && y == 0 && z == 0);
	}

    inline float Min() {
		return MIN(x, MIN(y, z));
    }

	inline float Max() {
		return MAX(x, MAX(y, z));
	}

	inline float Get(int xyz) {
		if (xyz == 0) return x;
		if (xyz == 1) return y;
		if (xyz == 2) return z;
		return 0;
	}
};


struct RadianEuler {
	float x, y, z;
};

struct Quaternion {
	float x, y, z, w;
};

struct Quaternion64 {
	uint64_t x : 21;
	uint64_t y : 21;
	uint64_t z : 21;
	uint64_t wneg : 1;
};

struct Matrix3x4_t {
	float m[3][4];
};


inline uint64_t StringToGuid(const char* string)
{
	uint64_t         v2;
	int              v3;
	uint32_t         v4;
	uint32_t          i;
	uint64_t         v6;
	int              v7;
	int              v8;
	int              v9;
	uint32_t        v10;
	int             v12;
	uint32_t* a1 = (uint32_t*)string;

	v2 = 0i64;
	v3 = 0;
	v4 = (*a1 - 45 * ((~(*a1 ^ 0x5C5C5C5Cu) >> 7) & (((*a1 ^ 0x5C5C5C5Cu) - 0x1010101) >> 7) & 0x1010101)) & 0xDFDFDFDF;
	for (i = ~*a1 & (*a1 - 0x1010101) & 0x80808080; !i; i = v8 & 0x80808080)
	{
		v6 = v4;
		v7 = a1[1];
		++a1;
		v3 += 4;
		v2 = ((((uint64_t)(0xFB8C4D96501i64 * v6) >> 24) + 0x633D5F1 * v2) >> 61) ^ (((uint64_t)(0xFB8C4D96501i64 * v6) >> 24)
			+ 0x633D5F1 * v2);
		v8 = ~v7 & (v7 - 0x1010101);
		v4 = (v7 - 45 * ((~(v7 ^ 0x5C5C5C5Cu) >> 7) & (((v7 ^ 0x5C5C5C5Cu) - 0x1010101) >> 7) & 0x1010101)) & 0xDFDFDFDF;
	}
	v9 = -1;
	v10 = (i & -(signed)i) - 1;
	if (_BitScanReverse((unsigned long*)&v12, v10))
	{
		v9 = v12;
	}
	return 0x633D5F1 * v2 + ((0xFB8C4D96501i64 * (uint64_t)(v4 & v10)) >> 24) - 0xAE502812AA7333i64 * (uint32_t)(v3 + v9 / 8);
}
