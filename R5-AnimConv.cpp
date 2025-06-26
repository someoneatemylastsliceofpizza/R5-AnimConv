
#include <iostream>  
#include <fstream>
#include <rrig/rrig.h>
#include <rseq/rseq.h>
#include <utils/print.h>

int main(int argc, char* argv[]) {
	std::string input_mdl;
	std::string override_rseq_path;
	std::string override_rrig_path;

	// -nv : No verbose output
	// -ne : No entry output
	// -rp <override_rrig_path> : Override internal Rrig path //e.g. -rp "animrig/titans/buddy"
	// -sp <override_rseq_path> : Override internal Rseq path //e.g. -sp "animseq/titans/buddy_sp" to avoid autolayer guid mismatch
	
	if (argc < 2) {
		std::cerr << "Usage: R5-AnimConv.exe <path-to-your-model.mdl> [-nv] [-ne] [-rp <override_rrig_path>] [-sp <override_rseq_path>]" << std::endl;
		system("pause");
		return 1;
	}

	input_mdl = argv[1];

	for (int i = 2; i < argc; ++i) {
		std::string arg = argv[i];
		if (arg == "-nv") is_no_verbose = true;
		else if (arg == "-ne") is_no_entry = true;
		else if (arg == "-rp") {
			if (i + 1 < argc) {
				override_rrig_path = argv[++i];
			} else {
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
			std::cerr << "Unknown option: " << arg << "\nUsage: R5-AnimConv.exe <path-to-your-model.mdl> [-nv] [-ne] [-rp <override_rrig_path>] [-sp <override_rseq_path>]" << std::endl;
			return 1;
		}
	}

	std::ifstream mdl_stream(input_mdl, std::ios::binary);

	std::filesystem::path file_path = std::filesystem::absolute(input_mdl);
	std::string filename = file_path.filename().string().substr(0, file_path.filename().string().find_last_of("."));
	std::string output_dir = file_path.parent_path().string();;

	if (!std::filesystem::exists(input_mdl)) {
		std::cerr << "Error: Input file does not exist." << std::endl;
		return 1;
	}

	print("Reading: %s...\n", input_mdl.c_str());

	int magic = 0;
	mdl_stream.read(reinterpret_cast<char*>(&magic), sizeof(int));
	if (magic != 'TSDI') {
		std::cerr << "Error: Input file is not a MDL file." << std::endl;
		return 1;
	}

	int mdl_version = 0;
	mdl_stream.read(reinterpret_cast<char*>(&mdl_version), sizeof(int));

	std::vector<std::string> sequence_names;
	std::string rig_name = "CANNOT LOAD RRIG NAME";
	std::vector<std::pair<std::pair<int, int>, int>> nodedata;

	uintmax_t mdlFileSize = std::filesystem::file_size(input_mdl);
	char* buffer = new char[mdlFileSize];

	print("Starting...\n\n");

	switch (mdl_version) {
	case 49:
		//RSEQ
		mdl_stream.seekg(0, std::ios::beg);
		mdl_stream.read(buffer, mdlFileSize);
		sequence_names = ConvertMDL_49_RSEQ(buffer, output_dir, filename, override_rseq_path, nodedata);
		//RRIG
		mdl_stream.seekg(0, std::ios::beg);
		mdl_stream.read(buffer, mdlFileSize);
		rig_name = ConvertMDL_49_RRIG(buffer, output_dir, filename, override_rrig_path, nodedata);
		break;
	case 53:
		//RSEQ
		mdl_stream.seekg(0, std::ios::beg);
		mdl_stream.read(buffer, mdlFileSize);
		sequence_names = ConvertMDL_53_RSEQ(buffer, output_dir, filename, override_rseq_path, nodedata);
		//RRIG
		mdl_stream.seekg(0, std::ios::beg);
		mdl_stream.read(buffer, mdlFileSize);
		rig_name = ConvertMDL_53_RRIG(buffer, output_dir, filename, override_rrig_path, nodedata);
		break;
	default:
		printf("Failed: This MDL v%d does not support yet, Only v49 and v53 are supported.\n", mdl_version);
	}
	
	if (!is_no_entry) {
		printf("{\n     \"_type\": \"arig\",\n     \"_path\" : \"%s\",\n     \"$sequences\" : [\n", rig_name.c_str());
		for (int i = 0; i < sequence_names.size(); i++)
			printf("        \"%s\"%s\n", sequence_names.at(i).c_str(), (i == sequence_names.size() - 1) ? "\n      ]\n    }\n" : ", ");
		system("pause");
	}

	print("All Done!\n");

	delete[] buffer;
	mdl_stream.close();
	return 0;  
}