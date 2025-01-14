#version 330 core

out vec4 FragColor;

in vec2 v_TexCoord;

uniform vec4 u_Color;
uniform sampler2D u_Texture;
uniform float u_Time;
uniform float u_WaveSpeed = 2.0;
uniform float u_WaveFrequency = 10.0;
uniform float u_DissolveAmount = 0.5;

void main() {
    vec4 texColor = texture(u_Texture, v_TexCoord) * u_Color;
    float wave = sin(v_TexCoord.x * u_WaveFrequency + u_Time * u_WaveSpeed) * 0.5 + 0.5;
    float dissolve = step(wave, u_DissolveAmount);
    FragColor = texColor * dissolve;
}
