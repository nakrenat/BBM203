#include "SecretImage.h"
#include <fstream>
#include <iostream>

// Constructor: split image into upper and lower triangular arrays
SecretImage::SecretImage(const GrayscaleImage& image) {
    width = image.get_width();
    height = image.get_height();

    // Dynamically allocate memory for the upper and lower triangular matrices
    int upper_size = width * (width + 1) / 2;
    int lower_size = width * (width - 1) / 2;

    upper_triangular = new (std::nothrow) int[upper_size];
    lower_triangular = new (std::nothrow) int[lower_size];

    // Check if memory allocation succeeded
    if (!upper_triangular || !lower_triangular) {
        std::cerr << "Memory allocation failed" << std::endl;
        delete[] upper_triangular;
        delete[] lower_triangular;
        throw std::bad_alloc();
    }

    int upper_index = 0;
    int lower_index = 0;

    // Fill the matrices
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int pixel = image.get_pixel(i, j);
            if (j >= i) {
                upper_triangular[upper_index++] = pixel;
            } else {
                lower_triangular[lower_index++] = pixel;
            }
        }
    }
}

// Constructor: instantiate based on data read from file (reuse original arrays)
SecretImage::SecretImage(int w, int h, int* upper, int* lower) : width(w), height(h) {
    // Take ownership of the provided arrays without copying
    upper_triangular = upper;
    lower_triangular = lower;

    // Ensure the pointers are valid
    if (!upper_triangular || !lower_triangular) {
        std::cerr << "Error: Null pointer passed for triangular matrices." << std::endl;
        throw std::invalid_argument("Null pointer passed for triangular matrices.");
    }
}

// Destructor: free the arrays
SecretImage::~SecretImage() {
    delete[] upper_triangular;
    delete[] lower_triangular;
}

// Reconstructs and returns the full image from upper and lower triangular matrices
GrayscaleImage SecretImage::reconstruct() const {
    GrayscaleImage image(width, height);

    int upper_index = 0;
    int lower_index = 0;

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (j >= i) {
                image.set_pixel(i, j, upper_triangular[upper_index++]);
            } else {
                image.set_pixel(i, j, lower_triangular[lower_index++]);
            }
        }
    }

    return image;
}

// Save the filtered image back to the triangular arrays
void SecretImage::save_back(const GrayscaleImage& image) {
    int upper_index = 0;
    int lower_index = 0;

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int pixel = image.get_pixel(i, j);
            if (j >= i) {
                upper_triangular[upper_index++] = pixel;
            } else {
                lower_triangular[lower_index++] = pixel;
            }
        }
    }
}

// Save the upper and lower triangular arrays to a file
void SecretImage::save_to_file(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    file << width << " " << height << std::endl;

    int upper_size = width * (width + 1) / 2;
    for (int i = 0; i < upper_size; ++i) {
        file << upper_triangular[i];
        if (i < upper_size - 1) file << " ";
    }
    file << std::endl;

    int lower_size = width * (width - 1) / 2;
    for (int i = 0; i < lower_size; ++i) {
        file << lower_triangular[i];
        if (i < lower_size - 1) file << " ";
    }
    file << std::endl;

    file.close();
}

// Static function to load a SecretImage from a file
SecretImage SecretImage::load_from_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return SecretImage(nullptr);
    }

    int w, h;
    file >> w >> h;

    int upper_size = w * (w + 1) / 2;
    int lower_size = w * (w - 1) / 2;

    int* upper = new int[upper_size];
    int* lower = new int[lower_size];

    for (int i = 0; i < upper_size; ++i) {
        file >> upper[i];
    }

    for (int i = 0; i < lower_size; ++i) {
        file >> lower[i];
    }

    file.close();

    return SecretImage(w, h, upper, lower);
}

// Returns a pointer to the upper triangular part of the secret image.
int* SecretImage::get_upper_triangular() const {
    return upper_triangular;
}

// Returns a pointer to the lower triangular part of the secret image.
int* SecretImage::get_lower_triangular() const {
    return lower_triangular;
}

// Returns the width of the secret image.
int SecretImage::get_width() const {
    return width;
}

// Returns the height of the secret image.
int SecretImage::get_height() const {
    return height;
}
