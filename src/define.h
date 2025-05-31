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
