#include <iostream>  
#include <fstream>
#include <vector>
#include <filesystem>

std::vector<std::string> ConvertMDL_49_RSEQ(char* mdl_buffer, std::string output_dir, std::string filename, std::string override_path, std::vector<std::pair<std::pair<int, int>, int>>& nodedata);

std::vector<std::string> ConvertMDL_53_RSEQ(char* mdl_buffer, std::string output_dir, std::string filename, std::string override_path, std::vector<std::pair<std::pair<int, int>, int>>& nodedata);