#include <iostream>  
#include <fstream>
#include <utils/fileio.cpp>

std::string ConvertMDL_RRIG(char* mdl_buffer, std::string output_dir, std::string filename, std::vector<std::pair<std::pair<int, int>, int>>& nodedata);