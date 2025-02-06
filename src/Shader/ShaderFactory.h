#pragma once

#include "Shader.h"

namespace Engine {

class ShaderFactory {
   public:
    ShaderFactory();
    ShaderFactory& AddVertexInput(const std::string& input);
    ShaderFactory& AddFragmentOutput(const std::string& output);
    ShaderFactory& AddUniform(const std::string& uniform);
    ShaderFactory& SetVertexMain(const std::string& mainCode);
    ShaderFactory& SetFragmentMain(const std::string& mainCode);
    std::shared_ptr<Shader> Build();

   private:
    std::string m_Version;
    std::string m_VertexInputs;
    std::string m_FragmentOutputs;
    std::string m_Uniforms;
    std::string m_VertexMain;
    std::string m_FragmentMain;
};

}  // namespace Engine
