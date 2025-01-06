#ifndef CONFIGLOADER_H
#define CONFIGLOADER_H

#include <string>
#include <unordered_map>

class ConfigLoader {
public:
    // Get the singleton instance
    static ConfigLoader& getInstance();

    // Delete copy constructor and assignment operator
    ConfigLoader(const ConfigLoader&) = delete;
    ConfigLoader& operator=(const ConfigLoader&) = delete;

    // Public methods
    bool load(const std::string& fileName);
    std::string get(const std::string& key, const std::string& defaultValue = "") const;

private:
    // Private constructor
    ConfigLoader() = default;

    // Key-value store for configuration
    std::unordered_map<std::string, std::string> data;
};

#endif
