
#include "DefaultShaders.h"

namespace Engine {

const char* const DefaultShaders::BATCH_RENDERER_2D_VERT = R"(
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

const char* const DefaultShaders::BATCH_RENDERER_2D_FRAG = R"(
    #version 330 core
    out vec4 FragColor;
    
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
            // ...add more cases as needed
        }
        FragColor = texColor;
    }
)";

std::shared_ptr<Shader> DefaultShaders::GetOrCreate(const std::string& name) {
    static std::unordered_map<std::string, std::shared_ptr<Shader>> s_DefaultShaders;

    if (s_DefaultShaders.find(name) != s_DefaultShaders.end()) {
        return s_DefaultShaders[name];
    }

    if (name == "BatchRenderer2D") {
        auto shader = Shader::CreateFromSource(BATCH_RENDERER_2D_VERT, BATCH_RENDERER_2D_FRAG);
        s_DefaultShaders[name] = shader;
        return shader;
    }

    LOG_ERROR("Unknown default shader requested: ", name);
    return nullptr;
}

void DefaultShaders::PreloadShaders() { GetOrCreate("BatchRenderer2D"); }

}  // namespace Engine