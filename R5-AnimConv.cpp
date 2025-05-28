
#include <iostream>  
#include <fstream>
#include <mdl/rrig.h>
#include <mdl/rseq.h>

int main(int argc, char* argv[]) {  
	if (argc != 2 ) {
		std::cerr << "Usage: " << argv[0] << " <input.mdl>" << std::endl;  
		return 1;  
	}  

	std::string input_mdl = argv[1];
	std::ifstream mdl_stream(input_mdl, std::ios::binary);


	std::filesystem::path file_path(input_mdl);
	std::string filename = file_path.filename().string().substr(0, file_path.filename().string().find_last_of("."));
	std::string output_dir = file_path.parent_path().string();;

	if (!std::filesystem::exists(input_mdl)) {
		std::cerr << "Error: Input file does not exist." << std::endl;
		return 1;
	}

	printf("Reading: %s...\n", input_mdl.c_str());

	int magic = 0;
	mdl_stream.read(reinterpret_cast<char*>(&magic), sizeof(int));
	if (magic != 'TSDI') {
		std::cerr << "Error: Input file is not a MDL file." << std::endl;
		return 1;
	}

	int mdl_version = 0;
	mdl_stream.read(reinterpret_cast<char*>(&mdl_version), sizeof(int));
	printf("MDL version %d\n", mdl_version);

	std::vector<std::string> sequence_names;
	std::string rig_name = "CANNOT LOAD RRIG NAME";
	                             //entry exit  seq            
	std::vector<std::pair<std::pair<int, int>, int>> nodedata;

	if (mdl_version == 49) {
		printf("Starting...\n");

		uintmax_t mdlFileSize = std::filesystem::file_size(input_mdl);

		//RSEQ
		mdl_stream.seekg(0, std::ios::beg);
		char* buffer = new char[mdlFileSize];
		mdl_stream.read(buffer, mdlFileSize);
		sequence_names = ConvertMDL_RSEQ(buffer, output_dir, filename, nodedata);
		//RRIG
		mdl_stream.seekg(0, std::ios::beg);
		buffer = new char[mdlFileSize];
		mdl_stream.read(buffer, mdlFileSize);
		rig_name = ConvertMDL_RRIG(buffer, output_dir, filename, nodedata);
		delete[] buffer;

		printf("\n//----------------------------------------------------------------------\n");
		printf("{\n     \"_type\": \"arig\",\n     \"_path\" : \"%s\",\n     \"$sequences\" : [\n", rig_name.c_str());
		for (int i = 0; i < sequence_names.size(); i++)
			printf("        \"%s\"%s\n", sequence_names.at(i).c_str(), (i == sequence_names.size() - 1) ? "\n      ]\n    }\n" : ", ");
	}
	else 
		printf("Failed: This MDL version %d does not support yet, Only v49 is supported.\n", mdl_version);
	
	mdl_stream.close();

	system("pause");
	return 0;  
	}