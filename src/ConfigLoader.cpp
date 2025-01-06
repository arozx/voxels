#include "ConfigLoader.h"
#include <fstream>
#include <iostream>
#include <sstream>

bool ConfigLoader::load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    std::string line, section;
    while (std::getline(file, line)) {
        // Trim whitespace
        line = trim(line);

        // Skip comments and empty lines
        if (line.empty() || line[0] == ';' || line[0] == '#') continue;

        // Handle sections [Section]
        if (line[0] == '[' && line.back() == ']') {
            section = line.substr(1, line.size() - 2);
            section = trim(section);
        }
        // Handle key-value pairs
        else {
            auto delimiterPos = line.find('=');
            if (delimiterPos == std::string::npos) continue;

            std::string key = trim(line.substr(0, delimiterPos));
            std::string value = trim(line.substr(delimiterPos + 1));

            // Store in the map, including section if present
            if (!section.empty()) {
                key = section + "." + key;
            }
            data_[key] = value;
        }
    }

    file.close();
    return true;
}

std::string ConfigLoader::get(const std::string& key, const std::string& defaultValue) const {
    auto it = data_.find(key);
    return (it != data_.end()) ? it->second : defaultValue;
}

std::string ConfigLoader::trim(const std::string& str) {
    const char* whitespace = " \t\n\r";
    size_t start = str.find_first_not_of(whitespace);
    size_t end = str.find_last_not_of(whitespace);
    return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}
