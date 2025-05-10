#ifndef GRAYSCALE_IMAGE_H
#define GRAYSCALE_IMAGE_H

class GrayscaleImage {
private:
    int** data;  // 2D array for storing pixel values
    int width{}, height{};  // Image dimensions

public:
    // Constructor: loads an image from a file
    GrayscaleImage(const char* filename);

    // Constructor: initializes from a 2D data matrix
    GrayscaleImage(int** inputData, int h, int w);

    // Constructor to create a blank image of given width and height
    GrayscaleImage(int w, int h);

    // Copy constructor
    GrayscaleImage(const GrayscaleImage& other);
    GrayscaleImage(int w, int h, int initialValue);

    // Destructor
    ~GrayscaleImage();

    // Operator overloads for image comparison, addition, and subtraction
    bool operator==(const GrayscaleImage& other) const;  // Checks equality of two images
    GrayscaleImage operator+(const GrayscaleImage& other) const;  // Adds two images
    GrayscaleImage operator-(const GrayscaleImage& other) const;  // Subtracts one image from another

    // Method to get image dimensions
    int get_width() const { return width; }  // Returns the width of the image
     int get_height() const { return height; }  // Returns the height of the image

    // Get a specific pixel value at (row, col)
     int get_pixel(int row, int col) const;

    // Set a specific pixel value at (row, col)
    void set_pixel(int row, int col, int value);

    // Function to save the image to a PNG file
    void save_to_file(const char* filename) const;

    // Getter function for accessing the raw pixel data (the 2D matrix)
     int** get_data() const {
        return data;
    }

};

#endif // GRAYSCALE_IMAGE_H
