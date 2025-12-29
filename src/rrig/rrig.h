#pragma once
#include <pch.h>

void ParseRRIG_v121(char* buffer, temp::rig_t& rig);
void ParseRRIG_v13(char* buffer, temp::rig_t& rig);
void ParseRRIG_v14(char* buffer, temp::rig_t& rig);
void ParseRRIG_v16(char* buffer, temp::rig_t& rig);
void ParseRRIG_v17(char* buffer, temp::rig_t& rig);
void WriteRRIG_v8(std::string output_dir, const temp::rig_t& rig);
void SetFlagForDescendants(temp::rig_t& rig, int parentIdx, int flag);