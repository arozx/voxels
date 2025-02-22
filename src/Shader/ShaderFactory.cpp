#include "ShaderFactory.h"

namespace Engine {

ShaderFactory::ShaderFactory() { m_Version = "#version 330 core\n"; }

ShaderFactory& ShaderFactory::AddVertexInput(const std::string& input) {
    m_VertexInputs += input + "\n";
    return *this;
}

ShaderFactory& ShaderFactory::AddFragmentOutput(const std::string& output) {
    m_FragmentOutputs += output + "\n";
    return *this;
}

ShaderFactory& ShaderFactory::AddUniform(const std::string& uniform) {
    m_Uniforms += uniform + "\n";
    return *this;
}

ShaderFactory& ShaderFactory::SetVertexMain(const std::string& mainCode) {
    m_VertexMain = mainCode;
    return *this;
}

ShaderFactory& ShaderFactory::SetFragmentMain(const std::string& mainCode) {
    m_FragmentMain = mainCode;
    return *this;
}

std::shared_ptr<Shader> ShaderFactory::Build() {
    std::string vertexSrc = m_Version + m_VertexInputs + m_Uniforms + m_VertexMain;
    std::string fragmentSrc = m_Version + m_FragmentOutputs + m_Uniforms + m_FragmentMain;
    return Shader::CreateFromSource(vertexSrc.c_str(), fragmentSrc.c_str());
}

}  // namespace Engine
