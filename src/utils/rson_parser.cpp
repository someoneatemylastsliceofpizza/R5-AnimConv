#include <pch.h>
#include <utils/rson_parser.h>

std::unordered_map<std::string, std::vector<std::string>> parse_rson(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) throw std::runtime_error("Failed to open file");

    std::string content((std::istreambuf_iterator<char>(file)), {});
    std::unordered_map<std::string, std::vector<std::string>> result;

    // remove // comments
    std::string cleaned;
    cleaned.reserve(content.size());
    bool in_comment = false;
    for (size_t i = 0; i < content.size(); ++i) {
        if (!in_comment && i + 1 < content.size() && content[i] == '/' && content[i + 1] == '/') {
            in_comment = true;
            ++i; // skip second '/'
            continue;
        }
        if (in_comment && (content[i] == '\n' || content[i] == '\r')) {
            in_comment = false;
        }
        if (!in_comment) cleaned += content[i];
    }

    std::string_view view(cleaned);
    size_t pos = 0;
    while (pos < view.size()) {
        while (pos < view.size() && isspace((unsigned char)view[pos])) pos++;
        if (pos >= view.size()) break;

        // parse key
        size_t key_start = pos;
        while (pos < view.size() && view[pos] != ':') pos++;
        if (pos >= view.size()) break;
        std::string key(view.substr(key_start, pos - key_start));
        key.erase(key.find_last_not_of(" \t\r\n") + 1);
        key.erase(0, key.find_first_not_of(" \t\r\n"));

        // expect :[
        pos++;
        while (pos < view.size() && isspace((unsigned char)view[pos])) pos++;
        if (pos >= view.size() || view[pos] != '[') continue;
        pos++;

        // collect values until ]
        std::vector<std::string> vals;
        size_t val_start = pos;
        while (pos < view.size() && view[pos] != ']') {
            if (view[pos] == '\n' || view[pos] == '\r') {
                std::string val(view.substr(val_start, pos - val_start));
                val.erase(val.find_last_not_of(" \t\r\n") + 1);
                val.erase(0, val.find_first_not_of(" \t\r\n"));
                if (!val.empty()) vals.push_back(val);
                val_start = pos + 1;
            }
            pos++;
        }

        // handle last value before ]
        if (val_start < pos) {
            std::string val(view.substr(val_start, pos - val_start));
            val.erase(val.find_last_not_of(" \t\r\n") + 1);
            val.erase(0, val.find_first_not_of(" \t\r\n"));
            if (!val.empty()) vals.push_back(val);
        }

        result.emplace(std::move(key), std::move(vals));
        pos++;
    }

    return result;
}