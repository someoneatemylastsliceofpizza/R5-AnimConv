#include "src/pch.h"
#include <rseq/rseq.h>


int GetSectionLength(const int numframes, const int sectionframes, const int section) {
	if (!sectionframes) return numframes;

	const int frameoffset = (section * sectionframes);
	const int remainingframes = numframes - frameoffset;

	if (remainingframes <= sectionframes)
		return remainingframes;

	return sectionframes;
}

template <typename T>
int GetSectionLength(T& animdesc, const int section, const int numSections) {
	if (!animdesc.sectionframes) return animdesc.numframes;
	if (section == (numSections - 1)) return 1;

	const int frameoffset = (section * animdesc.sectionframes);
	const int remainingframes = animdesc.numframes - frameoffset - 1;

	if (remainingframes <= animdesc.sectionframes)
		return remainingframes;

	return animdesc.sectionframes;
}
template int GetSectionLength<p2::mstudioanimdesc_t>(p2::mstudioanimdesc_t&, const int, const int);
template int GetSectionLength<r2::mstudioanimdesc_t>(r2::mstudioanimdesc_t&, const int, const int);
template int GetSectionLength<r5::anim::v7::mstudioanimdesc_t>(r5::anim::v7::mstudioanimdesc_t&, const int, const int);

void ExtractAnimValue_v49(int frame, const r5::anim::mstudioanimvalue_t* panimvalue, float scale, float& v1) {
	if (!panimvalue) {
		v1 = 0;
		return;
	}

	int k = frame;

	while (panimvalue->meta.total <= k) {
		k -= panimvalue->meta.total;
		panimvalue += panimvalue->meta.type + 1;
		if (panimvalue->meta.total == 0) {
			v1 = 0;
			return;
		}
	}
	if (panimvalue->meta.type > k) {
		v1 = panimvalue[k + 1].value * scale;
	}
	else {
		v1 = panimvalue[panimvalue->meta.type].value * scale;
	}
}

int GetSectionCount(p2::mstudioanimdesc_t& animdesc) {
	const int sectionbase = (animdesc.numframes - 1) / animdesc.sectionframes;
	const int sectionindex = sectionbase + 1;
	return sectionindex + 1;
}

int GetSectionCount(r2::mstudioanimdesc_t& animdesc) {
	const int sectionbase = (animdesc.numframes - 1) / animdesc.sectionframes;
	const int sectionindex = sectionbase + 1;
	return sectionindex + 1;
}

int GetSectionCount(r5::anim::v7::mstudioanimdesc_t& animdesc) {
	const int useTrailSection = (animdesc.flags & 0x200000) ? false : true;

	const int sectionbase = (animdesc.numframes - 1) / animdesc.sectionframes;
	const int sectionindex = sectionbase + useTrailSection;
	return sectionindex + 1;
}

template <typename T>
void WriteCompressedAnim(char*& pData, const std::vector<T>& rawdata, temp::animblock_t c, int axis, float scale) {
	uint32_t framerange = c.endframe - c.startframe;

	auto* valueRLE = reinterpret_cast<r5::anim::mstudioanimvalue_t*>(pData);
	char* cValueRLE = reinterpret_cast<char*>((char*)valueRLE + 4);
	pData += sizeof(r5::anim::mstudioanimvalue_t);

	valueRLE->meta.type = c.comptype;
	valueRLE->meta.total = framerange;

	int16_t base = (int16_t)(rawdata[c.startframe][axis] / scale);
	switch (c.comptype) {
	case 0:
		for (int i = 0; i < framerange; i++) {
			int32_t frame = c.startframe + i;
			valueRLE[i + 1].value = (int16_t)(rawdata[frame][axis] / scale);
			pData += sizeof(int16_t);
		}
		break;
	case 1:
		valueRLE[1].value = base;
		pData += sizeof(int16_t);
		for (int i = 0; i < framerange - 1; i++) {
			int32_t frame = c.startframe + i;
			cValueRLE[i] = (int16_t)(rawdata[frame + 1][axis] / scale) - base;
		}
		pData += (framerange - 1) * sizeof(int8_t);
		break;
	case 2:
		valueRLE[1].value = base;
		pData += sizeof(int16_t);
		break;
	default:
		std::cerr << "[!] Error: anim data compress type is invalid.\n";
		break;
	}
	ALIGN2(pData);
}
template void WriteCompressedAnim<Vector3>(char*&, const std::vector<Vector3>&, temp::animblock_t, int, float);
template void WriteCompressedAnim<Vector4>(char*&, const std::vector<Vector4>&, temp::animblock_t, int, float);


