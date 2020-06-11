#include "Image.h"
#include <cmath>
#include <ctime>
#include <fstream>

std::vector<std::vector <int>> ordered_dithering = {
   {0, 48, 12, 60, 3, 51, 15, 63},
   {32, 16, 44, 28, 35, 19, 47, 31},
   {8, 56, 4, 52, 11, 59, 7, 55},
   {40, 24, 36, 20, 43, 27, 39, 23},
   {2, 50, 14, 62, 1, 49, 13, 61},
   {34, 18, 46, 30, 33, 17, 45, 29},
   {10, 58, 6, 54, 9, 57, 5, 53},
   {42, 26, 38, 22, 41, 25, 37, 21},
};
std::vector<std::vector <int>> halftone = {
   {7, 13, 11, 4},
   {12, 16, 14, 8},
   {10, 15, 6, 2},
   {5, 9, 3, 1},
};

// Конструктор
Point::Point(double x, double y) {
    this->x = x;
    this->y = y;
}

// Конструктор
Image::Image(const std::string& filename, bool gradient, double gamma, bool SRGB) {
	// Задаем seed для rand()
    srand(time(nullptr));
    // Открываем файл
	std::ifstream fin(filename, std::ios::binary);
    if (!fin.is_open()) // файл не открылся
        throw std::runtime_error("failed to open file");

	// Читаем хедер
    char ch[2];
    fin >> ch[0] >> ch[1];
    if (ch[0] != 'P' || ch[1] != '5') // формат не поддерживается
        throw std::runtime_error("expected P5 format");
    fin >> this->width >> this->height >> this->color_depth;
    this->image.assign(this->height, std::vector<double>(this->width));

	// Читаем пиксели
    char pixel;
    fin.read(&pixel, 1);
    for (int i = 0; i < this->height; i++) {
        for (int j = 0; j < this->width; j++) {
            if (!gradient) {
                fin.read(&pixel, sizeof(unsigned char));
                double old = static_cast<double>(static_cast<unsigned char>(pixel)) / this->color_depth;
                if (SRGB) { // SRGB гамма-коррекция
                    if (gamma <= 0.0)
                        gamma = 2.4;
                    old = (old < 0.04045 ? old / 12.92 : pow((old + 0.055) / 1.055, gamma));
                }
                else {
                    old = pow(old, gamma);
                }
                this->image[i][j] = old;
            }
            else {
                this->image[i][j] = j * (1.0 / (this->width - 1));
            }
        }
    }
    fin.close();
}

// Запись картинки в файл
void Image::write(const std::string& filename, double gamma, bool SRGB, int bit) {
    this->color_depth = (1 << bit) - 1; // 2 ** bit - 1
    std::ofstream fout(filename, std::ios::binary);
    if (!fout.is_open()) { // файл не открылся
        throw std::runtime_error("cannot open output file");
    }
    fout << "P5\n" << this->width << ' ' << this->height << '\n' << this->color_depth << '\n';
    for (int i = 0; i < this->height; i++) {
        for (int j = 0; j < this->width; j++) {
            double old = this->image[i][j];
            if (SRGB)
                old = (old <= 0.0031308 ? old * 12.92 : pow(old, 1.0 / gamma) * 1.055 - 0.055);
            else
                old = pow(old, 1.0 / gamma);
            const int color = round(old * this->color_depth);
            fout << (static_cast<unsigned char>(color));
        }
    }
    fout.flush();
    fout.close();
}

double sum_between_0_and_1(double A, double B) {
    if (A + B > 1.0)
        return 1.0;
    if (A + B < 0.0)
        return 0.0;
    return A + B;
}

double nearest_color(int bit, double pixel_color) {
    int temp = bit;
    const auto p_c = static_cast<unsigned char>(pixel_color * 255);
    unsigned char Result = p_c;
    while (temp < 8)
    {
        Result = Result >> bit;
        Result += (p_c >> (8 - bit)) << (8 - bit);
        temp += bit;
    }
    return Result;
}

