#pragma once
#include "Shader.h"

namespace Engine {
    namespace DefaultShaders {
        static std::shared_ptr<Shader> LoadBasicShader() {
            return std::shared_ptr<Shader>(Shader::CreateFromFiles(
                "assets/shaders/basic_mvp.vert",
                "assets/shaders/color.frag"
            ));
        }

        static std::shared_ptr<Shader> LoadSimpleColorShader() {
            return std::shared_ptr<Shader>(Shader::CreateFromFiles(
                "assets/shaders/basic_mvp.vert",
                "assets/shaders/simple_color.frag"
            ));
        }

        static std::shared_ptr<Shader> LoadTexturedShader() {
            return std::shared_ptr<Shader>(Shader::CreateFromFiles(
                "assets/shaders/textured.vert",
                "assets/shaders/textured.frag"
            ));
        }
    }
}
