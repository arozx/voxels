#pragma once

#include <map>
#include <string>
#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2 Size;        // Size of glyph
    glm::ivec2 Bearing;     // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Offset to advance to next glyph
};

class TextRenderer {
public:
    TextRenderer(const char* fontPath, unsigned int fontSize, unsigned int shaderProgram);
    void RenderText(const std::string& text, float x, float y, float scale, glm::vec3 color);

private:
    std::map<char, Character> Characters;
    unsigned int VAO, VBO;
    unsigned int shaderProgram;
};
