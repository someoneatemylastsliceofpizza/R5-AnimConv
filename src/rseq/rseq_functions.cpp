#include <pch.h>
#include <rseq/rseq.h>

using namespace r5;

static char s_FrameBitCountLUT[4]{ 0, 2, 4, 0 };
static float s_FrameValOffsetLUT[4]{ 0.0f, 3.0f, 15.0f, 0.0f };
static char s_AnimSeekLUT[60]{
	1,  15, 16, 2,  7,  8,  2,  15, 0,  3,  15, 0,  4,  15, 0,  5,
	15, 0,  6,  15, 0,  7,  15, 0,  2,  15, 2,  3,  15, 2,  4,  15,
	2,  5,  15, 2,  6,  15, 2,  7,  15, 2,  2,  15, 4,  3,  15, 4,
	4,  15, 4, 5, 15, 4, 6, 15, 4,  7,  15, 4,
};

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
	if constexpr (
		std::is_same_v<T, p2::mstudioanimdesc_t> ||
		std::is_same_v<T, r2::mstudioanimdesc_t> ||
		std::is_same_v<T, r5::anim::v7::mstudioanimdesc_t>)
	{
		if (!animdesc.sectionframes) return animdesc.numframes;
		if (section == (numSections - 1)) return 1;

		const int frameoffset = (section * animdesc.sectionframes);
		const int remainingframes = animdesc.numframes - frameoffset - 1;

		if (remainingframes <= animdesc.sectionframes) return remainingframes;
		return animdesc.sectionframes;

	}
	else if constexpr (
		std::is_same_v<T, r5::anim::v10::mstudioanimdesc_t> ||
		std::is_same_v<T, r5::anim::v11::mstudioanimdesc_t> ||
		std::is_same_v<T, r5::anim::v121::mstudioanimdesc_t>)
	{
		if (!animdesc.sectionframes) return animdesc.numframes;
		const int sectionstallframes = animdesc.sectionstallframes;

		if (sectionstallframes && section == 0) return sectionstallframes;
		if (section == (numSections - 1)) return 1;

		const int sectionbase = section - (bool)sectionstallframes;
		const int frameoffset = sectionstallframes + (sectionbase * animdesc.sectionframes);
		const int remainingframes = animdesc.numframes - frameoffset - 1;

		if (remainingframes <= animdesc.sectionframes) return remainingframes;
		return animdesc.sectionframes;
	}
	return 0;
}
template int GetSectionLength<p2::mstudioanimdesc_t>(p2::mstudioanimdesc_t&, const int, const int);
template int GetSectionLength<r2::mstudioanimdesc_t>(r2::mstudioanimdesc_t&, const int, const int);
template int GetSectionLength<r5::anim::v7::mstudioanimdesc_t>(r5::anim::v7::mstudioanimdesc_t&, const int, const int);
template int GetSectionLength<r5::anim::v10::mstudioanimdesc_t>(r5::anim::v10::mstudioanimdesc_t&, const int, const int);
template int GetSectionLength<r5::anim::v11::mstudioanimdesc_t>(r5::anim::v11::mstudioanimdesc_t&, const int, const int);
template int GetSectionLength<r5::anim::v121::mstudioanimdesc_t>(r5::anim::v121::mstudioanimdesc_t&, const int, const int);

int RLE::GetAnimValueOffset(const r5::anim::mstudioanimvalue_t* const panimvalue) {
	const int lutBaseIdx = panimvalue->meta.type * 3;
	return (s_AnimSeekLUT[lutBaseIdx] + ((s_AnimSeekLUT[lutBaseIdx + 1] + panimvalue->meta.total * s_AnimSeekLUT[lutBaseIdx + 2]) >> 4));
}

