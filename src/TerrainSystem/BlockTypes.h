#pragma once

/**
 * @enum BlockType
 * @brief Defines different types of blocks available in the terrain
 */
enum class BlockType {
    Air = 0,    ///< Empty/air block
    Grass,      ///< Grass block with dirt sides
    Dirt,       ///< Pure dirt block
    Stone,      ///< Stone block
    Snow,       ///< Snow block
    COUNT       ///< Total number of block types
};

/**
 * @struct BlockTexture
 * @brief Holds UV coordinates for different faces of a block
 */
struct BlockTexture {
    float topU, topV;           ///< Top face UV coordinates
    float sideU, sideV;         ///< Side faces UV coordinates
    float bottomU, bottomV;     ///< Bottom face UV coordinates
    
    // Each texture is 16x16 in a 32x48 atlas
    static constexpr float TEXTURE_SIZE = 0.5f;      ///< 16/32 = 0.5 for horizontal
    static constexpr float TEXTURE_V_SIZE = 0.333f;  ///< 16/48 ≈ 0.333 for vertical
};

// Texture atlas layout:
// [snow (0,0)]    [stone (0.5,0)]
// [dirt (0,0.333)]  [grass_side (0.5,0.333)]
// [grass_top (0,0.666)]  [empty (0.5,0.666)]

constexpr BlockTexture BLOCK_TEXTURES[] = {
    // Air (unused)
    { 0.0f, 0.0f,      0.0f, 0.0f,       0.0f, 0.0f },
    
    // Grass
    { 0.0f, 0.666f,    // top (grass_top)
      0.5f, 0.333f,    // side (grass_side)
      0.0f, 0.333f },  // bottom (dirt)
    
    // Dirt (all dirt texture)
    { 0.0f, 0.333f,    // top
      0.0f, 0.333f,    // side
      0.0f, 0.333f },  // bottom
    
    // Stone (all stone texture)
    { 0.5f, 0.0f,      // top
      0.5f, 0.0f,      // side
      0.5f, 0.0f },    // bottom
    
    // Snow (all snow texture)
    { 0.0f, 0.0f,      // top
      0.0f, 0.0f,      // side
      0.0f, 0.0f }     // bottom
};