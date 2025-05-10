#include "GrayscaleImage.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <cstring>  // For memcpy
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <stdexcept>
#include <algorithm>

GrayscaleImage::GrayscaleImage(const char* filename) {
    // Image loading code using stbi
    int channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, STBI_grey);

    if (image == nullptr) {
        std::cerr << "Error: Could not load image " << filename << std::endl;
        exit(1);
    }

    // Dynamically allocate memory for the 2D array
    data = new int*[height];
    for (int i = 0; i < height; ++i) {
        data[i] = new int[width];
    }

    // Fill the matrix with pixel values from the image
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            data[i][j] = image[i * width + j];  // Access the 1D array as a 2D matrix
        }
    }

    // Free the dynamically allocated memory of stbi image
    stbi_image_free(image);
}
// Constructor: initialize from a pre-existing data matrix
GrayscaleImage::GrayscaleImage(int** inputData, int h, int w) : width(w), height(h) {
    // Dynamically allocate memory for the matrix
    data = new int*[height];
    for (int i = 0; i < height; ++i) {
        data[i] = new int[width];
    }

    // Copy the values from inputData to data
    for (int i = 0; i < height; ++i) {
        std::memcpy(data[i], inputData[i], width * sizeof(int));
    }
}
GrayscaleImage::GrayscaleImage(int w, int h) : width(w), height(h) {
    // Dynamically allocate memory for the matrix
    data = new int*[height];
    for (int i = 0; i < height; ++i) {
        data[i] = new int[width];
    }

    // Initialize all pixel values to 0 (black)
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            data[i][j] = 0;
        }
    }
}
GrayscaleImage::GrayscaleImage(const GrayscaleImage& other) : width(other.width), height(other.height) {
    // Dynamically allocate memory for the matrix
    data = new int*[height];
    for (int i = 0; i < height; ++i) {
        data[i] = new int[width];
    }

    // Copy the pixel values from the other image
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            data[i][j] = other.data[i][j];
        }
    }
}
GrayscaleImage::~GrayscaleImage() {
    // Deallocate memory for the 2D matrix
    for (int i = 0; i < height; ++i) {
        delete[] data[i];
    }
    delete[] data;
}
// Get a specific pixel value
int GrayscaleImage::get_pixel(int row, int col) const {
    return data[row][col];
}



// Set a specific pixel value
void GrayscaleImage::set_pixel(int row, int col, int value) {
    data[row][col] = value;
}

// Function to save the image to a PNG file
void GrayscaleImage::save_to_file(const char* filename) const {
    // Create a buffer to hold the image data in the format stb_image_write expects
    unsigned char* imageBuffer = new unsigned char[width * height];

    // Fill the buffer with pixel data (convert int to unsigned char)
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            imageBuffer[i * width + j] = static_cast<unsigned char>(data[i][j]);
        }
    }

    // Write the buffer to a PNG file
    if (!stbi_write_png(filename, width, height, 1, imageBuffer, width)) {
        std::cerr << "Error: Could not save image to file " << filename << std::endl;
    }

    // Clean up the allocated buffer
    delete[] imageBuffer;
}

bool GrayscaleImage::operator==(const GrayscaleImage& other) const {
    if (width != other.width || height != other.height) {
        return false;
    }

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (data[i][j] != other.data[i][j]) {
                return false;
            }
        }
    }

    return true;
}
GrayscaleImage GrayscaleImage::operator+(const GrayscaleImage& other) const {
    GrayscaleImage result(width, height);

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int sum = data[i][j] + other.data[i][j];
            result.data[i][j] = std::min(255, std::max(0, sum));  // Clamp to [0, 255]
        }
    }

    return result;
}
GrayscaleImage GrayscaleImage::operator-(const GrayscaleImage& other) const {
    GrayscaleImage result(width, height);

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int diff = data[i][j] - other.data[i][j];
            result.data[i][j] = std::min(255, std::max(0, diff));  // Clamp to [0, 255]
        }
    }

    return result;
}

