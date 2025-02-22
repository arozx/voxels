#pragma once

#include <pch.h>
#include <filesystem>

namespace Engine {
    class FileSystem {
    public:
        static bool Exists(const std::string& filepath) {
            std::string path = SantizePath(filepath);
            return std::filesystem::exists(path);
        }
        static bool CreateDirectory(const std::string& path) {
            std::string sanitizedPath = SantizePath(path);
            return std::filesystem::create_directory(sanitizedPath);
        }
        static bool WriteFile(const std::string& filepath, const std::string& content) {
            constexpr size_t MAX_FILE_SIZE = 10 * 1024 * 1024;  // 10MB limit
            if (content.size() > MAX_FILE_SIZE) {
                LOG_ERROR("Content size exceeds limit: ", content.size(), " > ", MAX_FILE_SIZE);
                return false;
            }
            std::string path = SantizePath(filepath);
            std::ofstream file(path);
            if (!file.is_open()) {
                LOG_ERROR("Failed to open file '", path, "': ", std::strerror(errno));
                return false;
            }
            file << content;
            if (file.fail()) {
                LOG_ERROR("Failed to write to file '", path, "': ", std::strerror(errno));
                file.close();
                return false;
            }
            file.close();
            return true;
        }

   private:
       static std::string SantizePath(const std::string& path) {
        std::string sanitizedPath = path;
        // Replace all backslashes with forward slashes
        std::replace(sanitizedPath.begin(), sanitizedPath.end(), '\\', '/');
        // Remove consecutive forward slashes
        sanitizedPath.erase(std::unique(sanitizedPath.begin(), sanitizedPath.end(), [](char a, char b) { return a == '/' && b == '/'; }), sanitizedPath.end());
        // remove '%.%.' path traversals
        while (sanitizedPath.find("/./") != std::string::npos) {
            sanitizedPath.replace(sanitizedPath.find("/./"), 3, "/");
        }
        // remove '%..%' path traversals
        while (sanitizedPath.find("/../") != std::string::npos) {
            sanitizedPath.replace(sanitizedPath.find("/../"), 4, "/");
        }
        // remove trailing '%.%' path traversals
        if (sanitizedPath.find("/.") == sanitizedPath.size() - 2) {
            sanitizedPath.erase(sanitizedPath.size() - 2);
        }
        return sanitizedPath;
    }
    };
}