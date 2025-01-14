#include "VertexArray.h"
#include "OpenGLVertexArray.h"

namespace Engine {
    VertexArray* VertexArray::Create() {
        return new OpenGLVertexArray();
    }
}