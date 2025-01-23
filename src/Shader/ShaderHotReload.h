#pragma once
#include <pch.h>
#include "../Core/FileWatcher.h"

namespace Engine {
    class Shader;

    /**
     * @brief Manages hot-reloading of shader files
     * 
     * This class watches shader files for changes and automatically reloads them
     * when modifications are detected, enabling real-time shader development.
     */
    class ShaderHotReload {
    public:
        /**
         * @brief Gets the singleton instance of ShaderHotReload
         * @return Reference to the ShaderHotReload instance
         */
        static ShaderHotReload& Get();

        /**
         * @brief Sets up file watching for a shader's vertex and fragment files
         * 
         * @param shader The shader to watch
         * @param vertPath Path to the vertex shader file
         * @param fragPath Path to the fragment shader file
         */
        void WatchShader(const std::shared_ptr<Shader>& shader,
                        const std::string& vertPath,
                        const std::string& fragPath);

        /**
         * @brief Updates the file watcher to check for changes
         */
        void Update();

    private:
        /**
         * @brief Reloads a shader from its source files
         * 
         * @param shader The shader to reload
         * @param vertPath Path to the vertex shader file
         * @param fragPath Path to the fragment shader file
         */
        void ReloadShader(const std::shared_ptr<Shader>& shader,
                         const std::string& vertPath,
                         const std::string& fragPath);

        FileWatcher m_Watcher;
        std::unordered_map<std::shared_ptr<Shader>, std::pair<std::string, std::string>> m_ShaderPaths;
    };
}
