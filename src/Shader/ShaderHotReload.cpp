#include "ShaderHotReload.h"
#include "Shader.h"

namespace Engine {
    /** @brief Gets the singleton instance */
    ShaderHotReload& ShaderHotReload::Get() {
        static ShaderHotReload instance;
        return instance;
    }

    /** @brief Implements shader file watching */
    void ShaderHotReload::WatchShader(const std::shared_ptr<Shader>& shader,
            const std::string& vertPath,
            const std::string& fragPath) {
        m_Watcher.WatchFile(vertPath, [this, shader, vertPath, fragPath](const std::string&) {
            ReloadShader(shader, vertPath, fragPath);
        });
        
        m_Watcher.WatchFile(fragPath, [this, shader, vertPath, fragPath](const std::string&) {
            ReloadShader(shader, vertPath, fragPath);
        });

        m_ShaderPaths[shader] = {vertPath, fragPath};
    }

    /** @brief Handles shader reloading */
    void ShaderHotReload::ReloadShader(const std::shared_ptr<Shader>& shader,
        const std::string& vertPath,
        const std::string& fragPath) {
        LOG_INFO("Reloading shader: {} and {}", vertPath, fragPath);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        if (shader->Reload(vertPath + ";" + fragPath)) {
            LOG_INFO("Shader reloaded successfully");
        } else {
            LOG_ERROR("Failed to reload shader");
        }
    }

    /** @brief Updates file watching system */
    void ShaderHotReload::Update() {
        m_Watcher.Update();
    }
}
