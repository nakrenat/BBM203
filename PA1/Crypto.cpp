#include "Crypto.h"
#include "GrayscaleImage.h"
#include "SecretImage.h"
#include <stdexcept>
#include <bitset>
#include <vector>
#include <string>

// Extract LSBs from SecretImage
std::vector<int> Crypto::extract_LSBits(SecretImage& secret_image, int message_length) {
    std::vector<int> LSB_array;

    // 1. Reconstruct the SecretImage to a GrayscaleImage.
    GrayscaleImage image = secret_image.reconstruct();

    // 2. Calculate the image dimensions.
    int width = secret_image.get_width();
    int height = secret_image.get_height();
    int total_pixels = width * height;

    // 3. Determine the total bits required based on message length (7 bits per character).
    int total_bits = message_length*7 ;

    // 4. Ensure the image has enough pixels.
    if (total_pixels < total_bits) {
        throw std::runtime_error("Not enough pixels to extract the message.");
    }

    // 5. Calculate the starting pixel index.
    int start_pixel = total_pixels - total_bits;

    // 6. Extract LSBs from the image.
    for (int i = start_pixel; i < total_pixels; ++i) {
        int x = i % width;
        int y = i / width;
        int pixel_value = image.get_pixel(y, x);

        // Extract the least significant bit and store it.
        LSB_array.push_back(pixel_value & 1);
    }

    return LSB_array;
}

// Decrypt message from LSB array
std::string Crypto::decrypt_message(const std::vector<int>& LSB_array) {
    std::string message;

    // 1. Check if LSB array size is a multiple of 7.
    if (LSB_array.size() % 7 != 0) {
        throw std::runtime_error("LSB array size must be a multiple of 7.");
    }

    // 2. Convert each group of 7 bits to an ASCII character.
    for (size_t i = 0; i < LSB_array.size(); i += 7) {
        int ascii_value = 0;
        for (int j = 0; j < 7; ++j) {
            ascii_value = (ascii_value << 1) | LSB_array[i + j];
        }
        message += static_cast<char>(ascii_value);
    }

    return message;
}

// Encrypt message into LSB array
std::vector<int> Crypto::encrypt_message(const std::string& message) {
    std::vector<int> LSB_array;

    // 1. Convert each character into a 7-bit binary representation.
    for (char c : message) {
        for (int i = 6; i >= 0; --i) {
            LSB_array.push_back((c >> i) & 1);
        }
    }

    return LSB_array;
}

// Embed LSB array into GrayscaleImage
SecretImage Crypto::embed_LSBits(GrayscaleImage& image, const std::vector<int>& LSB_array) {
    int width = image.get_width();
    int height = image.get_height();
    int total_pixels = width * height;

    // 1. Ensure the image has enough pixels to store the LSB array.
    if (total_pixels < LSB_array.size()) {
        throw std::runtime_error("Not enough pixels.");
    }

    // 2. Calculate the starting pixel.
    int start_pixel = total_pixels - LSB_array.size();

    // 3. Embed LSBs into the image.
    for (size_t i = 0; i < LSB_array.size(); ++i) {
        int x = (start_pixel + i) % width;
        int y = (start_pixel + i) / width;
        int pixel_value = image.get_pixel(y, x);

        // Embed the LSB.
        pixel_value = (pixel_value & ~1) | LSB_array[i];
        image.set_pixel(y, x, pixel_value);
    }

    // 4. Return the modified image as a SecretImage.
    return SecretImage(image);
}


