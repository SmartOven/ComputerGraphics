#include "Image.h"
#include <cmath>
#include <algorithm>
#include <fstream>

// Вспомогательные функции
double sqr(double a) {
    return a * a;
}
double distance(Point p1, Point p2) {
    return sqrt(sqr(p1.x - p2.x) + sqr(p1.y - p2.y));
}
double triangle_square(Point A, Point B, Point C) { // нахождение площади треугольника
    double a = distance(A, B);
    double b = distance(B, C);
    double c = distance(C, A);
    double p = (a + b + c) / 2;
    return sqrt(p * (p - a) * (p - b) * (p - c));
}
bool point_in_rectangle(Point A, Point B, Point C, Point D, Point P) { // находится ли точка внутри прямоугольника
	const double rectangle_h = distance(A, B);
	const double rectangle_w = distance(A, C);
	const double rectangle_square = rectangle_h * rectangle_w;
	const double possible_size = triangle_square(P, A, B) + triangle_square(P, A, C) + triangle_square(P, C, D) + triangle_square(P, D, B);
    return fabs(rectangle_square - possible_size) < 1e-5;
}
double intersection_square(Point A, Point B, Point C, Point D, double X, double Y) { // площадь пересечения пикселя с прямоугольником
    if (point_in_rectangle(A, B, C, D, Point{ X, Y }) &&
        point_in_rectangle(A, B, C, D, Point{ X + 1.0, Y * 1.0 }) &&
        point_in_rectangle(A, B, C, D, Point{ X * 1.0, Y + 1.0 }) &&
        point_in_rectangle(A, B, C, D, Point{ X + 1.0, Y + 1.0 })) {
        return 1.0;
    }
    int ins = 0;
    for(double i = X + 0.1; i + 0.1 <= X + 1; i += 0.1)
        for(double j = Y + 0.1; j + 0.1 <= Y + 1; j += 0.1)
            if(point_in_rectangle(A, B, C, D, Point{i, j}))
                ins++;
    return ins * 1.0 / 100;
}

// Конструктор
Point::Point(double x, double y) {
    this->x = x;
    this->y = y;
}

// Конструктор
Image::Image(std::string filename) {
	std::ifstream fin(filename, std::ios::binary);
    if(!fin.is_open()) // файл не открылся
        throw std::runtime_error("failed to open file");

    char ch[2];
    fin >> ch[0] >> ch[1];
    if(ch[0] != 'P' || ch[1] != '5') // формат не поддерживается
        throw std::runtime_error("expected P5 format");
    fin >> this->width >> this->height >> this->color_depth;
    this->image.assign(this->height, std::vector<unsigned char>(this->width));
    char pixel;
    fin.read(&pixel, 1);
    for (int i = 0; i < this->height; i++) {
        for (int j = 0; j < this->width; j++) {
            fin.read(&pixel, sizeof(unsigned char));
            this->image[i][j] = pixel;
        }
    }
    fin.close();
}

// Запись картинки в файл
void Image::write(std::string filename) {
	std::ofstream fout(filename, std::ios::binary);
    if(!fout.is_open()) { // файл не открылся
        throw std::runtime_error("cannot open output file");
    }
    fout << "P5\n" << width << ' ' << height << '\n' << color_depth << '\n';
    for(int i = 0; i < height; i ++)
        for(int j = 0; j < width; j ++)
            fout << static_cast<unsigned char>(image[i][j]);
    fout.flush();
    fout.close();
}

// Отрисовка точки
void Image::plot(int x, int y, double brightness, int color, double gamma, bool SRGB) {
    if(x < 0 || x >= this->width || y < 0 || y >= this->height || brightness < 0) {
        return;
    }
    double old = static_cast<double>(this->image[y][x]) / this->color_depth;
    double corrected_color = color * 1.0 / 255;
    if (SRGB) { // SRGB гамма-коррекция
        old = (old < 0.04045 ? old / 12.92 : pow((old + 0.055) / 1.055, gamma));
        corrected_color = (corrected_color < 0.04045 ? corrected_color / 12.92 : pow((corrected_color + 0.055) / 1.055, gamma));
    }
    else {
        old = pow(old, gamma);
        corrected_color = pow(corrected_color, gamma);
    }
    old *= (1.0 - brightness);
    old += brightness * corrected_color;
    if(SRGB)
        old = (old <= 0.0031308 ? old * 12.92 : pow(old,  1.0/gamma)*1.055 - 0.055);
    else
        old = pow(old, 1.0 / gamma);
    if(old >= 0.9999) old = 1.0;
    this->image[y][x] = static_cast<unsigned char>(this->color_depth * old);
}

// Отрисовка линии
void Image::draw_line(Point start, Point end, double thickness, int color, double gamma, bool SRGB) {
    if(start.x == end.x) {
        start.x += 0.5;
        end.x += 0.5;
    }
    if(start.y == end.y) {
        start.y += 0.5;
        end.y += 0.5;
    }
    const Point line_vector = {end.x - start.x, end.y - start.y};
    Point thickness_vector = {1.0, 0.0};
    if(line_vector.x != 0)
        thickness_vector = {-line_vector.y / line_vector.x, 1.0};
    const double thickness_coef = sqrt((thickness*thickness / 4) / (thickness_vector.x*thickness_vector.x + thickness_vector.y*thickness_vector.y));
    Point A = {start.x + thickness_coef*thickness_vector.x, start.y + thickness_coef*thickness_vector.y};
    Point B = {start.x - thickness_coef*thickness_vector.x, start.y - thickness_coef*thickness_vector.y};
    Point C = {end.x + thickness_coef*thickness_vector.x, end.y + thickness_coef*thickness_vector.y};
    Point D = {end.x - thickness_coef*thickness_vector.x, end.y - thickness_coef*thickness_vector.y};
	for(int i = std::max(0, int(std::min(std::min(A.x, B.x), std::min(C.x, D.x))) - 3); i < std::min(width, int(std::max(std::max(A.x, B.x), std::max(C.x, D.x))) + 3); i++) {
        for (int j = std::max(0, int(std::min(std::min(A.y, B.y), std::min(C.y, D.y))) - 3); j < std::min(height, int(std::max(std::max(A.y, B.y), std::max(C.y, D.y))) + 3); j++)
			plot(i, j, intersection_square(A, B, C, D, i, j), color, gamma, SRGB);
    }
}