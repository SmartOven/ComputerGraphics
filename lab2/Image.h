#pragma once
#include <vector>
#include <string>

class Point{
public:
    double x, y;
    Point(double, double);
};

class Image{
public:
    Image(std::string);
    void write(std::string);
    void draw_line(Point, Point, double, int, double, bool);
private:
    int width;
    int height;
	int color_depth;
    std::vector<std::vector<unsigned char>> image;
    void plot(int, int, double, int, double, bool);
};
