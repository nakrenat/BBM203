#include "Filter.h"
#include <algorithm>
#include <cmath>
#include <vector>
#include <numeric>
#include <math.h>
#include <cmath>
#include <omp.h>
// Mean Filter
#include <omp.h>

#include <cmath> // For std::floor

void Filter::apply_mean_filter(GrayscaleImage& image, int kernelSize) {
    // Step 1: Copy the original image for reference.
    int width = image.get_width();
    int height = image.get_height();
    GrayscaleImage original_image = image;  // Copy of the original image.

    // Step 2: Pad the original image with black pixels (0).
    int padSize = kernelSize / 2;
    GrayscaleImage padded_image(width + 2 * padSize, height + 2 * padSize);

    // Fill padded image with zeros (black)
    for (int y = 0; y < padded_image.get_height(); ++y) {
        for (int x = 0; x < padded_image.get_width(); ++x) {
            if (x < padSize || x >= (width + padSize) || y < padSize || y >= (height + padSize)) {
                padded_image.set_pixel(x, y, 0);  // Padding with black pixels
            } else {
                // Copy original image pixels into padded image
                padded_image.set_pixel(x, y, original_image.get_pixel(x - padSize, y - padSize));
            }
        }
    }

    // Step 3: For each pixel, calculate the mean value of its neighbors using a kernel.
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int sum = 0;
            int count = 0;

            // Iterate over the kernel
            for (int ky = -padSize; ky <= padSize; ++ky) {
                for (int kx = -padSize; kx <= padSize; ++kx) {
                    // Access pixels from the padded image
                    int pixel_value = padded_image.get_pixel(x + kx + padSize, y + ky + padSize);
                    sum += pixel_value;
                    count++;
                }
            }

            // Step 4: Compute the average and update the pixel in the original image.
            int average = sum / count;  // Integer division
            image.set_pixel(x, y, average);  // Update the central pixel
        }
    }
}





// Helper function to create Gaussian kernel
std::vector<std::vector<double>> create_gaussian_kernel(int kernelSize, double sigma) {
    int padding = kernelSize / 2;
    std::vector<std::vector<double>> kernel(kernelSize, std::vector<double>(kernelSize, 0));
    double sum = 0.0;
    double s = 2.0 * sigma * sigma;

    // Create the Gaussian kernel using the formula
    for (int x = -padding; x <= padding; ++x) {
        for (int y = -padding; y <= padding; ++y) {
            double r = sqrt(x * x + y * y);
            double value = exp(-(r * r) / s) / (M_PI * s);
            kernel[x + padding][y + padding] = value;
            sum += value;
        }
    }

    // Normalize the kernel
    for (int i = 0; i < kernelSize; ++i) {
        for (int j = 0; j < kernelSize; ++j) {
            kernel[i][j] /= sum;
        }
    }

    return kernel;
}

// Gaussian Smoothing Filter
void Filter::apply_gaussian_smoothing(GrayscaleImage& image, int kernelSize, double sigma) {
    int width = image.get_width();
    int height = image.get_height();
    int padding = kernelSize / 2;

    // 1. Create Gaussian kernel
    std::vector<std::vector<double>> kernel = create_gaussian_kernel(kernelSize, sigma);

    // 2. Copy the original image for reference
    GrayscaleImage copy = image;

    // 3. Iterate over each pixel in the image
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            double sum = 0.0;

            // 4. Compute the weighted sum using the Gaussian kernel
            for (int ky = -padding; ky <= padding; ++ky) {
                for (int kx = -padding; kx <= padding; ++kx) {
                    int nx = x + kx;
                    int ny = y + ky;

                    // Check boundaries (handle edges)
                    if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                        sum += copy.get_pixel(nx, ny) * kernel[ky + padding][kx + padding];
                    }
                }
            }

            // 5. Update the pixel values with the smoothed result
            int newValue = static_cast<int>(sum);
            image.set_pixel(x, y, newValue);
        }
    }
}
#include <cmath> // For std::floor

void Filter::apply_unsharp_mask(GrayscaleImage& image, int kernelSize, double amount) {
    int width = image.get_width();
    int height = image.get_height();

    // 1. Create a blurred version of the image using Gaussian smoothing
    GrayscaleImage blurredImage = image; // Make a copy of the image
    apply_gaussian_smoothing(blurredImage, kernelSize, 1.0); // sigma = 1.0

    // 2. Iterate over each pixel in the image to apply the unsharp mask formula
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // Get the original and blurred pixel values
            int originalPixel = image.get_pixel(x, y);
            int blurredPixel = blurredImage.get_pixel(x, y);

            // 3. Calculate the "edge" or "high-frequency" component: original - blurred
            int edgeComponent = originalPixel - blurredPixel;

            // 4. Apply the unsharp mask formula: original + amount * edgeComponent
            double sharpenedPixel = originalPixel + amount * edgeComponent;

            // 5. Use floor to round down to the nearest whole number
            sharpenedPixel = std::floor(sharpenedPixel);

            // 6. Clip the pixel value to stay within the valid range [0, 255]
            sharpenedPixel = std::max(0, std::min(255, static_cast<int>(sharpenedPixel)));

            // 7. Set the sharpened pixel value in the image
            image.set_pixel(x, y, sharpenedPixel);
        }
    }
}



