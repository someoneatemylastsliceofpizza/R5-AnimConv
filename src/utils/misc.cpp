#include <pch.h>

void GatherRigPaths(std::string in_dir, std::filesystem::directory_entry dir, std::vector<temp::rig_t>& rrig) {
	if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir)) {
		std::cerr << "[!] Error: directory does not exist.\n";
		return;
	}

	for (const auto& entry : std::filesystem::directory_iterator(dir)) {
		std::string e = entry.path().string();
		if (entry.is_regular_file()) {
			std::string ext = entry.path().extension().string();
			if (ext == ".rrig" || ext == ".rmdl") {
				std::filesystem::path relative_path = std::filesystem::relative(entry.path(), in_dir);
				auto first_dir = relative_path.begin();

				if (first_dir->string() != "conv") {
					temp::rig_t r{};
					r.rrigpath = entry.path().string();
					rrig.push_back(r);
				}
			}
		}
		else if (entry.is_directory()) {
			GatherRigPaths(in_dir, entry, rrig);
		}
	}
}

void PrintRepakEntries(temp::rig_t& rig) {
	if (_enable_no_entry) return;
	std::replace(rig.name.begin(), rig.name.end(), '\\', '/');
	for (auto& name : rig.rseqpaths) std::replace(name.begin(), name.end(), '\\', '/');
	for (auto& name : rig.rigpaths) std::replace(name.begin(), name.end(), '\\', '/');

	std::string asset_type = "arig";
	if (std::filesystem::path(rig.name).extension().string() == ".rmdl") asset_type = "mdl_";

	printf("    {\n      \"_type\": \"%s\",\n      \"_path\" : \"%s\"", asset_type.c_str(), rig.name.c_str());
	if (rig.rigpaths.size()) {
		printf(",\n      \"$animrigs\" : [\n");
		for (int i = 0; i < rig.rigpaths.size(); i++) {
			printf("        \"%s\"%s", rig.rigpaths.at(i).c_str(), (i == rig.rigpaths.size() - 1) ? "\n      ]" : ", \n");
		}
	}

	if (rig.rseqpaths.size()) {
		printf(",\n      \"$sequences\" : [\n");
		for (int i = 0; i < rig.rseqpaths.size(); i++) {
			printf("        \"%s\"%s", rig.rseqpaths.at(i).c_str(), (i == rig.rseqpaths.size() - 1) ? "\n      ]" : ", \n");
		}
	}
	printf("\n    },\n");
}

//void PrintSets(std::unordered_map<std::string, ParserSet> &Parsers, const char* text = "") {
//	printf("%s [ ", text);
//	for (auto& v : Parsers) {
//		printf("%s ", v.first);
//	}
//	printf("]\n");
//}
