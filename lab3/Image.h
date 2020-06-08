#pragma once
#include <vector>
#include <string>

class Point {
public:
    double x, y;
    Point(double, double);
};

class Image {
public:
    Image(const std::string&, bool, double, bool);
    void write(const std::string&, double, bool, int);
    void dithering(int, int);
private:
    int width;
    int height;
    int color_depth;
    std::vector <std::vector <double>> image;
    std::vector <std::vector <double>> errors;
};