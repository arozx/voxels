#include "ShaderLibrary.h"

#include "ShaderFactory.h"

namespace Engine {

// Initialize static members
std::unordered_map<std::string, std::shared_ptr<Shader>> ShaderLibrary::s_Shaders;

std::shared_ptr<Shader> ShaderLibrary::CreateBasicShader() {
    const std::string name = "basic";
    if (Exists(name)) return Get(name);

    auto shader = Load(name, "assets/shaders/basic.vert", "assets/shaders/basic.frag");
#ifdef ENGINE_DEBUG
    shader->EnableHotReload("assets/shaders/basic.vert", "assets/shaders/basic.frag");
#endif
    return shader;
}

std::shared_ptr<Shader> ShaderLibrary::CreateColorShader() {
    const std::string name = "color";
    if (Exists(name)) return Get(name);

    auto shader = Load(name, "assets/shaders/basic.vert", "assets/shaders/simple_color.frag");
#ifdef ENGINE_DEBUG
    shader->EnableHotReload("assets/shaders/basic.vert", "assets/shaders/simple_color.frag");
#endif
    return shader;
}

std::shared_ptr<Shader> ShaderLibrary::CreateTextureShader() {
    const std::string name = "texture";
    if (Exists(name)) return Get(name);

    auto shader = Load(name, "assets/shaders/texture.vert", "assets/shaders/texture.frag");
#ifdef ENGINE_DEBUG
    shader->EnableHotReload("assets/shaders/texture.vert", "assets/shaders/texture.frag");
#endif
    return shader;
}

std::shared_ptr<Shader> ShaderLibrary::CreateBatchRenderer2DShader() {
    const std::string name = "batch_renderer_2d";
    if (Exists(name)) return Get(name);

    auto shader = Load(name, "assets/shaders/batch_renderer_2d.vert",
                       "assets/shaders/batch_renderer_2d.frag");
#ifdef ENGINE_DEBUG
    shader->EnableHotReload("assets/shaders/batch_renderer_2d.vert",
                            "assets/shaders/batch_renderer_2d.frag");
#endif
    return shader;
}

std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& name, const std::string& vertexPath,
                                            const std::string& fragmentPath) {
    auto shader = Shader::CreateFromFiles(vertexPath, fragmentPath);
    if (!shader) {
        LOG_ERROR("Failed to load shader from files: ", vertexPath, " and ", fragmentPath);
        return nullptr;
    }
    s_Shaders[name] = shader;
    return shader;
}

std::shared_ptr<Shader> ShaderLibrary::LoadFromSource(const std::string& name,
                                                      const std::string& vertexSrc,
                                                      const std::string& fragmentSrc) {
    auto shader = Shader::CreateFromSource(vertexSrc.c_str(), fragmentSrc.c_str());
    if (!shader) {
        LOG_ERROR("Failed to create shader from source for: ", name);
        return nullptr;
    }
    s_Shaders[name] = shader;
    return shader;
}

std::shared_ptr<Shader> ShaderLibrary::Get(const std::string& name) {
    auto it = s_Shaders.find(name);
    if (it == s_Shaders.end()) {
        LOG_ERROR("Shader not found: ", name);
        return nullptr;
    }
    return it->second;
}

bool ShaderLibrary::Exists(const std::string& name) {
    return s_Shaders.find(name) != s_Shaders.end();
}

std::shared_ptr<Shader> ShaderLibrary::LoadTexturedShader() { return CreateTextureShader(); }

}  // namespace Engine