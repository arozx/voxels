#include "Renderer2D.h"

#include <GLFW/glfw3.h>

#include "../Camera/OrthographicCamera.h"
#include "../Shader/DefaultShaders.h"
#include "../Shader/Shader.h"
#include "Material.h"
#include "VertexArray.h"

namespace Engine {
struct QuadVertex {
    glm::vec3 Position;
    glm::vec4 Color;
    glm::vec2 TexCoord;
    float TexIndex;
    float TilingFactor;
};

struct Renderer2DData {
    static const uint32_t MaxQuads = 100000;
    static const uint32_t MaxVertices = MaxQuads * 4;
    static const uint32_t MaxIndices = MaxQuads * 6;
    static const uint32_t MaxTextureSlots = 32;

    std::shared_ptr<VertexArray> QuadVertexArray;
    std::shared_ptr<VertexBuffer> QuadVertexBuffer;
    std::shared_ptr<Material> TextureMaterial;
    std::shared_ptr<Texture> WhiteTexture;

    uint32_t QuadIndexCount = 0;
    QuadVertex* QuadVertexBufferBase = nullptr;
    QuadVertex* QuadVertexBufferPtr = nullptr;

    std::array<std::shared_ptr<Texture>, MaxTextureSlots> TextureSlots;
    uint32_t TextureSlotIndex = 1;  // 0 = white texture

    glm::vec4 QuadVertexPositions[4];
};

static Renderer2DData s_Data;

void Renderer2D::Initialize() {
    // Create vertex array
    s_Data.QuadVertexArray = std::shared_ptr<VertexArray>(VertexArray::Create());

    // Create vertex buffer
    s_Data.QuadVertexBuffer = std::shared_ptr<VertexBuffer>(
        VertexBuffer::Create(nullptr, s_Data.MaxVertices * sizeof(QuadVertex)));

    // Set buffer layout
    BufferLayout layout = {{ShaderDataType::Float3, "a_Position"},
                           {ShaderDataType::Float4, "a_Color"},
                           {ShaderDataType::Float2, "a_TexCoord"},
                           {ShaderDataType::Float, "a_TexIndex"},
                           {ShaderDataType::Float, "a_TilingFactor"}};
    s_Data.QuadVertexBuffer->SetLayout(layout);
    s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

    // Create and set index buffer
    uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];
    uint32_t offset = 0;
    for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6) {
        quadIndices[i + 0] = offset + 0;
        quadIndices[i + 1] = offset + 1;
        quadIndices[i + 2] = offset + 2;
        quadIndices[i + 3] = offset + 2;
        quadIndices[i + 4] = offset + 3;
        quadIndices[i + 5] = offset + 0;
        offset += 4;
    }

    auto quadIB = std::shared_ptr<IndexBuffer>(IndexBuffer::Create(quadIndices, s_Data.MaxIndices));
    s_Data.QuadVertexArray->SetIndexBuffer(quadIB);
    delete[] quadIndices;

    // Create and initialize the texture material with default shader
    s_Data.TextureMaterial =
        std::make_shared<Material>(DefaultShaders::GetOrCreate("BatchRenderer2D"));

    // Configure texture sampling properties
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Create white texture
    s_Data.WhiteTexture = std::make_shared<Texture>(1, 1);
    uint8_t whiteTextureData[4] = {255, 255, 255, 255};
    s_Data.WhiteTexture->SetData(whiteTextureData, sizeof(whiteTextureData));

    // Initialize texture slots
    s_Data.TextureSlots[0] = s_Data.WhiteTexture;

    // Create quad vertices
    s_Data.QuadVertexPositions[0] = {-0.5f, -0.5f, 0.0f, 1.0f};
    s_Data.QuadVertexPositions[1] = {0.5f, -0.5f, 0.0f, 1.0f};
    s_Data.QuadVertexPositions[2] = {0.5f, 0.5f, 0.0f, 1.0f};
    s_Data.QuadVertexPositions[3] = {-0.5f, 0.5f, 0.0f, 1.0f};

    // Allocate vertex buffer
    s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];
}

