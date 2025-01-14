#version 450 core
out vec4 FragColor;

in vec2 v_TexCoord;

uniform sampler2D u_Texture;
uniform vec4 u_Color;
uniform float u_PixelSize = 32.0; // Number of pixels (higher = more pixelated)

void main() {
    vec2 texSize = textureSize(u_Texture, 0);
    vec2 pixelCoord = v_TexCoord * texSize;
    vec2 roundedCoord = floor(pixelCoord / u_PixelSize) * u_PixelSize;
    vec2 finalCoord = roundedCoord / texSize;
    
    FragColor = texture(u_Texture, finalCoord) * u_Color;
}
