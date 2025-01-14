#version 330 core

out vec4 FragColor;

in vec2 v_TexCoord;

uniform vec4 u_Color;
uniform sampler2D u_Texture;
uniform float u_BlurStrength = 0.005;

void main() {
    vec2 texelSize = 1.0 / textureSize(u_Texture, 0);
    vec4 result = vec4(0.0);
    
    // 3x3 Gaussian blur
    for(int x = -1; x <= 1; x++) {
        for(int y = -1; y <= 1; y++) {
            vec2 offset = vec2(float(x), float(y)) * u_BlurStrength;
            result += texture(u_Texture, v_TexCoord + offset);
        }
    }
    
    FragColor = (result / 9.0) * u_Color;
}