void Renderer2D::Shutdown() { delete[] s_Data.QuadVertexBufferBase; }

void Renderer2D::BeginScene() {
    // Default implementation for IRenderer interface
}

void Renderer2D::BeginScene(const std::shared_ptr<OrthographicCamera>& camera) {
    // Ensure material and shader exist
    if (!s_Data.TextureMaterial || !s_Data.TextureMaterial->GetShader()) {
        return;
    }

    s_Data.TextureMaterial->GetShader()->Bind();
    s_Data.TextureMaterial->GetShader()->SetMat4("u_ViewProjection",
                                                 camera->GetViewProjectionMatrix());
    StartBatch();
}

void Renderer2D::EndScene() { Flush(); }

void Renderer2D::StartBatch() {
    s_Data.QuadIndexCount = 0;
    s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
    s_Data.TextureSlotIndex = 1;
}

void Renderer2D::Flush() {
    if (s_Data.QuadIndexCount == 0) return;

    uint32_t dataSize =
        (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);
    s_Data.QuadVertexBuffer->Bind();
    glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, s_Data.QuadVertexBufferBase);

    // Bind textures
    for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++) s_Data.TextureSlots[i]->Bind(i);

    s_Data.TextureMaterial->Bind();
    s_Data.QuadVertexArray->Bind();
    glDrawElements(GL_TRIANGLES, s_Data.QuadIndexCount, GL_UNSIGNED_INT, nullptr);

    // Update stats
    m_Stats.DrawCalls++;
    m_Stats.IndexCount += s_Data.QuadIndexCount;
    m_Stats.VertexCount += s_Data.QuadIndexCount * 4 / 6;  // Convert indices to vertices
}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size,
                          const glm::vec4& color) {
    DrawQuad({position.x, position.y, 0.0f}, size, color);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size,
                          const glm::vec4& color) {
    if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices) NextBatch();

    const float texIndex = 0.0f;  // White Texture
    const float tilingFactor = 1.0f;

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                          glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

    // Add vertices to buffer
    for (size_t i = 0; i < 4; i++) {
        s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
        s_Data.QuadVertexBufferPtr->Color = color;
        s_Data.QuadVertexBufferPtr->TexCoord = {(i == 1 || i == 2) ? 1.0f : 0.0f,
                                                (i == 2 || i == 3) ? 1.0f : 0.0f};
        s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
        s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
        s_Data.QuadVertexBufferPtr++;
    }

    s_Data.QuadIndexCount += 6;
}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size,
                          const std::shared_ptr<Texture>& texture, float tilingFactor,
                          const glm::vec4& tintColor) {
    DrawQuad({position.x, position.y, 0.0f}, size, texture, tilingFactor, tintColor);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size,
                          const std::shared_ptr<Texture>& texture, float tilingFactor,
                          const glm::vec4& tintColor) {
    if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices) NextBatch();

    float textureIndex = 0.0f;
    for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++) {
        if (s_Data.TextureSlots[i] == texture) {
            textureIndex = (float)i;
            break;
        }
    }

    if (textureIndex == 0.0f) {
        if (s_Data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots) NextBatch();

        textureIndex = (float)s_Data.TextureSlotIndex;
        s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
        s_Data.TextureSlotIndex++;
    }

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                          glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

    for (size_t i = 0; i < 4; i++) {
        s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
        s_Data.QuadVertexBufferPtr->Color = tintColor;
        s_Data.QuadVertexBufferPtr->TexCoord = {(i == 1 || i == 2) ? 1.0f : 0.0f,
                                                (i == 2 || i == 3) ? 1.0f : 0.0f};
        s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
        s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
        s_Data.QuadVertexBufferPtr++;
    }

    s_Data.QuadIndexCount += 6;
}

void Renderer2D::NextBatch() {
    Flush();
    StartBatch();
}
}  // namespace Engine
