#pragma once
#include <vector>
#include <fstream>

class Pixel {
public:
    unsigned char a, b, c;
    Pixel(unsigned char, unsigned char, unsigned char);
    Pixel() = default;
    void RGB_to_HSL();       // 1 -> 2
    void HSL_to_RGB();       // 2 -> 1
    void RGB_to_HSV();       // 1 -> 3
    void HSV_to_RGB();       // 3 -> 1
    void RGB_to_YCbCr_601(); // 1 -> 4
    void YCbCr_601_to_RGB(); // 4 -> 1
    void RGB_to_YCbCr_709(); // 1 -> 5
    void YCbCr_709_to_RGB(); // 5 -> 1
    void RGB_to_YCoCg();     // 1 -> 6
    void YCoCg_to_RGB();     // 6 -> 1
    void RGB_to_CMY();       // 1 -> 7
    void CMY_to_RGB();       // 7 -> 1
};

class Image {
public:
    Image(const std::string&, int);
    void write(std::string, int);
    void change_color_space(std::string, std::string);
private:
    int width;
    int height;
	int color_depth;
    std::vector <std::vector <Pixel>> image;
};


