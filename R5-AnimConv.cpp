#include <pch.h>

#include <rrig/rrig.h>
#include <rseq/rseq.h>
#include <mdl/mdl.h>
#include <utils/rson_parser.h>
#include <utils/misc.h>

int main(int argc, char* argv[]) {
	std::string input_mdl;
	std::string override_rseq_path;
	std::string override_rrig_path;
	std::string in_season = "23";
	std::string out_season = "3";
	bool bSkipEvents = false;
	bool bNoPause = false;

	std::string usage = "Usage: \n" \
		"  Mdl  mode : R5-AnimConv.exe <model.mdl> [-verbose] [-ne] [-rp <override_rrig_path>] [-sp <override_rseq_path>]\n" \
		"  Rseq mode : R5-AnimConv.exe <parent directory> [-i <in season>] [-o <out season>] [-verbose] [-ne]\n";

	// -i <in_season>  : Input assets season (default: 23)
	// -o <out_season> : Output assets season (default: 3)
	// -verbose    : Verbose outputs
	// -ne         : No RePak Entries outputs
	// -skipevents : Skip events for debugging
	// -nopause    : No pause at the end of execution
	// -rp <override_rrig_path> : Override internal rrig path //e.g. -rp "animrig/titans/buddy"
	// -sp <override_rseq_path> : Override internal rseq path //e.g. -sp "animseq/titans/buddy_sp" to avoid autolayer guid mismatch

	if (argc < 2) {
		printf("%s", usage.c_str());
		system("pause");
		return 1;
	}

	input_mdl = argv[1];
	for (int i = 2; i < argc; ++i) {
		std::string arg = argv[i];
		ARG_VAL("-i", in_season, "[!] Error: -i requires input assets season.\n");
		ARG_VAL("-o", out_season, "[!] Error: -o requires output assets season.\n");
		ARG_BOOL("-verbose", _enable_verbose);
		ARG_BOOL("-ne", _enable_no_entry);
		ARG_BOOL("-skipevents", bSkipEvents);
		ARG_BOOL("-nopause", bNoPause);
		ARG_VAL("-rp", override_rrig_path, "[!] Error: -rp requires a path argument.\n");
		ARG_VAL("-sp", override_rseq_path, "[!] Error: -sp requires a path argument.\n");

		printf("Unknown option: %s \n%s", arg.c_str(), usage.c_str());
		return 1;
	}

	/* RSEQ TO RSEQ */
	if (std::filesystem::exists(input_mdl)) {
		if (std::filesystem::is_regular_file(input_mdl) && (std::filesystem::path(input_mdl).extension() == ".mdl")) goto CONVERT_MDL;

		auto parser = Parsers.find(in_season);
		if (parser == Parsers.end()) {
			printf("[!] Error: Unsupported assets version.\n");
			return 1;
		}

		std::vector<temp::rig_t> rigs;
		std::string in_dir = std::filesystem::is_regular_file(input_mdl) ? std::filesystem::path(input_mdl).parent_path().string() : input_mdl;
		std::filesystem::directory_entry entry = std::filesystem::directory_entry(in_dir);

		/* GATHER PATHS */
		GatherRigPaths(in_dir, entry, rigs);
		for (auto& rig : rigs) {
			std::filesystem::path rigpath = rig.rrigpath;
			std::filesystem::path rsonpath = rigpath.parent_path().string() + "\\" + rigpath.stem().string() + ".rson";
			if (std::filesystem::is_regular_file(rsonpath)) {
				auto data = parse_rson(rsonpath.string());
				rig.rsonpath = rsonpath.string();
				rig.rseqpaths = data["seqs"];
				rig.rigpaths = data["rigs"];
				//rig.materialpaths = data["matl"];
			}
		}

		if (rigs.empty()) {
			printf("[!] Error: No rrig files found in the specified directory.\n");
			return 1;
		}

		/* PARSE */
		for (auto& rig : rigs) {
			if (rig.rsonpath.empty()) {
				printf("[!] Skipping: no .rson was founded for %s\n", rig.rrigpath.c_str());
				continue;
			}

			/* PARSE RRIG */ {
				verbose("\nParsing rrig %s...\n", rig.rrigpath.c_str());
				uint32_t rigFileSize = (uint32_t)std::filesystem::file_size(rig.rrigpath);
				std::ifstream rrig_stream(rig.rrigpath, std::ios::binary);
				rrig_stream.seekg(0, std::ios::beg);
				std::vector<char> buffer(rigFileSize);
				rrig_stream.read(buffer.data(), rigFileSize);
				rrig_stream.close();

				parser->second.rrig(buffer.data(), rig);
				std::replace(rig.name.begin(), rig.name.end(), '\\', '/');

				if (rig.rseqpaths.empty()) {
					printf("[!] Warning: No .rseq paths found in .rson for %s\n", rig.name.c_str());
					continue;
				}
			}
			rig.sequences.reserve(rig.rseqpaths.size());

			/* PARSE RSEQ */ {
				printf("Parsing Sequences for %s\n", rig.name.c_str());
				parser->second.rseq(in_dir, rig);
			}
		}
		printf("\n");

		/* WRITE */
		for (auto& rig : rigs) {
			if (std::filesystem::path(rig.name).extension() == ".rmdl") {
				printf("Skipping %s\n", rig.name.c_str());
				goto WRITE_RSEQ;
			}

			printf("Writing %s\n", rig.name.c_str());
			WriteRRIG_v8(in_dir + "/conv", rig);

		WRITE_RSEQ:
			if (rig.rseqpaths.empty()) {
				printf("[!] Skipping: No RSEQ path was founded in RSON for %s\n", rig.name.c_str());
				continue;
			}

			printf("Writing rseqs for %s\n", rig.name.c_str());
			if (out_season == "3") {
				WriteRSEQ_v7(rig, bSkipEvents);
			}
			else {
				printf("[!] Error: Only rseq v7 is supported now\n");
				return 1;
			}
		}

		/* PRINT REPAK ENTRIES */
		if (!_enable_no_entry) printf("\n\nRePak Entries:\n");
		for (auto& rig : rigs)  PrintRepakEntries(rig);
		verbose("[+] Succeeded!\n"); 
		if (!bNoPause) system("pause");
		return 0;
	}

CONVERT_MDL:
	/* MDL TO RRIG/RSEQ */
	std::ifstream mdl_stream(input_mdl, std::ios::binary);
	std::filesystem::path file_path = std::filesystem::absolute(input_mdl);
	std::string output_dir = file_path.parent_path().string();
	verbose("Reading: %s...\n", input_mdl.c_str());

	if (!std::filesystem::exists(input_mdl)) {
		printf("[!] Error: Input file does not exist.\n");
		return 1;
	}

	int magic = 0;
	mdl_stream.read(reinterpret_cast<char*>(&magic), sizeof(int));
	if (magic != 'TSDI') {
		printf("[!] Error: Input file is not a MDL file.\n");
		return 1;
	}

	int mdl_version = 0;
	mdl_stream.read(reinterpret_cast<char*>(&mdl_version), sizeof(int));

	uint32_t mdlFileSize = (uint32_t)std::filesystem::file_size(input_mdl);
	std::vector<char> buffer(mdlFileSize, 0);
	mdl_stream.seekg(0, std::ios::beg);
	mdl_stream.read(buffer.data(), mdlFileSize);
	mdl_stream.close();

	/* PARSE MDL */
	printf("Parsing %s\n", file_path.filename().string().c_str());
	temp::rig_t rig;
	switch (mdl_version) {
	case 49:
		ParseMDL_v49(buffer.data(), rig, output_dir, override_rrig_path, override_rseq_path);
		break;
	case 53:
		ParseMDL_v53(buffer.data(), rig, output_dir, override_rrig_path, override_rseq_path);
		break;
	default:
		printf("Failed: This MDL v%d does not support yet, Only v49 and v53 are supported.\n", mdl_version);
	}

	/* WRITE RRIG/RSEQ */
	printf("\n\nWriting %s\n", rig.name.c_str());
	WriteRRIG_v8(output_dir, rig);
	printf("Writing sequences\n");
	WriteRSEQ_v7(rig);

	/* PRINT REPAK ENTRIES */
	if (!_enable_no_entry) printf("\n\nRePak Entries:\n");
	PrintRepakEntries(rig);
	verbose("[+] Succeeded!\n");
	if (!bNoPause) system("pause");
	return 0;
}