void RLE::ExtractAnimValue(const r5::anim::mstudioanimvalue_t* panimvalue, const int frame, const float scale, float& v1)
{
	switch (panimvalue->meta.type) {
	case 0: {
		v1 = static_cast<float>(panimvalue[frame + 1].value) * scale;
		return;
	}
	case 1: {
		int16_t value = panimvalue[1].value;
		if (frame > 0) value += reinterpret_cast<const char*>(&panimvalue[2])[frame - 1];

		v1 = static_cast<float>(value) * scale;
		return;
	}
	default: {
		float v7 = 1.0f;
		const int bitLUTIndex = (panimvalue->meta.type - 2) / 6;
		const int numUnkValue = (panimvalue->meta.type - 2) % 6;

		float value = static_cast<float>(panimvalue[1].value);
		const float cycle = static_cast<float>(frame) / static_cast<float>(panimvalue->meta.total - 1);

		for (int i = 0; i < numUnkValue; i++) {
			v7 *= cycle;
			value += static_cast<float>(panimvalue[i + 2].value) * v7;
		}

		if (bitLUTIndex) {
			const int16_t* pbits = &panimvalue[numUnkValue + 2].value;
			value -= s_FrameValOffsetLUT[bitLUTIndex];
			const uint8_t maskBitCount = s_FrameBitCountLUT[bitLUTIndex];
			const uint8_t mask = (1 << maskBitCount) - 1;
			const int frameValueBitOffset = frame * maskBitCount;
			constexpr int animValueBitSize = (8 * sizeof(r5::anim::mstudioanimvalue_t));
			static_assert(animValueBitSize == 16);
			const int bitOffset = frameValueBitOffset & (animValueBitSize - 1);

			value += 2.0f * (mask & (pbits[frameValueBitOffset >> 4] >> bitOffset));
		}
		v1 = value * scale;
		return;
	}
	}
}

