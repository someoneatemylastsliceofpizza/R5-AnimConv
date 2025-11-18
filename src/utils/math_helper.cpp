#include "src/pch.h"

// math

float HalfToFloat(const uint16_t h) {
	uint16_t h_exp = (h & 0x7C00u) >> 10;
	uint16_t h_sig = h & 0x03FFu;
	uint32_t f_sgn = (h & 0x8000u) << 16;

	uint32_t f_exp, f_sig;

	if (h_exp == 0) {
		if (h_sig == 0) {
			f_exp = 0;
			f_sig = 0;
		}
		else {
			h_exp = 1;
			while ((h_sig & 0x0400u) == 0) {
				h_sig <<= 1;
				h_exp--;
			}
			h_sig &= 0x03FFu;
			f_exp = (127 - 15 - h_exp + 1) << 23;
			f_sig = h_sig << 13;
		}
	}
	else if (h_exp == 0x1F) {
		f_exp = 0xFF << 23;
		f_sig = h_sig << 13;
	}
	else {
		f_exp = (h_exp + (127 - 15)) << 23;
		f_sig = h_sig << 13;
	}

	uint32_t f = f_sgn | f_exp | f_sig;
	float result;
	memcpy(&result, &f, sizeof(result));
	return result;
}

uint16_t FloatToHalf(float value) {
	uint32_t f;
	memcpy(&f, &value, sizeof(f));

	uint32_t f_sgn = (f >> 16) & 0x8000;
	int32_t  f_exp = ((f >> 23) & 0xFF) - 127 + 15;
	uint32_t f_sig = f & 0x007FFFFF;

	uint16_t h;

	if (f_exp <= 0) {
		if (f_exp < -10) {
			h = f_sgn;
		}
		else {
			f_sig |= 0x00800000;
			int rshift = 1 - f_exp;
			uint32_t sub_sig = f_sig >> (rshift + 13);
			uint32_t round_bit = (f_sig >> (rshift + 12)) & 1;
			h = f_sgn | (sub_sig + round_bit);
		}
	}
	else if (f_exp == 0xFF - 127 + 15) {
		if (f_sig == 0) {
			h = f_sgn | 0x7C00;
		}
		else {
			h = f_sgn | 0x7C00 | (f_sig >> 13);
		}
	}
	else if (f_exp > 30) {
		h = f_sgn | 0x7C00;
	}
	else {
		h = f_sgn | (f_exp << 10) | ((f_sig + 0x00001000) >> 13);
	}
	return h;
}

bool allEqualVector(const std::vector<Vector3>& v, size_t start, size_t end) {
	if (end <= start) return true;
	const Vector3& ref = v[start];

	__m128 refVec = _mm_set_ps(0.0f, ref.z, ref.y, ref.x);

	for (size_t i = start + 1; i < end; ++i) {
		__m128 cur = _mm_set_ps(0.0f, v[i].z, v[i].y, v[i].x);
		__m128 cmp = _mm_cmpeq_ps(refVec, cur);
		if (_mm_movemask_ps(cmp) != 0xF) return false;
	}
	return true;
}

bool allEqualVector(const std::vector<Vector3>& v, size_t start, size_t end, int axis) {
	if (end <= start) return true;
	const float& ref = v[start][axis];

	for (size_t i = start + 1; i < end; ++i) {
		if (v[i][axis] != ref) return false;
	}
	return true;
}

bool allEqualVector(const std::vector<Vector3>& v, size_t start, size_t end, int axis, float scale) {
	if (end <= start) return true;
	const int16_t& ref = (int16_t)(v[start][axis] / scale);

	for (size_t i = start + 1; i < end; ++i) {
		if ((int16_t)(v[i][axis] / scale) != ref) return false;
	}
	return true;
}

bool allEqualVector(const std::vector<Vector4>& v, size_t start, size_t end, int axis, float scale) {
	if (end <= start) return true;
	const int16_t& ref = (int16_t)(v[start][axis] / scale);

	for (size_t i = start + 1; i < end; ++i) {
		if ((int16_t)(v[i][axis] / scale) != ref) return false;
	}
	return true;
}

void findMinMaxSIMD(const std::vector<Vector3>& v, size_t start, size_t end, Vector3& minOut, Vector3& maxOut) {
	if (start >= end) return;

	__m128 vmin = _mm_set_ps(0.0f, v[start].z, v[start].y, v[start].x);
	__m128 vmax = vmin;

	for (size_t i = start + 1; i < end; ++i) {
		__m128 cur = _mm_set_ps(0.0f, v[i].z, v[i].y, v[i].x);
		vmin = _mm_min_ps(vmin, cur);
		vmax = _mm_max_ps(vmax, cur);
	}

	alignas(16) float minVals[4], maxVals[4];
	_mm_store_ps(minVals, vmin);
	_mm_store_ps(maxVals, vmax);

	minOut = { minVals[0], minVals[1], minVals[2] };
	maxOut = { maxVals[0], maxVals[1], maxVals[2] };
}