// Дизеринг
void Image::dithering(int bit, int type) {
    errors.assign(height, std::vector<double>(width, 0));

    // Без дизиринга
    if(type == 0) {
        for(int i = 0; i < height; i++)
            for(int j = 0; j < width; j++)
                image[i][j] = nearest_color(bit, image[i][j]);
    }
    // Ordered (8x8)
    else if(type == 1) {
        for (int i = 0; i < height; i++)
            for (int j = 0; j < width; j++)
                image[i][j] = nearest_color(bit, sum_between_0_and_1(image[i][j], ((ordered_dithering[i % 8][j % 8] / 64.0) - 0.5)));
    }
    // Random
    else if(type == 2) {
        for (int i = 0; i < height; i++)
            for (int j = 0; j < width; j++)
                image[i][j] = nearest_color(bit, sum_between_0_and_1(image[i][j], (rand() * 1.0 / (RAND_MAX - 1) - 0.5)));
    }
    // Floyd–Steinberg
    else if(type == 3) {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                image[i][j] = sum_between_0_and_1(image[i][j], errors[i][j]);
                const double color = nearest_color(bit, image[i][j]);
                const double error = (image[i][j] - color) / 16.0;
                image[i][j] = color;
                if (j + 1 < width)
                    errors[i][j + 1] += error * 7;
                if (i + 1 < height) {
                    if (j - 1 >= 0)
                        errors[i + 1][j - 1] += error * 3;
                    errors[i + 1][j] += error * 5;
                    if (j + 1 < width)
                        errors[i + 1][j + 1] += error;
                }
            }
        }
    }
    // Jarvis, Judice, Ninke
    else if (type == 4) {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                image[i][j] = sum_between_0_and_1(image[i][j], errors[i][j]);
                const double color = nearest_color(bit, image[i][j]);
                const double error = (image[i][j] - color) / 48.0;
                image[i][j] = color;
                if (j + 1 < width)
                    errors[i][j + 1] += error * 7;
                if (j + 2 < width)
                    errors[i][j + 2] += error * 5;
                if (i + 1 < height) {
                    if (j - 2 >= 0)
                        errors[i + 1][j - 2] += error * 3;
                    if (j - 1 >= 0)
                        errors[i + 1][j - 1] += error * 5;
                    errors[i + 1][j] += error * 7;
                    if (j + 1 < width)
                        errors[i + 1][j + 1] += (error * 5);
                    if (j + 2 < width)
                        errors[i + 1][j + 2] += (error * 3);
                }
                if (i + 2 < height) {
                    if (j - 2 >= 0)
                        errors[i + 2][j - 2] += (error * 1);
                    if (j - 1 >= 0)
                        errors[i + 2][j - 1] += (error * 3);
                    errors[i + 2][j] += (error * 5);
                    if (j + 1 < width)
                        errors[i + 2][j + 1] += (error * 3);
                    if (j + 2 < width)
                        errors[i + 2][j + 2] += (error * 1);
                }
            }
		}
    }
    // Sierra (Sierra-3)
    else if(type == 5) {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                image[i][j] = sum_between_0_and_1(image[i][j], errors[i][j]);
                const double color = nearest_color(bit, image[i][j]);
                const double error = (image[i][j] - color) / 32.0;
                image[i][j] = color;
                if (j + 1 < width)
                    errors[i][j + 1] += (error * 5);
                if (j + 2 < width)
                    errors[i][j + 2] += (error * 3);
                if (i + 1 < height) {
                    if (j - 2 >= 0)
                        errors[i + 1][j - 2] += (error * 2);
                    if (j - 1 >= 0)
                        errors[i + 1][j - 1] += (error * 4);
                    errors[i + 1][j] += (error * 5);
                    if (j + 1 < width)
                        errors[i + 1][j + 1] += (error * 4);
                    if (j + 2 < width)
                        errors[i + 1][j + 2] += (error * 2);
                }
                if (i + 2 < height)
                {
                    if (j - 1 >= 0)
                        errors[i + 2][j - 1] += (error * 2);
                    errors[i + 2][j] += (error * 3);
                    if (j + 1 < width)
                        errors[i + 2][j + 1] += (error * 2);
                }
            }
        }
    }
    // Atkinson
    else if(type == 6) {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                image[i][j] = sum_between_0_and_1(image[i][j], errors[i][j]);
                const double color = nearest_color(bit, image[i][j]);
                const double error = (image[i][j] - color) / 8.0;
                image[i][j] = color;
                if (j + 1 < width)
                    errors[i][j + 1] += error;
                if (j + 2 < width)
                    errors[i][j + 2] += error;
                if (i + 1 < height)
                {
                    if (j - 1 >= 0)
                        errors[i + 1][j - 1] += error;
                    errors[i + 1][j] += error;
                    if (j + 1 < width)
                        errors[i + 1][j + 1] += error;
                }
                if (i + 2 < height) {
                    errors[i + 2][j] += error;
                }
            }
        }
    }
    // Halftone (4x4, orthogonal)
    else if (type == 7) {
	    for (int i = 0; i < height; i++)
	        for (int j = 0; j < width; j++)
	            image[i][j] = nearest_color(bit, sum_between_0_and_1(image[i][j], (halftone[i % 4][j % 4] / 16.0 - 0.5)));
    }
}