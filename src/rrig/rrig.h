#include <iostream>  
#include <fstream>
#include <vector>
#include <filesystem>

std::string ConvertMDL_49_RRIG(char* mdl_buffer, std::string output_dir, std::string filename, std::string override_path, std::vector<std::pair<std::pair<int, int>, int>>& nodedata);

std::string ConvertMDL_53_RRIG(char* mdl_buffer, std::string output_dir, std::string filename, std::string override_path, std::vector<std::pair<std::pair<int, int>, int>>& nodedata);