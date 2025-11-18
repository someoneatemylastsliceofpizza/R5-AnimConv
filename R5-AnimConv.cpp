#include "src/pch.h"
#include <rrig/rrig.h>
#include <rseq/rseq.h>
#include <mdl/mdl.h>

static void PrintRepakEntries(std::string& rig_name, std::vector<std::string>& sequence_names) {
	if (is_no_entry) return;
	std::replace(rig_name.begin(), rig_name.end(), '\\', '/');
	for (auto& name : sequence_names) std::replace(name.begin(), name.end(), '\\', '/');

	printf("    {\n     \"_type\": \"arig\",\n     \"_path\" : \"%s\",\n     \"$sequences\" : [\n", rig_name.c_str());
	for (int i = 0; i < sequence_names.size(); i++) {
		printf("        \"%s\"%s\n", sequence_names.at(i).c_str(), (i == sequence_names.size() - 1) ? "\n      ]\n    }," : ", ");
	}

	if (sequence_names.empty()) {
		printf("\n      ]\n    },");
	}
}

static void PrintSets(std::unordered_set<uint32_t> set, const char* text = "") {
	printf("%s [ ", text);
	for (auto& v : set) { printf("%d ", v); }
	printf("]\n");
}

int main(int argc, char* argv[]) {
	std::string input_mdl;
	std::string override_rseq_path;
	std::string override_rrig_path;

	// -verbose : Enable Verbose
	// -ne : No entry output
	// -rp <override_rrig_path> : Override internal Rrig path //e.g. -rp "animrig/titans/buddy"
	// -sp <override_rseq_path> : Override internal Rseq path //e.g. -sp "animseq/titans/buddy_sp" to avoid autolayer guid mismatch

	std::string usage = "Usage: \n \
  Mdl  mode: R5-AnimConv.exe <model.mdl> [-verbose] [-ne] [-rp <override_rrig_path>] [-sp <override_rseq_path>]\n \
  Rseq mode: R5-AnimConv.exe <parent directory> [-verbose] [-ne] [-i <in assets season>] [-o <out assets season>]\n";

	if (argc < 2) {
		std::cerr << usage;
		system("pause");
		return 1;
	}

	input_mdl = argv[1];

	for (int i = 2; i < argc; ++i) {
		std::string arg = argv[i];
		if (arg == "-verbose") _enable_verbose = true;
		else if (arg == "-ne") is_no_entry = true;
		else if (arg == "-rp") {
			if (i + 1 < argc) {
				override_rrig_path = argv[++i];
			}
			else {
				std::cerr << "Error: -rp option requires a path argument.\n";
				return 1;
			}
		}
		else if (arg == "-sp") {
			if (i + 1 < argc) {
				override_rseq_path = argv[++i];
			}
			else {
				std::cerr << "Error: -sp option requires a path argument.\n";
				return 1;
			}
		}
		else {
			std::cerr << "Unknown option: " << arg << "\n" << usage;
			return 1;
		}
	}

	std::ifstream mdl_stream(input_mdl, std::ios::binary);
	std::filesystem::path file_path = std::filesystem::absolute(input_mdl);
	std::string output_dir = file_path.parent_path().string();

	if (!std::filesystem::exists(input_mdl)) {
		std::cerr << "Error: Input file does not exist." << std::endl;
		return 1;
	}

	verbose("Reading: %s...\n", input_mdl.c_str());

	int magic = 0;
	mdl_stream.read(reinterpret_cast<char*>(&magic), sizeof(int));
	if (magic != 'TSDI') {
		std::cerr << "Error: Input file is not a MDL file." << std::endl;
		return 1;
	}

	int mdl_version = 0;
	mdl_stream.read(reinterpret_cast<char*>(&mdl_version), sizeof(int));

	uintmax_t mdlFileSize = std::filesystem::file_size(input_mdl);
	std::vector<char> buffer(mdlFileSize, 0);
	mdl_stream.seekg(0, std::ios::beg);
	mdl_stream.read(buffer.data(), mdlFileSize);
	mdl_stream.close();

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

	printf("\n\nWriting %s\n", rig.name.c_str());
	WriteRRIG_v8(output_dir, rig);
	printf("Writing sequences\n");
	WriteRSEQ_v7(rig);

	if (!is_no_entry) printf("\n\nRePak Entries:\n");
	PrintRepakEntries(rig.name, rig.rseqpaths);
	verbose("[+] Done!\n");
	system("pause");
	return 0;
}