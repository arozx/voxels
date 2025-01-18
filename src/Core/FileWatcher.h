#pragma once
#include <pch.h>
#include <filesystem>
#include <chrono>

namespace Engine {
    /**
     * @brief A utility class for monitoring file changes
     * @details Watches specified files for modifications and executes callbacks when changes are detected
     */
    class FileWatcher {
    public:
        /** @brief Callback function type for file change notifications */
        using FileChangedCallback = std::function<void(const std::string&)>;

        /**
         * @brief Register a file to watch for changes
         * @param path The path to the file to watch
         * @param callback The function to call when the file changes
         */
        void WatchFile(const std::string& path, FileChangedCallback callback) {
            try {
                auto time = std::filesystem::last_write_time(std::filesystem::path(path));
                m_WatchList[path] = FileInfo{time, callback};
            } catch (const std::filesystem::filesystem_error& e) {
                LOG_ERROR("Failed to watch file {}: {}", path, e.what());
            }
        }

        /**
         * @brief Check for changes in watched files
         * @details Should be called regularly to detect file modifications
         */
        void Update() {
            for (auto& [path, info] : m_WatchList) {
                try {
                    auto currentTime = std::filesystem::last_write_time(std::filesystem::path(path));
                    if (currentTime != info.lastWriteTime) {
                        info.lastWriteTime = currentTime;
                        info.callback(path);
                    }
                } catch (const std::filesystem::filesystem_error& e) {
                    LOG_ERROR("Error watching file {}: {}", path, e.what());
                }
            }
        }

    private:
        /**
         * @brief Structure to hold file watching information
         */
        struct FileInfo {
            std::filesystem::file_time_type lastWriteTime; ///< Last recorded modification time
            FileChangedCallback callback; ///< Callback to execute on change
        };
        std::unordered_map<std::string, FileInfo> m_WatchList; ///< Map of watched files
    };
}
