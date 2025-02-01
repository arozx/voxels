#include "ShaderLibrary.h"

#include "ShaderFactory.h"

namespace Engine {

// Initialize static members
std::unordered_map<std::string, std::shared_ptr<Shader>> ShaderLibrary::s_Shaders;

std::shared_ptr<Shader> ShaderLibrary::CreateBasicShader() {
    const std::string name = "basic";
    if (Exists(name)) return Get(name);

    auto shader = ShaderFactory()
                      .AddVertexInput("layout (location = 0) in vec3 aPos;")
                      .AddUniform("uniform mat4 u_ViewProjection;")
                      .AddUniform("uniform mat4 u_Model;")
                      .SetVertexMain(R"(
            void main() {
                gl_Position = u_ViewProjection * u_Model * vec4(aPos, 1.0);
            }
        )")
                      .AddFragmentOutput("out vec4 FragColor;")
                      .AddUniform("uniform vec4 u_Color;")
                      .SetFragmentMain(R"(
            void main() {
                FragColor = u_Color;
            }
        )")
                      .Build();

    s_Shaders[name] = shader;
#ifdef ENGINE_DEBUG
    shader->EnableHotReload("assets/shaders/basic.vert", "assets/shaders/basic.frag");
#endif
    return shader;
}

std::shared_ptr<Shader> ShaderLibrary::CreateColorShader() {
    const std::string name = "color";
    if (Exists(name)) return Get(name);

    auto shader = ShaderFactory()
                      .AddVertexInput("layout (location = 0) in vec3 aPos;")
                      .AddUniform("uniform mat4 u_ViewProjection;")
                      .AddUniform("uniform mat4 u_Model;")
                      .SetVertexMain(R"(
            void main() {
                gl_Position = u_ViewProjection * u_Model * vec4(aPos, 1.0);
            }
        )")
                      .AddFragmentOutput("out vec4 FragColor;")
                      .SetFragmentMain(R"(
            void main() {
                FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
            }
        )")
                      .Build();

    s_Shaders[name] = shader;
#ifdef ENGINE_DEBUG
    shader->EnableHotReload("assets/shaders/basic.vert", "assets/shaders/simple_color.frag");
#endif
    return shader;
}

std::shared_ptr<Shader> ShaderLibrary::CreateTextureShader() {
    const std::string name = "texture";
    if (Exists(name)) return Get(name);

    auto shader = ShaderFactory()
                      .AddVertexInput("layout (location = 0) in vec3 aPos;")
                      .AddVertexInput("layout (location = 1) in vec2 aTexCoord;")
                      .AddUniform("uniform mat4 u_ViewProjection;")
                      .AddUniform("uniform mat4 u_Model;")
                      .SetVertexMain(R"(
            out vec2 TexCoord;
            void main() {
                gl_Position = u_ViewProjection * u_Model * vec4(aPos, 1.0);
                TexCoord = aTexCoord;
            }
        )")
                      .AddFragmentOutput("out vec4 FragColor;")
                      .SetFragmentMain(R"(
            in vec2 TexCoord;
            uniform sampler2D u_Texture;
            void main() {
                FragColor = texture(u_Texture, TexCoord);
            }
        )")
                      .Build();

    s_Shaders[name] = shader;
    return shader;
}

std::shared_ptr<Shader> ShaderLibrary::CreateBatchRenderer2DShader() {
    const std::string name = "batch_renderer_2d";
    if (Exists(name)) return Get(name);

    auto shader = ShaderFactory()
                      .AddVertexInput("layout (location = 0) in vec3 a_Position;")
                      .AddVertexInput("layout (location = 1) in vec4 a_Color;")
                      .AddVertexInput("layout (location = 2) in vec2 a_TexCoord;")
                      .AddVertexInput("layout (location = 3) in float a_TexIndex;")
                      .AddVertexInput("layout (location = 4) in float a_TilingFactor;")
                      .AddUniform("uniform mat4 u_ViewProjection;")
                      .SetVertexMain(R"(
            out vec4 v_Color;
            out vec2 v_TexCoord;
            out float v_TexIndex;
            out float v_TilingFactor;
            void main() {
                v_Color = a_Color;
                v_TexCoord = a_TexCoord;
                v_TexIndex = a_TexIndex;
                v_TilingFactor = a_TilingFactor;
                gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
            }
        )")
                      .AddFragmentOutput("out vec4 FragColor;")
                      .SetFragmentMain(R"(
            in vec4 v_Color;
            in vec2 v_TexCoord;
            in float v_TexIndex;
            in float v_TilingFactor;
            uniform sampler2D u_Textures[32];
            void main() {
                vec4 texColor = v_Color;
                switch(int(v_TexIndex)) {
                    case  0: texColor *= texture(u_Textures[ 0], v_TexCoord * v_TilingFactor); break;
                    case  1: texColor *= texture(u_Textures[ 1], v_TexCoord * v_TilingFactor); break;
                    case  2: texColor *= texture(u_Textures[ 2], v_TexCoord * v_TilingFactor); break;
                    case  3: texColor *= texture(u_Textures[ 3], v_TexCoord * v_TilingFactor); break;
                    case  4: texColor *= texture(u_Textures[ 4], v_TexCoord * v_TilingFactor); break;
                    case  5: texColor *= texture(u_Textures[ 5], v_TexCoord * v_TilingFactor); break;
                    case  6: texColor *= texture(u_Textures[ 6], v_TexCoord * v_TilingFactor); break;
                    case  7: texColor *= texture(u_Textures[ 7], v_TexCoord * v_TilingFactor); break;
                    case  8: texColor *= texture(u_Textures[ 8], v_TexCoord * v_TilingFactor); break;
                    case  9: texColor *= texture(u_Textures[ 9], v_TexCoord * v_TilingFactor); break;
                    case 10: texColor *= texture(u_Textures[10], v_TexCoord * v_TilingFactor); break;
                    case 11: texColor *= texture(u_Textures[11], v_TexCoord * v_TilingFactor); break;
                    case 12: texColor *= texture(u_Textures[12], v_TexCoord * v_TilingFactor); break;
                    case 13: texColor *= texture(u_Textures[13], v_TexCoord * v_TilingFactor); break;
                    case 14: texColor *= texture(u_Textures[14], v_TexCoord * v_TilingFactor); break;
                    case 15: texColor *= texture(u_Textures[15], v_TexCoord * v_TilingFactor); break;
                    case 16: texColor *= texture(u_Textures[16], v_TexCoord * v_TilingFactor); break;
                    case 17: texColor *= texture(u_Textures[17], v_TexCoord * v_TilingFactor); break;
                    case 18: texColor *= texture(u_Textures[18], v_TexCoord * v_TilingFactor); break;
                    case 19: texColor *= texture(u_Textures[19], v_TexCoord * v_TilingFactor); break;
                    case 20: texColor *= texture(u_Textures[20], v_TexCoord * v_TilingFactor); break;
                    case 21: texColor *= texture(u_Textures[21], v_TexCoord * v_TilingFactor); break;
                    case 22: texColor *= texture(u_Textures[22], v_TexCoord * v_TilingFactor); break;
                    case 23: texColor *= texture(u_Textures[23], v_TexCoord * v_TilingFactor); break;
                    case 24: texColor *= texture(u_Textures[24], v_TexCoord * v_TilingFactor); break;
                    case 25: texColor *= texture(u_Textures[25], v_TexCoord * v_TilingFactor); break;
                    case 26: texColor *= texture(u_Textures[26], v_TexCoord * v_TilingFactor); break;
                    case 27: texColor *= texture(u_Textures[27], v_TexCoord * v_TilingFactor); break;
                    case 28: texColor *= texture(u_Textures[28], v_TexCoord * v_TilingFactor); break;
                    case 29: texColor *= texture(u_Textures[29], v_TexCoord * v_TilingFactor); break;
                    case 30: texColor *= texture(u_Textures[30], v_TexCoord * v_TilingFactor); break;
                    case 31: texColor *= texture(u_Textures[31], v_TexCoord * v_TilingFactor); break;
                }
                FragColor = texColor;
            }
        )")
                      .Build();

    s_Shaders[name] = shader;
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