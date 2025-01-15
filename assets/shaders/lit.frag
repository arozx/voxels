
#version 330 core

in vec2 v_TexCoord;
in vec3 v_Normal;
in vec3 v_FragPos;

uniform vec3 u_LightPos;
uniform vec3 u_ViewPos;
uniform vec4 u_Color;
uniform sampler2D u_Texture;

uniform vec3 u_LightColor;
uniform float u_AmbientStrength;
uniform float u_SpecularStrength;
uniform float u_Shininess;

out vec4 FragColor;

void main() {
    // Ambient
    vec3 ambient = u_AmbientStrength * u_LightColor;

    // Diffuse
    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(u_LightPos - v_FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * u_LightColor;

    // Specular
    vec3 viewDir = normalize(u_ViewPos - v_FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Shininess);
    vec3 specular = u_SpecularStrength * spec * u_LightColor;

    vec4 texColor = texture(u_Texture, v_TexCoord) * u_Color;
    vec3 result = (ambient + diffuse + specular) * texColor.rgb;
    FragColor = vec4(result, texColor.a);
}