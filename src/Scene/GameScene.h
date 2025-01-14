#pragma once

#include "Scene.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Material.h"
#include "Shader/DefaultShaders.h"
#include "Renderer/MeshTemplates.h"

namespace Engine {
    /**
     * @brief Example game scene implementation
     * 
     * Demonstrates scene setup with textured objects and
     * parent-child relationships.
     */
    class GameScene : public Scene {
    public:
        /** @brief Creates a new game scene */
        GameScene() : Scene("Game") {}

        /** @brief Sets up initial scene objects and materials */
        virtual void OnCreate() override {
            // Create a textured square
            auto square = CreateObject("Square");
            square->vertexArray = createSquareVA();
            square->material = createSquareMaterial();
            square->transform.position = {0.0f, 0.0f, 0.0f};
            
            // Create a child object
            auto childSquare = CreateObject("ChildSquare");
            childSquare->vertexArray = square->vertexArray;
            childSquare->material = createTransparentMaterial();
            childSquare->transform.position = {0.5f, 0.5f, 0.0f};
            square->AddChild(childSquare);
        }

        /** @brief Updates scene logic */
        virtual void OnUpdate(float deltaTime) override {
            // Update scene logic
        }

    private:
        /**
         * @brief Creates a vertex array for a square
         * @return Configured vertex array
         */
        std::shared_ptr<VertexArray> createSquareVA() {
            auto squareVertices = MeshTemplates::TexturedSquare;
            std::shared_ptr<VertexArray> squareVA(VertexArray::Create());
            
            std::shared_ptr<VertexBuffer> squareVB(
                VertexBuffer::Create(squareVertices.data(), 
                squareVertices.size() * sizeof(float)));

            BufferLayout squareLayout = {
                { ShaderDataType::Float3, "aPosition" },
                { ShaderDataType::Float2, "aTexCoord" }
            };
            
            squareVB->SetLayout(squareLayout);
            squareVA->AddVertexBuffer(squareVB);

            std::shared_ptr<IndexBuffer> squareIB(
                IndexBuffer::Create(MeshTemplates::SquareIndices.data(), 
                MeshTemplates::SquareIndices.size()));
            squareVA->SetIndexBuffer(squareIB);

            return squareVA;
        }

        /**
         * @brief Creates a textured material
         * @return Configured material
         */
        std::shared_ptr<Material> createSquareMaterial() {
            auto shader = DefaultShaders::LoadTexturedShader();
            auto material = std::make_shared<Material>(shader);
            material->SetTexture("u_Texture", Texture::Create("assets/textures/test.png"));
            material->SetVector4("u_Color", glm::vec4(1.0f));
            return material;
        }

        /**
         * @brief Creates a semi-transparent material
         * @return Configured material
         */
        std::shared_ptr<Material> createTransparentMaterial() {
            auto shader = DefaultShaders::LoadTexturedShader();
            auto material = std::make_shared<Material>(shader);
            material->SetVector4("u_Color", glm::vec4(1.0f, 0.0f, 0.0f, 0.5f)); // Semi-transparent red
            return material;
        }
    };
}