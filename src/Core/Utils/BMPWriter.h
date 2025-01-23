#pragma once
#include <pch.h>

class BMPWriter {
public:
/**
 * @brief A utility class for writing grayscale BMP image files
 */
    static void SaveGrayscaleBMP(const char* filename, const std::vector<uint8_t>& data, int width, int height) {
        std::ofstream file(filename, std::ios::binary);
    /**
     * @brief Saves a grayscale image as a BMP file
     * 
     * This function creates a BMP file with an 8-bit grayscale color depth (256 shades).
     * The image is written bottom-up as per BMP specification, with appropriate padding
     * to ensure each row is aligned to 4 bytes.
     *
     * @param filename The path and name of the output BMP file
     * @param data Vector containing the grayscale pixel data (values 0-255)
     * @param width The width of the image in pixels
     * @param height The height of the image in pixels
     * 
     * @details The function:
     * - Creates a standard BMP header
     * - Writes a 256-color grayscale palette
     * - Adds necessary row padding
     * - Writes pixel data from bottom to top
     * 
     * @note The input data vector must contain width * height elements
     */        
        // Calculate file size including palette
        uint32_t paletteSize = 256 * 4; // 256 colors * 4 bytes each
        uint32_t pixelDataOffset = 54 + paletteSize;
        uint32_t fileSize = pixelDataOffset + width * height;
        uint32_t numColors = 256;
        
        // BMP Header
        uint8_t header[54] = {
            'B', 'M',            // Signature
            (uint8_t)(fileSize & 0xFF), // File size
            (uint8_t)((fileSize >> 8) & 0xFF),
            (uint8_t)((fileSize >> 16) & 0xFF),
            (uint8_t)((fileSize >> 24) & 0xFF),
            0, 0, 0, 0,         // Reserved
            (uint8_t)(pixelDataOffset & 0xFF),      // Pixel data offset
            (uint8_t)((pixelDataOffset >> 8) & 0xFF),
            (uint8_t)((pixelDataOffset >> 16) & 0xFF),
            (uint8_t)((pixelDataOffset >> 24) & 0xFF),
            40, 0, 0, 0,        // Info header size
            (uint8_t)(width & 0xFF),   // Width
            (uint8_t)((width >> 8) & 0xFF),
            (uint8_t)((width >> 16) & 0xFF),
            (uint8_t)((width >> 24) & 0xFF),
            (uint8_t)(height & 0xFF),  // Height
            (uint8_t)((height >> 8) & 0xFF),
            (uint8_t)((height >> 16) & 0xFF),
            (uint8_t)((height >> 24) & 0xFF),
            1, 0,              // Planes
            8, 0,              // Bits per pixel (8 for grayscale)
            0, 0, 0, 0,        // Compression
            0, 0, 0, 0,        // Image size
            0, 0, 0, 0,        // X pixels per meter
            0, 0, 0, 0,        // Y pixels per meter
            (uint8_t)(numColors & 0xFF),        // Number of colors
            (uint8_t)((numColors >> 8) & 0xFF),
            (uint8_t)((numColors >> 16) & 0xFF),
            (uint8_t)((numColors >> 24) & 0xFF),
            (uint8_t)(numColors & 0xFF),        // Important colors
            (uint8_t)((numColors >> 8) & 0xFF),
            (uint8_t)((numColors >> 16) & 0xFF),
            (uint8_t)((numColors >> 24) & 0xFF)
        };

        // Write header
        file.write(reinterpret_cast<char*>(header), 54);

        // Write color palette (grayscale)
        for (int i = 0; i < 256; i++) {
            uint8_t color[4] = {
                static_cast<uint8_t>(i),  // Blue
                static_cast<uint8_t>(i),  // Green
                static_cast<uint8_t>(i),  // Red
                0                         // Reserved
            };
            file.write(reinterpret_cast<char*>(color), 4);
        }

        // Add padding to ensure each row is a multiple of 4 bytes
        int padding = (4 - (width % 4)) % 4;
        std::vector<uint8_t> paddingBytes(padding, 0);

        // Write pixel data (bottom-up)
        for (int y = height - 1; y >= 0; y--) {
            file.write(reinterpret_cast<const char*>(&data[y * width]), width);
            if (padding > 0) {
                file.write(reinterpret_cast<const char*>(paddingBytes.data()), padding);
            }
        }
        
        file.close();
    }
};