void findMinMaxSIMD(const std::vector<Vector4>& v, size_t start, size_t end, Vector4& minOut, Vector4& maxOut) {
	if (start >= end) return;

	__m128 vmin = _mm_set_ps(v[start].w, v[start].z, v[start].y, v[start].x);
	__m128 vmax = vmin;

	for (size_t i = start + 1; i < end; ++i) {
		__m128 cur = _mm_set_ps(v[i].w, v[i].z, v[i].y, v[i].x);
		vmin = _mm_min_ps(vmin, cur);
		vmax = _mm_max_ps(vmax, cur);
	}

	alignas(16) float minVals[4], maxVals[4];
	_mm_store_ps(minVals, vmin);
	_mm_store_ps(maxVals, vmax);

	minOut = { minVals[0], minVals[1], minVals[2] , minVals[3]};
	maxOut = { maxVals[0], maxVals[1], maxVals[2] , maxVals[3]};
}

// studio
void SinCos(float radians, float* sine, float* cosine){
	*sine = sinf(radians);
	*cosine = cosf(radians);
}

void QuaternionMatrix(const Quaternion& q, Matrix3x4_t& matrix) {
	matrix[0][0] = 1.0f - 2.0f * q.y * q.y - 2.0f * q.z * q.z;
	matrix[1][0] = 2.0f * q.x * q.y + 2.0f * q.w * q.z;
	matrix[2][0] = 2.0f * q.x * q.z - 2.0f * q.w * q.y;

	matrix[0][1] = 2.0f * q.x * q.y - 2.0f * q.w * q.z;
	matrix[1][1] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.z * q.z;
	matrix[2][1] = 2.0f * q.y * q.z + 2.0f * q.w * q.x;

	matrix[0][2] = 2.0f * q.x * q.z + 2.0f * q.w * q.y;
	matrix[1][2] = 2.0f * q.y * q.z - 2.0f * q.w * q.x;
	matrix[2][2] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.y * q.y;

	matrix[0][3] = 0.0f;
	matrix[1][3] = 0.0f;
	matrix[2][3] = 0.0f;
}

void MatrixRads(const Matrix3x4_t& matrix, float* angles)
{
	float forward[3];
	float left[3];
	float up[3];

	forward[0] = matrix[0][0];
	forward[1] = matrix[1][0];
	forward[2] = matrix[2][0];
	left[0] = matrix[0][1];
	left[1] = matrix[1][1];
	left[2] = matrix[2][1];
	up[2] = matrix[2][2];

	float xyDist = sqrtf(forward[0] * forward[0] + forward[1] * forward[1]);

	if (xyDist > 0.001f)
	{
		angles[1] = atan2f(forward[1], forward[0]);
		angles[0] = atan2f(-forward[2], xyDist);
		angles[2] = atan2f(left[2], up[2]);
	}
	else
	{
		angles[1] = atan2f(-left[0], left[1]);
		angles[0] = atan2f(-forward[2], xyDist);
		angles[2] = 0;
	}
}

void QuaternionRads(const Quaternion& q, Vector3& angles)
{
	Matrix3x4_t matrix;
	QuaternionMatrix(q, matrix);
	MatrixRads(matrix, &angles.x);
}

void AngleQuaternion(Vector3 angles, Quaternion& outQuat) {

	float sr, sp, sy, cr, cp, cy;

	__m128 radians, sine, cosine;
	radians = _mm_load_ps(&angles.x);
	radians = _mm_mul_ps(radians, _mm_set_ps1(0.5f));

	sine = _mm_sincos_ps(&cosine, radians);

	sr = sine.m128_f32[0];
	sp = sine.m128_f32[1];
	sy = sine.m128_f32[2];
	cr = cosine.m128_f32[0];
	cp = cosine.m128_f32[1];
	cy = cosine.m128_f32[2];

	float srXcp = sr * cp, crXsp = cr * sp;
	outQuat.x = srXcp * cy - crXsp * sy;
	outQuat.y = crXsp * cy + srXcp * sy;

	float crXcp = cr * cp, srXsp = sr * sp;
	outQuat.z = crXcp * sy - srXsp * cy;
	outQuat.w = crXcp * cy + srXsp * sy;
}

uint64_t StringToGuid(const char* string)
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