void p2::RLE::ExtractAnimValue(int frame, const r5::anim::mstudioanimvalue_t* panimvalue, float scale, float& v1) {
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

void RLE::ExtractAnimValue(int frame, const r5::anim::mstudioanimvalue_t* panimvalue, float scale, float& v1) {
	int k = frame;

	while (panimvalue->meta.total <= k) {
		k -= panimvalue->meta.total;
		int offset = RLE::GetAnimValueOffset(panimvalue);
		panimvalue += offset;
	}

	RLE::ExtractAnimValue(panimvalue, k, scale, v1);
}

void RLE::ExtractAnimValue(int frame, const r5::anim::mstudioanimvalue_t* panimvalue, float scale, float& v1, float& v2){
	int k = frame;

	while (panimvalue->meta.total <= k) {
		k -= panimvalue->meta.total;
		panimvalue += RLE::GetAnimValueOffset(panimvalue);
	}

	if (k >= panimvalue->meta.total - 1) {
		RLE::ExtractAnimValue(panimvalue, k, scale, v1);
		RLE::ExtractAnimValue(panimvalue + RLE::GetAnimValueOffset(panimvalue), 0, scale, v2);
	}
	else {
		RLE::ExtractAnimValue(panimvalue, k, scale, v1);
		RLE::ExtractAnimValue(panimvalue, k + 1, scale, v2);
	}
}

template <typename T>
int GetSectionCount(T& animdesc) {
	if constexpr (std::is_same_v<T, p2::mstudioanimdesc_t> || std::is_same_v<T, r2::mstudioanimdesc_t> || std::is_same_v<T, r5::anim::v7::mstudioanimdesc_t>) {
		return (animdesc.numframes - 1) / animdesc.sectionframes + 2;
	}
	else if constexpr (
		std::is_same_v<T, r5::anim::v10::mstudioanimdesc_t> ||
		std::is_same_v <T, r5::anim::v11::mstudioanimdesc_t>)
	{
		const int useTrail = (animdesc.flags & ANIM_DATAPOINT) ? 0 : 1;
		const int useStall = animdesc.sectionstallframes ? 1 : 0;
		const int base = (animdesc.numframes - animdesc.sectionstallframes - 1) / animdesc.sectionframes;
		return base + useTrail + useStall + 1;
	}
	else if constexpr (std::is_same_v<T, r5::anim::v121::mstudioanimdesc_t>) {
		const int useTrail = (animdesc.flags & ANIM_DATAPOINT) ? 0 : 1;
		const int useStall = animdesc.sectionstallframes ? (animdesc.sectionstallframes != animdesc.sectionframes) : 0;
		const int base = (animdesc.numframes - animdesc.sectionstallframes - 1) / animdesc.sectionframes;
		return base + useTrail + useStall + 1;
	}
	return 0;
}
template int GetSectionCount<p2::mstudioanimdesc_t>(p2::mstudioanimdesc_t&);
template int GetSectionCount<r2::mstudioanimdesc_t>(r2::mstudioanimdesc_t&);
template int GetSectionCount<r5::anim::v7::mstudioanimdesc_t>(r5::anim::v7::mstudioanimdesc_t&);
template int GetSectionCount<r5::anim::v10::mstudioanimdesc_t>(r5::anim::v10::mstudioanimdesc_t&);
template int GetSectionCount<r5::anim::v11::mstudioanimdesc_t>(r5::anim::v11::mstudioanimdesc_t&);
template int GetSectionCount<r5::anim::v121::mstudioanimdesc_t>(r5::anim::v121::mstudioanimdesc_t&);

void RLE::CalcBonePosition(const r5::anim::mstudio_rle_anim_t& pAnim, uint16_t** BoneTrackData, Vector3& trackval, uint32_t localframe) {
	if (!pAnim.bAnimPosition) {
		Vector48* packedpos = reinterpret_cast<Vector48*>(*BoneTrackData);
		trackval = Unpack48(*packedpos);
		*BoneTrackData += 3;
	}
	else {
		auto* valueptr = reinterpret_cast<r5::anim::studioanimvalue_ptr_t*>((char*)*BoneTrackData + 4);
		Vector3 Result(0, 0, 0);

		r5::anim::mstudioanimvalue_t* track_x = PTR_FROM_IDX(r5::anim::mstudioanimvalue_t, valueptr, valueptr->offset);
		r5::anim::mstudioanimvalue_t* track_y = track_x + valueptr->idx1;
		r5::anim::mstudioanimvalue_t* track_z = track_x + valueptr->idx2;
		r5::anim::mstudioanimvalue_t* tracks[] = { track_x, track_y, track_z };

		const long ptrflags = valueptr->flags;
		float ps = *(float*)*BoneTrackData;
		for (int cur_axis = 0; cur_axis < 3; cur_axis++) {
			if (_bittest(&ptrflags, 0x2 - cur_axis)) {
				ExtractAnimValue((int)localframe, tracks[cur_axis], *(float*)*BoneTrackData, Result[cur_axis]);
			}
		};

		trackval += Result;
		*BoneTrackData += 4;
	}
}

void RLE::CalcBoneQuaternion(const r5::anim::mstudio_rle_anim_t& pAnim, uint16_t** BoneTrackData, Vector3& trackval, uint32_t localframe) {
	Quaternion64* q64 = reinterpret_cast<Quaternion64*>(*BoneTrackData);
	if (!pAnim.bAnimRotation) {
		trackval = UnpackQuat64(*q64).ToRad();
		*BoneTrackData += 4;
	}
	else {
		auto* valueptr = reinterpret_cast<r5::anim::studioanimvalue_ptr_t*>((char*)*BoneTrackData);

		r5::anim::mstudioanimvalue_t* track_x = PTR_FROM_IDX(r5::anim::mstudioanimvalue_t, valueptr, valueptr->offset);
		r5::anim::mstudioanimvalue_t* track_y = track_x + valueptr->idx1;
		r5::anim::mstudioanimvalue_t* track_z = track_x + valueptr->idx2;
		r5::anim::mstudioanimvalue_t* tracks[] = { track_x, track_y, track_z };

		Vector3 rad(0, 0, 0);
		uint16_t ptrflags = valueptr->flags;
		for (int cur_axis = 0; cur_axis < 3; cur_axis++) {
			if (_bittest((const long*)&ptrflags, 0x2 - cur_axis)) {
				ExtractAnimValue((int)localframe, tracks[cur_axis], 0.00019175345f, rad[cur_axis]);
				trackval[cur_axis] = rad[cur_axis];
			}
		};

		*BoneTrackData += 2;
	}
}

void RLE::CalcBoneScale(const r5::anim::mstudio_rle_anim_t& pAnim, uint16_t** BoneTrackData, Vector3& trackval, uint32_t localframe)
{
	if (!pAnim.bAnimScale) {
		Vector48* packedscl = reinterpret_cast<Vector48*>(*BoneTrackData);
		trackval = Unpack48(*packedscl);
		*BoneTrackData += 3;
	}
	else {
		auto* valueptr = reinterpret_cast<r5::anim::studioanimvalue_ptr_t*>((char*)*BoneTrackData);
		Vector3 Result(0, 0, 0);

		r5::anim::mstudioanimvalue_t* track_x = PTR_FROM_IDX(r5::anim::mstudioanimvalue_t, valueptr, valueptr->offset);
		r5::anim::mstudioanimvalue_t* track_y = track_x + valueptr->idx1;
		r5::anim::mstudioanimvalue_t* track_z = track_x + valueptr->idx2;
		r5::anim::mstudioanimvalue_t* tracks[] = { track_x, track_y, track_z };

		const long ptrflags = valueptr->flags;
		const long* pp = &ptrflags;
		for (int cur_axis = 0; cur_axis < 3; cur_axis++) {
			if (_bittest(&ptrflags, 0x2 - cur_axis)) {
				ExtractAnimValue((int)localframe, tracks[cur_axis], 0.0030518509f, Result[cur_axis]);
			}
		};

		trackval += Result;
		*BoneTrackData += 4;
	}
}

std::vector<int32_t> GetAnimIndexes(const int32_t* pBlends, temp::Sequence& seq, int32_t numanims) {
	std::vector<int32_t> blends_index_map;
	std::unordered_map<int32_t, uint8_t> blendIndex;
	for (uint8_t blend_idx = 0; blend_idx < numanims; ++blend_idx) {
		int32_t val = pBlends[blend_idx];
		auto [it, inserted] = blendIndex.emplace(val, blends_index_map.size());
		if (inserted) blends_index_map.push_back(val);
		seq.blends.push_back(it->second);
	}
	seq.numuniqueblends = static_cast<int32_t>(blends_index_map.size());

	return blends_index_map;
}

std::vector<int32_t> GetAnimIndexes(const uint16_t* pBlends, temp::Sequence& seq, int32_t numanims) {
	std::vector<int32_t> blends_index_map;
	std::unordered_map<int32_t, uint8_t> blendIndex;
	for (uint8_t blend_idx = 0; blend_idx < numanims; ++blend_idx) {
		int32_t val = OFFSET(pBlends[blend_idx]);
		auto [it, inserted] = blendIndex.emplace(val, blends_index_map.size());
		if (inserted) blends_index_map.push_back(val);
		seq.blends.push_back(it->second);
	}
	seq.numuniqueblends = static_cast<int32_t>(blends_index_map.size());

	return blends_index_map;
}

void ParsePoseKey(r5::anim::v10::mstudioseqdesc_t* pSeqDesc, temp::Sequence& seq) {
	if (pSeqDesc->posekeyindex) {
		auto* pPosekeys = reinterpret_cast<float*>((char*)pSeqDesc + pSeqDesc->posekeyindex);
		for (int i = 0; i < (pSeqDesc->groupsize[0] + pSeqDesc->groupsize[1]); i++) {
			seq.posekeys.push_back(pPosekeys[i]);
		}
	}
}

void ParsePoseKey(r5::anim::v11::mstudioseqdesc_t* pSeqDesc, temp::Sequence& seq) {
	if (pSeqDesc->posekeyindex) {
		auto* pPosekeys = reinterpret_cast<float*>((char*)pSeqDesc + OFFSET(pSeqDesc->posekeyindex));
		for (int i = 0; i < (pSeqDesc->groupsize[0] + pSeqDesc->groupsize[1]); i++) {
			seq.posekeys.push_back(pPosekeys[i]);
		}
	}
}

void ParseEvent_v10(r5::anim::v10::mstudioseqdesc_t* pSeqDesc, temp::Sequence& seq) {
	if (pSeqDesc->numevents) {
		if (!(pSeqDesc->numevents % 68)) printf("[!] Warning: events might be wrong, needed to check it manually.\n");
		bool b2ndEvent = bool((pSeqDesc->weightlistindex - pSeqDesc->eventindex) % 272);

		for (int i = 0; i < pSeqDesc->numevents; i++) {
			temp::seqevent_t event{};
			if (b2ndEvent) {
				auto* pEvents = reinterpret_cast<r5::anim::v10::mstudioevent_2_t*>((char*)pSeqDesc + pSeqDesc->eventindex);
				event.name = STRING_FROM_IDX(&pEvents[i], pEvents[i].szeventindex);
				event.cycle = pEvents[i].cycle;
				event.event = pEvents[i].event;
				event.type = pEvents[i].type;
				event.options = std::string(pEvents[i].options);
			}
			else {
				auto* pEvents = reinterpret_cast<r5::anim::v10::mstudioevent_t*>((char*)pSeqDesc + pSeqDesc->eventindex);
				event.name = STRING_FROM_IDX(&pEvents[i], pEvents[i].szeventindex);
				event.cycle = pEvents[i].cycle;
				event.event = pEvents[i].event;
				event.type = pEvents[i].type;
				event.options = std::string(pEvents[i].options);
			}
			seq.events.push_back(event);
		}
	}
}

void ParseEvent_v11(r5::anim::v11::mstudioseqdesc_t* pSeqDesc, temp::Sequence& seq) {
	if (pSeqDesc->numevents) {
		for (int i = 0; i < pSeqDesc->numevents; i++) {
			temp::seqevent_t event{};
			auto* pEvents = reinterpret_cast<r5::anim::v11::mstudioevent_t*>((char*)pSeqDesc + OFFSET(pSeqDesc->eventindex));
			event.name = STRING_FROM_IDX(&pEvents[i], OFFSET(pEvents[i].szeventindex));
			event.cycle = pEvents[i].cycle;
			event.event = pEvents[i].event;
			event.type = pEvents[i].type;
			event.options = STRING_FROM_IDX(&pEvents[i], OFFSET(pEvents[i].optionsindex));
			seq.events.push_back(event);
		}
	}
}

void ParseAutoLayer(r5::anim::v10::mstudioseqdesc_t* pSeqDesc, temp::Sequence& seq) {
	auto* pAutolayer = reinterpret_cast<r5::anim::v7::mstudioautolayer_t*>((char*)pSeqDesc + pSeqDesc->autolayerindex);
	for (int i = 0; i < pSeqDesc->numautolayers; i++) {
		temp::autolayer_t autolayer{};
		autolayer.guidSequence = pAutolayer[i].guidSequence;
		autolayer.iSequence = pAutolayer[i].iSequence;
		autolayer.iPose = pAutolayer[i].iPose;
		autolayer.flags = pAutolayer[i].flags;
		autolayer.start = pAutolayer[i].start;
		autolayer.peak = pAutolayer[i].peak;
		autolayer.tail = pAutolayer[i].tail;
		autolayer.end = pAutolayer[i].end;
		seq.autolayers.push_back(autolayer);
	}
}

void ParseAutoLayer(r5::anim::v11::mstudioseqdesc_t* pSeqDesc, temp::Sequence& seq) {
	auto* pAutolayer = reinterpret_cast<r5::anim::v11::mstudioautolayer_t*>((char*)pSeqDesc + OFFSET(pSeqDesc->autolayerindex));
	for (int i = 0; i < pSeqDesc->numautolayers; i++) {
		temp::autolayer_t autolayer{};
		autolayer.guidSequence = pAutolayer[i].guidSequence;
		autolayer.iPose = pAutolayer[i].iPose;
		autolayer.flags = pAutolayer[i].flags;
		autolayer.start = pAutolayer[i].start;
		autolayer.peak = pAutolayer[i].peak;
		autolayer.tail = pAutolayer[i].tail;
		autolayer.end = pAutolayer[i].end;
		seq.autolayers.push_back(autolayer);
	}
}

void ParseWeightList(r5::anim::v10::mstudioseqdesc_t* pSeqDesc, temp::Sequence& seq) {
	auto* pWeightList = reinterpret_cast<float*>((char*)pSeqDesc + pSeqDesc->weightlistindex);
	for (int i = 0; i < seq.weightlist.size(); i++) {
		seq.weightlist[i] = pWeightList[i];
	}
}

void ParseWeightList(r5::anim::v11::mstudioseqdesc_t* pSeqDesc, temp::Sequence& seq) {
	auto* pWeightList = reinterpret_cast<float*>((char*)pSeqDesc + OFFSET(pSeqDesc->weightlistindex));
	for (int i = 0; i < seq.weightlist.size(); i++) {
		seq.weightlist[i] = pWeightList[i];
	}
}

void ParseActMod_v10(const r5::anim::v10::mstudioseqdesc_t* pSeqDesc, temp::Sequence& seq) {
	if (pSeqDesc->numactivitymodifiers) {
		auto* pActMod = reinterpret_cast<r5::anim::v10::mstudioactivitymodifier_t*>((char*)pSeqDesc + pSeqDesc->activitymodifierindex);
		for (int i = 0; i < pSeqDesc->numactivitymodifiers; i++) {
			temp::actmod_t actmod{};
			actmod.name = STRING_FROM_IDX(&pActMod[i], pActMod[i].sznameindex);
			actmod.negate = pActMod->negate;
			seq.actmods.push_back(actmod);
		}
	}
}

void ParseActMod_v11(const r5::anim::v11::mstudioseqdesc_t* pSeqDesc, temp::Sequence& seq) {
	if (pSeqDesc->numactivitymodifiers) {
		auto* pActMod = reinterpret_cast<r5::anim::v11::mstudioactivitymodifier_t*>((char*)pSeqDesc + OFFSET(pSeqDesc->activitymodifierindex));
		for (int i = 0; i < pSeqDesc->numactivitymodifiers; i++) {
			temp::actmod_t actmod{};
			actmod.name = STRING_FROM_IDX(&pActMod[i], OFFSET(pActMod[i].sznameindex));
			actmod.negate = pActMod->negate;
			seq.actmods.push_back(actmod);
		}
	}
}

void RLE::ParseIkrules_v10(const r5::anim::v10::mstudioanimdesc_t* pAnimDesc, temp::animdesc_t& anim) {
	if (nullptr == pAnimDesc) PRINTANDTHROW(anim.name, "pAnimDesc is null");

	// ikrules
	if (pAnimDesc->numikrules) {
		auto* ikrules = reinterpret_cast<r5::anim::v10::mstudioikrule_t*>((char*)pAnimDesc + pAnimDesc->ikruleindex);
		for (int i = 0; i < pAnimDesc->numikrules; i++) {
			temp::ikrule_t ikrule{};
			ikrule.index = ikrules[i].index;
			ikrule.type = ikrules[i].type;
			ikrule.chain = ikrules[i].chain;
			ikrule.bone = ikrules[i].bone;
			ikrule.slot = ikrules[i].slot;
			ikrule.height = ikrules[i].height;
			ikrule.radius = ikrules[i].radius;
			ikrule.floor = ikrules[i].floor;
			ikrule.pos = ikrules[i].pos;
			ikrule.q = ikrules[i].q;
			for (int j = 0; j < 6; j++) ikrule.scale[j] = ikrules[i].scale[j];
			ikrule.sectionframes = ikrules[i].sectionframes;
			ikrule.iStart = ikrules[i].iStart;
			ikrule.start = ikrules[i].start;
			ikrule.peak = ikrules[i].peak;
			ikrule.tail = ikrules[i].tail;
			ikrule.end = ikrules[i].end;
			ikrule.contact = ikrules[i].contact;
			ikrule.drop = ikrules[i].drop;
			ikrule.top = ikrules[i].top;
			ikrule.endHeight = ikrules[i].endHeight;
			if (ikrules[i].szattachmentindex) ikrule.attachmentname = STRING_FROM_IDX(&ikrules[i], ikrules[i].szattachmentindex);

			// parse ikrule frames
			if (ikrules[i].sectionframes) {
				ikrule.ikruledata.resize(anim.numframes);
				auto sectioncount = static_cast<uint32_t>((float)(anim.numframes - 1) / (float)ikrules[i].sectionframes) + 1;
				auto* sectionindices = reinterpret_cast<int32_t*>((char*)&ikrules[i] + ikrules[i].compressedikerrorindex);

				uint32_t sectionbaseframe = 0;
				for (uint32_t section = 0; section < sectioncount; section++) {
					uint32_t sectionframes = GetSectionLength(anim.numframes, ikrules[i].sectionframes, section);
					auto* offsets = reinterpret_cast<int16_t*>((char*)&ikrules[i] + sectionindices[section]);

					for (uint32_t localframe = 0; localframe < sectionframes; localframe++) {
						uint32_t frame = sectionbaseframe + localframe;
						for (int idx = 0; idx < 3; idx++) {
							auto* panimvalue = reinterpret_cast<r5::anim::mstudioanimvalue_t*>((char*)offsets + offsets[idx]);
							if (idx < 3)
								RLE::ExtractAnimValue(localframe, panimvalue, ikrules[i].scale[idx], ikrule.ikruledata.pos[frame][idx]);
							else
								RLE::ExtractAnimValue(localframe, panimvalue, ikrules[i].scale[idx], ikrule.ikruledata.rot[frame][idx - 3]);

						}
					}
					sectionbaseframe += sectionframes;
				}
			}
			anim.ikrules.push_back(ikrule);
		}
	}
}

template<typename T>
void RLE::ParseIkrules_v12(const T* pAnimDesc, temp::animdesc_t& anim) {
	if (nullptr == pAnimDesc) PRINTANDTHROW(anim.name, "pAnimDesc is null");

	// ikrules
	if (pAnimDesc->numikrules && pAnimDesc->ikruleindex != 3 && pAnimDesc->ikruleindex != 5) {
		auto* ikrules = reinterpret_cast<r5::anim::v11::mstudioikrule_t*>((char*)pAnimDesc + OFFSET(pAnimDesc->ikruleindex));
		for (int i = 0; i < pAnimDesc->numikrules; i++) {
			temp::ikrule_t ikrule{};
			//ikrule.index = ikrules[i].index;
			ikrule.type = ikrules[i].type;
			ikrule.chain = ikrules[i].chain;
			ikrule.bone = ikrules[i].bone;
			ikrule.slot = ikrules[i].slot;
			ikrule.height = ikrules[i].height;
			ikrule.radius = ikrules[i].radius;
			ikrule.floor = ikrules[i].floor;
			ikrule.pos = ikrules[i].pos;
			ikrule.q = ikrules[i].q;
			for (int j = 0; j < 6; j++) ikrule.scale[j] = ikrules[i].scale[j];
			ikrule.sectionframes = ikrules[i].sectionframes;
			ikrule.iStart = ikrules[i].iStart;
			ikrule.start = ikrules[i].start;
			ikrule.peak = ikrules[i].peak;
			ikrule.tail = ikrules[i].tail;
			ikrule.end = ikrules[i].end;
			ikrule.contact = ikrules[i].contact;
			ikrule.drop = ikrules[i].drop;
			ikrule.top = ikrules[i].top;
			ikrule.endHeight = ikrules[i].endHeight;
			if (ikrules[i].szattachmentindex) ikrule.attachmentname = STRING_FROM_IDX(&ikrules[i], OFFSET(ikrules[i].szattachmentindex));

			// parse ikrule frames
			if (ikrules[i].sectionframes) {
				ikrule.ikruledata.resize(anim.numframes);
				auto sectioncount = static_cast<uint32_t>((float)(anim.numframes - 1) / (float)ikrules[i].sectionframes) + 1;
				auto* sectionindices = reinterpret_cast<uint16_t*>((char*)&ikrules[i] + OFFSET(ikrules[i].compressedikerrorindex));

				uint32_t sectionbaseframe = 0;
				for (uint32_t section = 0; section < sectioncount; section++) {
					uint32_t sectionframes = GetSectionLength(anim.numframes, ikrules[i].sectionframes, section);
					auto* offsets = reinterpret_cast<int16_t*>((char*)&ikrules[i] + OFFSET(sectionindices[section]));

					for (uint32_t localframe = 0; localframe < sectionframes; localframe++) {
						uint32_t frame = sectionbaseframe + localframe;
						for (int idx = 0; idx < 3; idx++) {
							auto* panimvalue = reinterpret_cast<r5::anim::mstudioanimvalue_t*>((char*)offsets + offsets[idx]);
							if (idx < 3)
								RLE::ExtractAnimValue(localframe, panimvalue, ikrules[i].scale[idx], ikrule.ikruledata.pos[frame][idx]);
							else
								RLE::ExtractAnimValue(localframe, panimvalue, ikrules[i].scale[idx], ikrule.ikruledata.rot[frame][idx - 3]);

						}
					}
					sectionbaseframe += sectionframes;
				}
			}
			anim.ikrules.push_back(ikrule);
		}
	}
}
template void RLE::ParseIkrules_v12<r5::anim::v11::mstudioanimdesc_t>(const r5::anim::v11::mstudioanimdesc_t*, temp::animdesc_t&);
template void RLE::ParseIkrules_v12<r5::anim::v12::mstudioanimdesc_t>(const r5::anim::v12::mstudioanimdesc_t*, temp::animdesc_t&);
template void RLE::ParseIkrules_v12<r5::anim::v121::mstudioanimdesc_t>(const r5::anim::v121::mstudioanimdesc_t*, temp::animdesc_t&);

void RLE::ParseFrameMovements_v10(const r5::anim::v10::mstudioanimdesc_t* pAnimDesc, temp::animdesc_t& anim) {
	if (nullptr == pAnimDesc) PRINTANDTHROW(anim.name, "pAnimDesc is null");

	if ((pAnimDesc->flags & r5::ANIM_FRAMEMOVEMENT) && pAnimDesc->framemovementindex) {
		auto* pFrameMovement = reinterpret_cast<r5::anim::v7::mstudioframemovement_t*>((char*)pAnimDesc + pAnimDesc->framemovementindex);
		auto sectioncount = static_cast<uint32_t>((float)(anim.numframes - 1) / (float)pFrameMovement->sectionframes) + 1;
		auto* sectionindices = reinterpret_cast<int32_t*>((char*)pFrameMovement + sizeof(r5::anim::v7::mstudioframemovement_t));

		temp::framemovement_t movement{};
		movement.scale = pFrameMovement->scale;
		movement.sectionframes = pFrameMovement->sectionframes;
		movement.movementdata.resize(anim.numframes);

		uint32_t sectionbaseframe = 0;
		for (uint32_t section = 0; section < sectioncount; section++) {
			uint32_t sectionframes = GetSectionLength(anim.numframes, pFrameMovement->sectionframes, section);
			auto* offsets = reinterpret_cast<int16_t*>((char*)pFrameMovement + sectionindices[section]);

			for (uint32_t localframe = 0; localframe < sectionframes; localframe++) {
				uint32_t frame = sectionbaseframe + localframe;
				for (int idx = 0; idx < 4; idx++) {
					if (offsets[idx]) {
						auto* panimvalue = reinterpret_cast<r5::anim::mstudioanimvalue_t*>((char*)offsets + offsets[idx]);
						RLE::ExtractAnimValue(localframe, panimvalue, pFrameMovement->scale[idx], movement.movementdata[frame][idx]);
					}
				}
			}
			sectionbaseframe += sectionframes;
		}
		anim.movement = movement;
	}
}

template<typename T>
void RLE::ParseFrameMovements_v12(const T* pAnimDesc, temp::animdesc_t& anim) {
	if (nullptr == pAnimDesc) PRINTANDTHROW(anim.name, "pAnimDesc is null");

	if ((pAnimDesc->flags & r5::ANIM_FRAMEMOVEMENT) && pAnimDesc->framemovementindex) {

		auto* pFrameMovement = reinterpret_cast<r5::anim::v7::mstudioframemovement_t*>((char*)pAnimDesc + OFFSET(pAnimDesc->framemovementindex));
		auto sectioncount = static_cast<uint32_t>((float)(anim.numframes - 1) / (float)pFrameMovement->sectionframes) + 1;
		auto* sectionindices = reinterpret_cast<uint16_t*>((char*)pFrameMovement + sizeof(r5::anim::v7::mstudioframemovement_t));

		temp::framemovement_t movement{};
		movement.scale = pFrameMovement->scale;
		movement.sectionframes = pFrameMovement->sectionframes;
		movement.movementdata.resize(anim.numframes);

		// RLE
		if (!(pAnimDesc->flags & ANIM_DATAPOINT)) {
			uint32_t sectionbaseframe = 0;
			for (uint32_t section = 0; section < sectioncount; section++) {
				uint32_t sectionframes = GetSectionLength(anim.numframes, pFrameMovement->sectionframes, section);
				auto* offsets = reinterpret_cast<int16_t*>((char*)pFrameMovement + OFFSET(sectionindices[section]));

				for (uint32_t localframe = 0; localframe < sectionframes; localframe++) {
					uint32_t frame = sectionbaseframe + localframe;
					for (int idx = 0; idx < 4; idx++) {
						if (offsets[idx]) {
							auto* panimvalue = reinterpret_cast<r5::anim::mstudioanimvalue_t*>((char*)offsets + offsets[idx]);
							RLE::ExtractAnimValue(localframe, panimvalue, pFrameMovement->scale[idx], movement.movementdata[frame][idx]);
						}
					}
				}
				sectionbaseframe += sectionframes;
			}
			anim.movement = movement;
		}
		// DP
		else {
			printf("[!] Error: Animation data cannot be decompressed.");
			throw std::runtime_error("[!] Error: Animation data cannot be decompressed.");
		}
	}
}
template void RLE::ParseFrameMovements_v12<r5::anim::v11::mstudioanimdesc_t>(const r5::anim::v11::mstudioanimdesc_t*, temp::animdesc_t&);
template void RLE::ParseFrameMovements_v12<r5::anim::v12::mstudioanimdesc_t>(const r5::anim::v12::mstudioanimdesc_t*, temp::animdesc_t&);
template void RLE::ParseFrameMovements_v12<r5::anim::v121::mstudioanimdesc_t>(const r5::anim::v121::mstudioanimdesc_t*, temp::animdesc_t&);

/* TODO: rewrite this func to support more than 3 types of compression */
template <typename T>
void WriteCompressedAnim(char*& pData, const std::vector<T>& rawdata, temp::animblock_t c, int axis, float scale) {
	int framerange = c.endframe - c.startframe;

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
static void FindConstantRanges(const std::vector<T>& raw, int axis, float eps, std::vector<temp::animblock_t>& out, uint32_t start, uint32_t end)
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
void WriteAnimData(char*& pData, const std::vector<T>& rawdata, uint32_t startframe, uint32_t endframe, int axis, float scale) {
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