template <typename T>
static void FindConstantRanges(const std::vector<T>& raw, int axis, float eps, std::vector<temp::animblock_t>& out,uint32_t start, uint32_t end)
{
	uint32_t i = start;
	while (i < end)
	{
		float ref = raw[i][axis];
		uint32_t j = i + 1;
		while (j < end && raw[j][axis] == ref)
			j++;

		if (j - i >= 3)
			out.push_back({ 2, i, j });

		i = j;
	}
}

template <typename T>
static void FindDiffRanges(const std::vector<T>& raw, int axis, float threshold, std::vector<temp::animblock_t>& out, uint32_t start, uint32_t end,
	const std::vector<temp::animblock_t>& existing)
{
	uint32_t i = start;

	auto isCovered = [&](uint32_t f) {
		for (auto& b : existing)
			if (f >= b.startframe && f < b.endframe)
				return true;
		return false;
		};

	while (i < end)
	{
		if (isCovered(i)) { i++; continue; }

		float ref = raw[i][axis];
		float vmin = ref, vmax = ref;
		uint32_t j = i + 1;

		while (j < end && !isCovered(j))
		{
			float v = raw[j][axis];
			vmin = std::min(vmin, v);
			vmax = std::max(vmax, v);
			if ((ref - vmin) >= threshold || (vmax - ref) >= threshold)
				break;
			j++;
		}

		if (j - i >= 3)
			out.push_back({ 1, i, j });

		i = j;
	}
}

template <typename T>
void WriteAnimData(char*& pData, const std::vector<T>& rawdata, uint32_t startframe, uint32_t endframe, int axis, float scale){
	std::vector<temp::animblock_t> blocks;
	std::vector<temp::animblock_t> merged;
	std::vector<temp::animblock_t> final;
	blocks.reserve(64);
	merged.reserve(64);
	final.reserve(64);
	const float threshold = 127.0f * scale;

	FindConstantRanges(rawdata, axis, scale, blocks, startframe, endframe);
	FindDiffRanges(rawdata, axis, threshold, blocks, startframe, endframe, blocks);
	
	std::sort(blocks.begin(), blocks.end(),
		[](auto& a, auto& b) { return a.startframe < b.startframe; });

	uint32_t pos = startframe;

	for (auto& b : blocks)
	{
		if (pos < b.startframe)
			merged.push_back({ 0, pos, b.startframe });
		merged.push_back(b);
		pos = b.endframe;
	}
	if (pos < endframe)
		merged.push_back({ 0, pos, endframe });

	for (auto& b : merged) {
		if (!final.empty() &&
			final.back().comptype == b.comptype && final.back().comptype == 0 &&
			final.back().endframe == b.startframe)
		{
			final.back().endframe = b.endframe;
		}
		else final.push_back(b);
	}

	for (auto& c : final)
		WriteCompressedAnim(pData, rawdata, c, axis, scale);
}
template void WriteAnimData<Vector3>(char*&, const std::vector<Vector3>&, const uint32_t, const uint32_t, int, float);
template void WriteAnimData<Vector4>(char*&, const std::vector<Vector4>&, const uint32_t, const uint32_t, int, float);

void WriteAnim(char*& pData, r5::anim::studioanimvalue_ptr_t* animvalueptr, std::vector<Vector3> rawdata, int32_t startframe, int32_t endframe, float scale) {
	animvalueptr->offset = pData - (char*)animvalueptr;
	uint8_t tmp = 0;
	uint8_t* offsets[] = { &tmp, &animvalueptr->idx1, &animvalueptr->idx2 };

	char* beginaddress = pData;
	for (int axis = 0; axis < 3; axis++) {
		bool bIsAllEqual = allEqualVector(rawdata, startframe, endframe, axis, scale);
		if (bIsAllEqual && rawdata[startframe][axis] == 0) continue;

		// offsets
		animvalueptr->flags |= (4 >> axis);
		*offsets[axis] = (uint8_t)((pData - beginaddress) / 2);

		WriteAnimData(pData, rawdata, startframe, endframe, axis, scale);
	}
}