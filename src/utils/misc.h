#pragma once
#include <pch.h>

#define ARG_BOOL(opt, var) \
    if (arg == opt) { var = true; continue; }

#define ARG_INT(opt, var, err) \
    if (arg == opt) { \
        if (++i >= argc) { \
            printf("%s\n", err); \
            return 1; \
        } \
        var = atoi(argv[i]); \
        continue; \
    }

#define ARG_VAL(opt, var, err) \
    if (arg == opt) { \
        if (++i >= argc) { \
            printf("%s\n", err); \
            return 1; \
        } \
        var = argv[i]; \
        continue; \
    }

using ParseRRIGFn = void(*)(char*, temp::rig_t&);
using ParseRSEQFn = void(*)(std::string, temp::rig_t&);

struct ParserSet {
	ParseRRIGFn rrig;
	ParseRSEQFn rseq;
};

static const std::unordered_map<std::string, ParserSet> Parsers = {
	{"9",  {ParseRRIG_v121, ParseRSEQ_v10}},
	{"10", {ParseRRIG_v121, ParseRSEQ_v10}},
	{"11", {ParseRRIG_v121, ParseRSEQ_v10}},
	{"12", {ParseRRIG_v121, ParseRSEQ_v10}},
	{"13", {ParseRRIG_v13,  ParseRSEQ_v10}},
	{"14", {ParseRRIG_v14,  ParseRSEQ_v10}},
	{"15", {ParseRRIG_v16,  ParseRSEQ_v11}},
	{"16", {ParseRRIG_v16,  ParseRSEQ_v11}},
	{"17", {ParseRRIG_v16,  ParseRSEQ_v11}},
	{"18", {ParseRRIG_v16,  ParseRSEQ_v11}},
	{"19", {ParseRRIG_v16,  ParseRSEQ_v11}},
	{"20", {ParseRRIG_v17,  ParseRSEQ_v11}},
	{"21", {ParseRRIG_v17,  ParseRSEQ_v11}},
	{"22", {ParseRRIG_v17,  ParseRSEQ_v11}},
	{"23", {ParseRRIG_v17,  ParseRSEQ_v11}},
};

void GatherRigPaths(std::string in_dir, std::filesystem::directory_entry dir, std::vector<temp::rig_t>& rrig);
void PrintRepakEntries(temp::rig_t& rig);
void PrintSets(std::unordered_map<std::string, ParserSet> &Parsers, const char* text = "");
