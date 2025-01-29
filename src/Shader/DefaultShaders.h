#pragma once

#include "Shader.h"
#include "../Core/AssetManager.h"

namespace Engine {
    /**
     * @brief Collection of predefined shader programs
     * 
     * Provides easy access to commonly used shader programs
     * with predefined vertex and fragment shader combinations.
     */
class DefaultShaders {
   public:
    /**
     * @brief Preloads frequently used shaders
     */
    static void PreloadShaders() {
        // These are prime candidates for caching & hot-reloading
        std::vector<std::pair<std::string, std::string>> frequentShaders = {
            {"assets/shaders/basic_mvp.vert", "assets/shaders/color.frag"},
            {"assets/shaders/basic_mvp.vert", "assets/shaders/simple_color.frag"},
            {"assets/shaders/textured.vert", "assets/shaders/textured.frag"}};

        for (const auto& [vert, frag] : frequentShaders) {
            auto shader = Shader::CreateFromFiles(vert, frag);
            if (shader) {
                // Cache the shader and enable hot-reloading in debug builds
                AssetManager::Get().MarkAsFrequentlyUsed<Shader>(vert + ";" + frag);
            }
        }
    }

    /**
     * @brief Loads basic MVP transformation shader
     * @return Shader for basic vertex transformation
     */
    static std::shared_ptr<Shader> LoadBasicShader() {
        return std::shared_ptr<Shader>(
            Shader::CreateFromFiles("assets/shaders/basic_mvp.vert", "assets/shaders/color.frag"));
    }

    /**
     * @brief Loads simple color shader
     * @return Shader for solid color rendering
     */
    static std::shared_ptr<Shader> LoadSimpleColorShader() {
        return std::shared_ptr<Shader>(Shader::CreateFromFiles("assets/shaders/basic_mvp.vert",
                                                               "assets/shaders/simple_color.frag"));
    }

    /**
     * @brief Loads textured rendering shader
     * @return Shader for textured objects
     */
    static std::shared_ptr<Shader> LoadTexturedShader() {
        return std::shared_ptr<Shader>(Shader::CreateFromFiles("assets/shaders/textured.vert",
                                                               "assets/shaders/textured.frag"));
    }

    /**
     * @brief Loads pixel art shader
     * @return Shader for pixel art style rendering
     */
    static std::shared_ptr<Shader> LoadPixelShader() {
        return std::shared_ptr<Shader>(Shader::CreateFromFiles(
            "assets/shaders/textured.vert",  // Reuse the textured vertex shader
            "assets/shaders/pixel.frag"));
    }

    /**
     * @brief Loads wave dissolve effect shader
     * @return Shader for wave dissolve transitions
     */
    static std::shared_ptr<Shader> LoadWaveDissolveShader() {
        return std::shared_ptr<Shader>(Shader::CreateFromFiles(
            "assets/shaders/textured.vert", "assets/shaders/wave_dissolve.frag"));
    }

    /**
     * @brief Loads gaussian blur shader
     * @return Shader for blur post-processing
     */
    static std::shared_ptr<Shader> LoadBlurShader() {
        return std::shared_ptr<Shader>(
            Shader::CreateFromFiles("assets/shaders/textured.vert", "assets/shaders/blur.frag"));
    }

    /**
     * @brief Loads terrain rendering shader
     * @return Shader for terrain visualization
     */
    static std::shared_ptr<Shader> LoadTerrainShader() { return LoadTexturedShader(); }

    static std::shared_ptr<Shader> GetOrCreate(const std::string& name) {
        static std::unordered_map<std::string, std::shared_ptr<Shader>> s_Shaders;

        if (s_Shaders.find(name) != s_Shaders.end()) {
            return s_Shaders[name];
        }

        if (name == "BatchRenderer2D") {
            auto shader = Shader::CreateFromSource(GetBatchRenderer2DVertexSrc(),
                                                   GetBatchRenderer2DFragmentSrc());
            if (shader) {
                s_Shaders[name] = shader;
            }
            return shader;
        }

        return nullptr;
    }

   private:
    static const char* GetBatchRenderer2DVertexSrc() {
        return R"(
                #version 330 core
                layout(location = 0) in vec3 a_Position;
                layout(location = 1) in vec4 a_Color;
                layout(location = 2) in vec2 a_TexCoord;
                layout(location = 3) in float a_TexIndex;
                layout(location = 4) in float a_TilingFactor;
                
                uniform mat4 u_ViewProjection;
                
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
            )";
    }

    static const char* GetBatchRenderer2DFragmentSrc() {
        return R"(
                #version 330 core
                layout(location = 0) out vec4 color;
                
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
                        // ...add more cases if needed...
                    }
                    color = texColor;
                }
            )";
    }
};
}
