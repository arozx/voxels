#pragma once
#include <pch.h>

namespace Engine {
    /**
     * @brief Types of data that can be stored in shader buffers
     */
    enum class ShaderDataType {
        None = 0,    ///< No type specified
        Float,       ///< Single precision float
        Float2,      ///< 2D vector of floats
        Float3,      ///< 3D vector of floats
        Float4,      ///< 4D vector of floats
        Mat3,        ///< 3x3 matrix
        Mat4,        ///< 4x4 matrix
        Int,         ///< 32-bit integer
        Int2,        ///< 2D vector of integers
        Int3,        ///< 3D vector of integers
        Int4,        ///< 4D vector of integers
        Bool         ///< Boolean value
    };

    /**
     * @brief Gets the size in bytes of a shader data type
     * @param type The shader data type
     * @return Size in bytes
     */
    static uint32_t ShaderDataTypeSize(ShaderDataType type) {
        switch (type) {
            case ShaderDataType::Float:  return 4;
            case ShaderDataType::Float2: return 4 * 2;
            case ShaderDataType::Float3: return 4 * 3;
            case ShaderDataType::Float4: return 4 * 4;
            case ShaderDataType::Mat3:   return 4 * 3 * 3;
            case ShaderDataType::Mat4:   return 4 * 4 * 4;
            case ShaderDataType::Int:    return 4;
            case ShaderDataType::Int2:   return 4 * 2;
            case ShaderDataType::Int3:   return 4 * 3;
            case ShaderDataType::Int4:   return 4 * 4;
            case ShaderDataType::Bool:   return 1;
            default: return 0;
        }
    }

    /**
     * @brief Gets the number of components in a shader data type
     * @param type The shader data type
     * @return Number of components
     */
    static uint32_t GetComponentCount(ShaderDataType type) {
        switch (type) {
            case ShaderDataType::Float:  return 1;
            case ShaderDataType::Float2: return 2;
            case ShaderDataType::Float3: return 3;
            case ShaderDataType::Float4: return 4;
            case ShaderDataType::Mat3:   return 3 * 3;
            case ShaderDataType::Mat4:   return 4 * 4;
            case ShaderDataType::Int:    return 1;
            case ShaderDataType::Int2:   return 2;
            case ShaderDataType::Int3:   return 3;
            case ShaderDataType::Int4:   return 4;
            case ShaderDataType::Bool:   return 1;
            default: return 0;
        }
    }

    /**
     * @brief Describes a single element in a buffer layout
     */
    struct BufferElement {
        std::string Name;      ///< Element name in shader
        ShaderDataType Type;   ///< Data type
        uint32_t Size;        ///< Size in bytes
        uint32_t Offset;      ///< Offset in buffer
        bool Normalized;      ///< Whether to normalize

        /**
         * @brief Constructs a buffer element
         * @param type Data type of the element
         * @param name Name in the shader
         * @param normalized Whether to normalize the data
         */
        BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
            : Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized) {}
    };

    /**
     * @brief Describes the layout of a vertex buffer
     */
    class BufferLayout {
    public:
        BufferLayout() {}
        BufferLayout(const std::initializer_list<BufferElement>& elements) 
            : m_Elements(elements) {
            CalculateOffsetsAndStride();
        }

        uint32_t GetStride() const { return m_Stride; }
        const std::vector<BufferElement>& GetElements() const { return m_Elements; }
        
        std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
        std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
        std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
        std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }
    
    private:
        void CalculateOffsetsAndStride() {
            uint32_t offset = 0;
            m_Stride = 0;
            
            for (auto& element : m_Elements) {
                element.Offset = offset;
                offset += element.Size;
                m_Stride += element.Size;
            }
        }

        std::vector<BufferElement> m_Elements;
        uint32_t m_Stride = 0;
    };

    /**
     * @brief Abstract vertex buffer interface
     */
    class VertexBuffer {
    public:
        virtual ~VertexBuffer() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void SetLayout(const BufferLayout& layout) = 0;
        virtual const BufferLayout& GetLayout() const = 0;

        static VertexBuffer* Create(const float* vertices, uint32_t size);
    };

    /**
     * @brief Abstract index buffer interface
     */
    class IndexBuffer {
    public:
        virtual ~IndexBuffer() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;
        
        virtual uint32_t GetCount() const = 0;

        static IndexBuffer* Create(const uint32_t* indices, uint32_t count);
    };
}