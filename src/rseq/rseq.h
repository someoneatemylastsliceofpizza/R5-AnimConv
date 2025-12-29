#pragma once
#include <pch.h>

namespace p2 {
	namespace RLE {
		void ExtractAnimValue(int frame, const r5::anim::mstudioanimvalue_t* panimvalue, float scale, float& v1);
	}
}
namespace r5 {
	constexpr uint32_t ANIM_LOOPING			= 0x1;
	constexpr uint32_t ANIM_DELTA			= 0x4;
	constexpr uint32_t ANIM_ALLZEROS		= 0x20;
	constexpr uint32_t ANIM_VALID			= 0x20000;
	constexpr uint32_t ANIM_FRAMEMOVEMENT	= 0x40000;
	constexpr uint32_t ANIM_DATAPOINT		= 0x200000;

	namespace RLE {
		constexpr uint8_t BONEPOS			= 0x1;
		constexpr uint8_t BONEROT			= 0x2;
		constexpr uint8_t BONESCALE			= 0x4;
		constexpr uint8_t BONEDATA			= (BONEPOS | BONEROT | BONESCALE);

		int GetAnimValueOffset(const r5::anim::mstudioanimvalue_t* const panimvalue);

		void ExtractAnimValue(const r5::anim::mstudioanimvalue_t* panimvalue, const int frame, const float scale, float& v1);
		void ExtractAnimValue(int frame, const r5::anim::mstudioanimvalue_t* panimvalue, float scale, float& v1);
		void ExtractAnimValue(int frame, const r5::anim::mstudioanimvalue_t* panimvalue, float scale, float& v1, float& v2);

		void CalcBonePosition(const r5::anim::mstudio_rle_anim_t& pAnim, uint16_t** BoneTrackData, Vector3& trackval, uint32_t localframe);
		void CalcBoneQuaternion(const r5::anim::mstudio_rle_anim_t& pAnim, uint16_t** BoneTrackData, Vector3& trackval, uint32_t localframe);
		void CalcBoneScale(const r5::anim::mstudio_rle_anim_t& pAnim, uint16_t** BoneTrackData, Vector3& trackval, uint32_t localframe);

		void ParseIkrules_v10(const r5::anim::v10::mstudioanimdesc_t* pAnimDesc, temp::animdesc_t& anim);
		template <typename T> void ParseIkrules_v12(const T* pAnimDesc, temp::animdesc_t& anim);
		void ParseFrameMovements_v10(const r5::anim::v10::mstudioanimdesc_t* pAnimDesc, temp::animdesc_t& anim);
		template <typename T> void ParseFrameMovements_v12(const T* pAnimDesc, temp::animdesc_t& anim);
	}
}

int GetSectionLength(const int numframes, const int sectionframes, const int section);
template <typename T> int GetSectionLength(T& animdesc, const int section, const int numSections);
template <typename T> int GetSectionCount(T& animdesc);

std::vector<int32_t> GetAnimIndexes(const int32_t* pBlends, temp::Sequence& seq, int32_t numanims);
std::vector<int32_t> GetAnimIndexes(const uint16_t* pBlends, temp::Sequence& seq, int32_t numanims);

void ParsePoseKey(r5::anim::v10::mstudioseqdesc_t* pSeqDesc, temp::Sequence& seq);
void ParsePoseKey(r5::anim::v11::mstudioseqdesc_t* pSeqDesc, temp::Sequence& seq);
void ParseEvent_v10(r5::anim::v10::mstudioseqdesc_t* pSeqDesc, temp::Sequence& seq);
void ParseEvent_v11(r5::anim::v11::mstudioseqdesc_t* pSeqDesc, temp::Sequence& seq);
void ParseAutoLayer(r5::anim::v10::mstudioseqdesc_t* pSeqDesc, temp::Sequence& seq);
void ParseAutoLayer(r5::anim::v11::mstudioseqdesc_t* pSeqDesc, temp::Sequence& seq);
void ParseWeightList(r5::anim::v10::mstudioseqdesc_t* pSeqDesc, temp::Sequence& seq);
void ParseWeightList(r5::anim::v11::mstudioseqdesc_t* pSeqDesc, temp::Sequence& seq);
void ParseActMod_v10(const r5::anim::v10::mstudioseqdesc_t* pSeqDesc, temp::Sequence& seq);
void ParseActMod_v11(const r5::anim::v11::mstudioseqdesc_t* pSeqDesc, temp::Sequence& seq);

void ParseRSEQ_v10(std::string in_dir, temp::rig_t& rig);
void ParseRSEQ_v11(std::string in_dir, temp::rig_t& rig);

void WriteAnim(char*& pData, r5::anim::studioanimvalue_ptr_t* animvalueptr, std::vector<Vector3> rawdata, int32_t startframe, int32_t endframe, float scale);
template <typename T> void WriteAnimData(char*& pData, const std::vector<T>& rawdata, uint32_t startframe, uint32_t endframe, int axis, float scale);
template <typename T> void WriteCompressedAnim(char*& pData, const std::vector<T>& rawdata, temp::animblock_t c, int axis, float scale);
void WriteRSEQ_v7(temp::rig_t& rig, bool bSkipEvents = false);
