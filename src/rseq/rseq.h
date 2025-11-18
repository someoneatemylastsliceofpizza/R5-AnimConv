#include "src/pch.h"

int GetSectionLength(const int numframes, const int sectionframes, const int section);
template <typename T>
int GetSectionLength(T& animdesc, const int section, const int numSections);
int GetSectionCount(p2::mstudioanimdesc_t& animdesc);
int GetSectionCount(r2::mstudioanimdesc_t& animdesc);
int GetSectionCount(r5::anim::v7::mstudioanimdesc_t& animdesc);
void ExtractAnimValue_v49(int frame, const r5::anim::mstudioanimvalue_t* panimvalue, float scale, float& v1);
void WriteRSEQ_v7(temp::rig_t& rig);

void WriteAnim(char*& pData, r5::anim::studioanimvalue_ptr_t* animvalueptr, std::vector<Vector3> rawdata, int32_t startframe, int32_t endframe, float scale);
template <typename T> void WriteAnimData(char*& pData, const std::vector<T>& rawdata, uint32_t startframe, uint32_t endframe, int axis, float scale);
template <typename T> void WriteCompressedAnim(char*& pData, const std::vector<T>& rawdata, temp::animblock_t c, int axis, float scale);