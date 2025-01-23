
#pragma once
#include <filesystem>

namespace Engine {
    class FileSystem {
    public:
        static bool Exists(const std::string& filepath) {
            return std::filesystem::exists(filepath);
        }
    };
}