#include "VertexArray.h"
#include "OpenGLVertexArray.h"

/**
 * @namespace Engine
 * @brief Main engine namespace containing all core functionality
 */
namespace Engine {
    /**
     * @brief Factory method to create a new vertex array
     * @return A new OpenGL vertex array instance
     */
    VertexArray* VertexArray::Create() {
        return new OpenGLVertexArray();
    }
}