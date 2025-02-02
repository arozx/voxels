#pragma once

#include <pch.h>

#include "Shader.h"

namespace Engine {

class ShaderLibrary {
   public:
    static std::shared_ptr<Shader> Load(const std::string& name, const std::string& vertexPath,
                                        const std::string& fragmentPath);
    static std::shared_ptr<Shader> LoadFromSource(const std::string& name,
                                                  const std::string& vertexSrc,
                                                  const std::string& fragmentSrc);
    static std::shared_ptr<Shader> Get(const std::string& name);
    static bool Exists(const std::string& name);

    // Factory methods for built-in shaders
    static std::shared_ptr<Shader> CreateBasicShader();
    static std::shared_ptr<Shader> CreateColorShader();
    static std::shared_ptr<Shader> CreateTextureShader();
    static std::shared_ptr<Shader> CreateBatchRenderer2DShader();
    static std::shared_ptr<Shader> LoadTexturedShader();

   private:
    static std::unordered_map<std::string, std::shared_ptr<Shader>> s_Shaders;
};

}  // namespace Engine