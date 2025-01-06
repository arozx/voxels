#pragma once

const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

struct Material {
    vec3 color;
    float ambient;
    float diffuse;
    float specular;
    int shininess;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float ambientStrength;
    float diffuseStrength;
    float specularStrength;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 color;
    float cutOff;
    float outerCutOff;
    float ambientStrength;
    float diffuseStrength;
    float specularStrength;
};

uniform Material material;
uniform bool isOutline;
uniform PointLight pointLights[4];
uniform int numPointLights;
uniform SpotLight spotLights[4];
uniform int numSpotLights;
uniform vec3 viewPos;
uniform bool isLightSource;

void main() {
    if (isOutline) {
        FragColor = vec4(material.color, 1.0);
        return;
    }

    if (isLightSource) {
        FragColor = vec4(material.color, 1.0);
        return;
    }

    vec3 ambient = vec3(0.0);
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    // Point lights
    for (int i = 0; i < numPointLights; ++i) {
        PointLight light = pointLights[i];

        // ambient
        ambient += light.ambientStrength * light.color * material.ambient;

        // diffuse
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(light.position - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        diffuse += light.diffuseStrength * diff * light.color * material.diffuse;

        // specular
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        specular += light.specularStrength * spec * light.color * material.specular;
    }

    // Spot lights
    for(int i = 0; i < numSpotLights; i++) {
        SpotLight light = spotLights[i];
        vec3 lightDir = normalize(light.position - FragPos);
        
        // Check if fragment is in spotlight cone
        float theta = dot(lightDir, normalize(-light.direction));
        float epsilon = light.cutOff - light.outerCutOff;
        float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

        if(theta > light.outerCutOff) {
            // ambient
            ambient += light.ambientStrength * light.color * material.ambient;

            // diffuse
            vec3 norm = normalize(Normal);
            float diff = max(dot(norm, lightDir), 0.0);
            diffuse += intensity * light.diffuseStrength * diff * light.color * material.diffuse;

            // specular
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
            specular += intensity * light.specularStrength * spec * light.color * material.specular;
        }
    }

    vec3 result = (ambient + diffuse + specular) * material.color;
    FragColor = vec4(result, 1.0);
}
)";
