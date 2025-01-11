#pragma once

namespace Engine {
    namespace DefaultShaders {
        // Basic vertex shader with MVP transformation
        static const char* BasicVertexShader = R"(
            #version 330 core
            layout (location = 0) in vec3 aPos;
            
            uniform mat4 u_ViewProjection;
            uniform mat4 u_Model;
            
            void main() {
                gl_Position = u_ViewProjection * u_Model * vec4(aPos, 1.0);
            }
        )";

        // Color fragment shader with uniform color
        static const char* ColorFragmentShader = R"(
            #version 330 core
            out vec4 FragColor;
            uniform vec4 u_Color;
            
            void main() {
                FragColor = u_Color;
            }
        )";

        // Simple color fragment shader with fixed color
        static const char* SimpleColorFragmentShader = R"(
            #version 330 core
            out vec4 FragColor;
            void main() {
                FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
            }
        )";

        // Textured vertex shader with texture coordinates
        constexpr const char* TexturedVertexShader = R"(
            #version 450 core
            layout(location = 0) in vec3 aPosition;
            layout(location = 1) in vec2 aTexCoord;
            
            uniform mat4 u_ViewProjection;
            uniform mat4 u_Model;
            
            out vec2 v_TexCoord;
            
            void main() {
                v_TexCoord = aTexCoord;
                gl_Position = u_ViewProjection * u_Model * vec4(aPosition, 1.0);
            }
        )";

        // Textured fragment shader with texture sampling
        constexpr const char* TexturedFragmentShader = R"(
            #version 450 core
            out vec4 FragColor;
            
            in vec2 v_TexCoord;
            
            uniform sampler2D u_Texture;
            uniform vec4 u_Color;
            
            void main() {
                FragColor = texture(u_Texture, v_TexCoord) * u_Color;
            }
        )";
    }
}
