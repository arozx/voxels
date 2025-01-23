
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_FragPos;

void main() {
    v_TexCoord = aTexCoord;
    v_Normal = mat3(transpose(inverse(u_Transform))) * aNormal;
    v_FragPos = vec3(u_Transform * vec4(aPosition, 1.0));
    gl_Position = u_ViewProjection * u_Transform * vec4(aPosition, 1.0);
}