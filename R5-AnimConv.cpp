
#include <iostream>  
#include <fstream>
#include <utils/fileio.cpp>
#include <mdl/rrig.h>
#include <mdl/rseq.h>

int main(int argc, char* argv[]) {  
  if (argc != 2) {
      std::cerr << "Usage: " << argv[0] << " <input.mdl>" << std::endl;  
      return 1;  
  }  

  std::string input_mdl = argv[1];

  if (input_mdl.substr(input_mdl.find_last_of('.')) != ".mdl") {  
      std::cerr << "Error: Input file must have a .mdl extension." << std::endl;  
      return 1;  
  } 
  std::filesystem::path file_path(input_mdl);
  std::string filename = file_path.filename().string().substr(0, file_path.filename().string().find_last_of("."));
  std::string output_dir = file_path.parent_path().string();

  if (!std::filesystem::exists(input_mdl)){
	  std::cerr << "Error: Input file does not exist." << std::endl;
	  return 1;
  }

  StreamIO mdl_stream;
  mdl_stream.open(input_mdl, eStreamIOMode::Read);

  printf("Reading: %s...\n", input_mdl.c_str());

  if (mdl_stream.read<int>() != 'TSDI') {
	  std::cerr << "Error: Input file is not a MDL file." << std::endl;
	  return 1;
  }

  int mdl_version = mdl_stream.read<int>();
  printf("MDL version %d\n", mdl_version);
  if (mdl_version == 49) {
	  printf("Starting convert...\n");

	  uintmax_t mdlFileSize = std::filesystem::file_size(input_mdl);
	  //RRIG
	  mdl_stream.seek(0, std::ios::beg);
	  char* buffer = new char[mdlFileSize];
	  mdl_stream.R()->read(buffer, mdlFileSize);
      ConvertMDL_RRIG(buffer, output_dir, filename);
	  //RSEQ
	  mdl_stream.seek(0, std::ios::beg);
	  buffer = new char[mdlFileSize];
	  mdl_stream.R()->read(buffer, mdlFileSize);
	  ConvertMDL_RSEQ(buffer, output_dir, filename);
	  delete[] buffer;
  }
  else {
	  printf("Failed: This MDL version %d does not support yet.\n", mdl_version);
  }

  mdl_stream.close();

  return 0;  
}