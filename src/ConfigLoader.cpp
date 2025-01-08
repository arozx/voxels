#include "ConfigLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>

ConfigLoader& ConfigLoader::getInstance() {
    static ConfigLoader instance;
    return instance;
}

bool ConfigLoader::load(const std::string& fileName) {
    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open config file: " << fileName << std::endl;
        return false;
    }

    std::string line, section;
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == ';' || line[0] == '#') continue;

        // Handle sections
        if (line[0] == '[' && line.back() == ']') {
            section = line.substr(1, line.size() - 2);
            continue;
        }

        // Handle key-value pairs
        std::istringstream keyValueStream(line);
        std::string key, value;
        if (std::getline(keyValueStream, key, '=') && std::getline(keyValueStream, value)) {
            // Trim whitespace and add to map
            key = section.empty() ? key : section + "." + key;
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            data[key] = value;
        }
    }

    file.close();
    return true;
}

std::string ConfigLoader::get(const std::string& key, const std::string& defaultValue) const {
    auto it = data.find(key);
    return it != data.end() ? it->second : defaultValue;
}
