#pragma once
#include <vector>
#include <fstream>

class Pixel {
public:
    unsigned char a, b, c;
    Pixel(unsigned char, unsigned char, unsigned char);
    Pixel() = default;
    void RGB_to_YCbCr_601();
    void YCbCr_601_to_RGB();
};

class Image {
public:
    Image(const std::string&);
    void write(std::string);
    void correct_brightness(int, int, double);
private:
    int width;
    int height;
	int color_depth;
    char format_0;
    char format_1;
    std::vector <std::vector <Pixel>> image;
};


