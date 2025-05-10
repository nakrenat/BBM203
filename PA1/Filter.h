#ifndef FILTER_H
#define FILTER_H

#include "GrayscaleImage.h"

class Filter {
public:
    // Apply the Mean Filter
    // @param image: The grayscale image to apply the mean filter on
    // @param kernelSize: Size of the kernel (should be odd), default is 3
    static void apply_mean_filter(GrayscaleImage& image, int kernelSize = 3);

    // Apply Gaussian Smoothing Filter
    // @param image: The grayscale image to apply the Gaussian smoothing filter on
    // @param kernelSize: Size of the Gaussian kernel (should be odd), default is 3
    // @param sigma: The standard deviation for the Gaussian distribution, default is 1.0
    static void apply_gaussian_smoothing(GrayscaleImage& image, int kernelSize = 3, double sigma = 1.0);

    // Apply Unsharp Masking Filter
    // @param image: The grayscale image to apply the unsharp mask on
    // @param kernelSize: Size of the Gaussian kernel for smoothing (should be odd), default is 3
    // @param amount: The amount of sharpening to apply, default is 1.5
    static void apply_unsharp_mask(GrayscaleImage& image, int kernelSize = 3, double amount = 1.5);
};

#endif // FILTER_H
