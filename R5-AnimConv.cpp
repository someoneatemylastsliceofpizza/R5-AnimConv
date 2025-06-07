
#include <iostream>  
#include <fstream>
#include <mdl/rrig.h>
#include <mdl/rseq.h>
#include <utils/print.h>

int main(int argc, char* argv[]) {  
	std::string input_mdl;

	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " <input.mdl> [-nv] [-ne]" << std::endl;
		return 1;
	}

	input_mdl = argv[1];

	for (int i = 2; i < argc; ++i) {
		std::string arg = argv[i];
		if (arg == "-nv") is_no_verbose = true;
		else if (arg == "-ne") is_no_entry = true;
		else {
			std::cerr << "Unknown option: " << arg << "\nUsage: " << argv[0] << " <input.mdl> [-nv] [-ne]" << std::endl;
			return 1;
		}
	}

	std::ifstream mdl_stream(input_mdl, std::ios::binary);

	std::filesystem::path file_path(input_mdl);
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

	if (mdl_version == 49) {
		print("Starting...\n\n");
		
		//RSEQ
		mdl_stream.seekg(0, std::ios::beg);
		mdl_stream.read(buffer, mdlFileSize);
		sequence_names = ConvertMDL_49_RSEQ(buffer, output_dir, filename, nodedata);
		//RRIG
		mdl_stream.seekg(0, std::ios::beg);
		mdl_stream.read(buffer, mdlFileSize);
		rig_name = ConvertMDL_49_RRIG(buffer, output_dir, filename, nodedata);
	}
	else 
		printf("Failed: This MDL version %d does not support yet, Only v49 is supported.\n", mdl_version);
	
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