#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

#include <string>
#include <unordered_map>

class ConfigLoader {
public:
    // Load the configuration file into memory
    bool load(const std::string& filename);

    // Get a value by key with an optional default
    std::string get(const std::string& key, const std::string& defaultValue = "") const;

private:
    std::unordered_map<std::string, std::string> data_;

    // Trim whitespace from both ends of a string
    static std::string trim(const std::string& str);
};

#endif
