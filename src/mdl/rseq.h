#include <iostream>  
#include <fstream>
#include <vector>
#include <filesystem>

std::vector<std::string> ConvertMDL_RSEQ(char* mdl_buffer, std::string output_dir, std::string filename, std::vector<std::pair<std::pair<int, int>, int>>& nodedata);