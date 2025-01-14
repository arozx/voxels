#pragma once

#include "Shader.h"

namespace Engine {
    /**
     * @brief Collection of predefined shader programs
     * 
     * Provides easy access to commonly used shader programs
     * with predefined vertex and fragment shader combinations.
     */
    namespace DefaultShaders {
        /**
         * @brief Loads basic MVP transformation shader
         * @return Shader for basic vertex transformation
         */
        static std::shared_ptr<Shader> LoadBasicShader() {
            return std::shared_ptr<Shader>(Shader::CreateFromFiles(
                "assets/shaders/basic_mvp.vert",
                "assets/shaders/color.frag"
            ));
        }

        /**
         * @brief Loads simple color shader
         * @return Shader for solid color rendering
         */
        static std::shared_ptr<Shader> LoadSimpleColorShader() {
            return std::shared_ptr<Shader>(Shader::CreateFromFiles(
                "assets/shaders/basic_mvp.vert",
                "assets/shaders/simple_color.frag"
            ));
        }

        /**
         * @brief Loads textured rendering shader
         * @return Shader for textured objects
         */
        static std::shared_ptr<Shader> LoadTexturedShader() {
            return std::shared_ptr<Shader>(Shader::CreateFromFiles(
                "assets/shaders/textured.vert",
                "assets/shaders/textured.frag"
            ));
        }

        /**
         * @brief Loads pixel art shader
         * @return Shader for pixel art style rendering
         */
        static std::shared_ptr<Shader> LoadPixelShader() {
            return std::shared_ptr<Shader>(Shader::CreateFromFiles(
                "assets/shaders/textured.vert", // Reuse the textured vertex shader
                "assets/shaders/pixel.frag"
            ));
        }

        /**
         * @brief Loads wave dissolve effect shader
         * @return Shader for wave dissolve transitions
         */
        static std::shared_ptr<Shader> LoadWaveDissolveShader() {
            return std::shared_ptr<Shader>(Shader::CreateFromFiles(
                "assets/shaders/textured.vert",
                "assets/shaders/wave_dissolve.frag"
            ));
        }

        /**
         * @brief Loads gaussian blur shader
         * @return Shader for blur post-processing
         */
        static std::shared_ptr<Shader> LoadBlurShader() {
            return std::shared_ptr<Shader>(Shader::CreateFromFiles(
                "assets/shaders/textured.vert",
                "assets/shaders/blur.frag"
            ));
        }

        /**
         * @brief Loads terrain rendering shader
         * @return Shader for terrain visualization
         */
        static std::shared_ptr<Shader> LoadTerrainShader() {
            return LoadTexturedShader();
        }
    }
}
