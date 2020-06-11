#include "Image.h"
#include <algorithm>
#include <stdexcept>
#include <cmath>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

unsigned char correct_color(unsigned char color, int offset, double coefficient) {
    int result = (static_cast<double>(color) - offset) * coefficient;
    if (result > 255)
        result = 255;
    if (result < 0)
        result = 0;
    return result;
}

//  онструктор
Pixel::Pixel(unsigned char a, unsigned char b, unsigned char c) : a(a), b(b), c(c) {}

void Pixel::RGB_to_YCbCr_601() {
    // R G B
    const double r = a / 255.0;
    const double g = b / 255.0;
    const double b = c / 255.0;

    // Coefficients for R G B
    const double coef_r = 0.299;
    const double coef_g = 0.587;
    const double coef_b = 0.114;

    // Y Cb Cr
    const double Y = coef_r * r + coef_g * g + coef_b * b;
    const double Cb = (b - Y) / (2.0 * (1.0 - coef_b));
    const double Cr = (r - Y) / (2.0 * (1.0 - coef_r));

    // Output
    this->a = static_cast<unsigned char>(std::min(255, std::max(0, static_cast<int>(std::round(Y * 255)))));
    this->b = static_cast<unsigned char>(std::min(255, std::max(0, static_cast<int>(std::round((Cb + 0.5) * 255)))));
    this->c = static_cast<unsigned char>(std::min(255, std::max(0, static_cast<int>(std::round((Cr + 0.5) * 255)))));
}

void Pixel::YCbCr_601_to_RGB() {
    // Y Cb Cr
    const double Y = a / 255.0;
    const double Cb = b / 255.0 - 0.5;
    const double Cr = c / 255.0 - 0.5;

    // Coefficients for R G B
    const double coef_r = 0.299;
    const double coef_g = 0.587;
    const double coef_b = 0.114;

    // R G B
    const double r = Y + (2.0 - 2.0 * coef_r) * Cr;
    const double g = Y - coef_b * (2.0 - 2.0 * coef_b) * Cb / coef_g - coef_r * (2 - 2.0 * coef_r) * Cr / coef_g;
    const double b = Y + (2.0 - 2.0 * coef_b) * Cb;

    // Output
    this->a = static_cast<unsigned char>(std::min(255, std::max(0, static_cast<int>(std::round(r * 255)))));
    this->b = static_cast<unsigned char>(std::min(255, std::max(0, static_cast<int>(std::round(g * 255)))));
    this->c = static_cast<unsigned char>(std::min(255, std::max(0, static_cast<int>(std::round(b * 255)))));
}


//  онструктор
Image::Image(const std::string& filename) {
    // ќткрываем файл
    std::ifstream fin(filename, std::ios::binary);
    if (!fin.is_open())
        throw std::runtime_error("failed to open file");

    // „итаем хедер
    fin >> this->format_0 >> this->format_1;
	if (this->format_0 != 'P' || (this->format_1 != '5' && this->format_1 != '6')) {
        throw std::runtime_error("expected P5 or P6 format");
	}
    fin >> this->width >> this->height >> this->color_depth;
    if(this->color_depth != 255)
        throw std::runtime_error("only 255 color depth is supported");
    this->image.assign(this->height, std::vector <Pixel>(this->width));

    // „итаем пиксели
    char color;
    fin.read(&color, 1);
    for (int i = 0; i < this->height; i++) {
        for (int j = 0; j < this->width; j++) {
            this->image[i][j] = Pixel();
            
            fin.read(&color, sizeof(unsigned char));
            this->image[i][j].a = color;
            this->image[i][j].b = color;
            this->image[i][j].c = color;
            if (this->format_1 == '6') {
                fin.read(&color, sizeof(unsigned char));
                this->image[i][j].b = color;

                fin.read(&color, sizeof(unsigned char));
                this->image[i][j].c = color;
            }
        }
    }
    fin.close();
}

// «апись картинки в файл(-ы)
void Image::write(std::string filename) {
    std::ofstream fout(filename, std::ios::binary);
    if(!fout.is_open()) {
        throw std::runtime_error("cannot open output file");
    }
    
    fout << format_0 << format_1 << "\n" << width << ' ' << height << '\n' << color_depth << '\n';
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fout << image[i][j].a;
            if (format_1 == '6') {
                fout << image[i][j].b;
                fout << image[i][j].c;
            }
        }
    }
    fout.flush();
    fout.close();
}

void Image::correct_brightness(int type, int offset, double coefficient) {
	if (type % 2 == 1) {
		// ѕереводим в YCbCr601
        for (int i = 0; i < this->height; i++) {
            for (int j = 0; j < this->width; j++) {
                this->image[i][j].RGB_to_YCbCr_601();
            }
        }
	}
	if (type == 0) {
        if (format_1)
        {
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    image[i][j].a = correct_color(image[i][j].a, offset, coefficient);
                    image[i][j].b = correct_color(image[i][j].b, offset, coefficient);
                    image[i][j].c = correct_color(image[i][j].c, offset, coefficient);
                }
            }
        }
        else
        {
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    int result = image[i][j].a = correct_color(image[i][j].a, offset, coefficient);
                    image[i][j].a = result;
                    image[i][j].b = result;
                    image[i][j].c = result;
                }
            }
        }
    }
    if (type == 1) {
        if (format_1) {
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    image[i][j].a = correct_color(image[i][j].a, offset, coefficient);
                }
            }
        }
        else {
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    int result = correct_color(image[i][j].a, offset, coefficient);
                    image[i][j].a = result;
                    image[i][j].b = result;
                    image[i][j].c = result;
                }
            }
        }
    }
	if (type == 2) {
        unsigned char min = 255;
        unsigned char max = 0;
        if (format_1) {
            for (int i = 0; i < this->height; i++) {
                for (int j = 0; j < this->width; j++) {
                    // »щем минимум
                    min = std::min(min, image[i][j].a);
                    min = std::min(min, image[i][j].b);
                    min = std::min(min, image[i][j].c);

                    // »щем максимум
                    max = std::max(max, image[i][j].a);
                    max = std::max(max, image[i][j].b);
                    max = std::max(max, image[i][j].c);
                }
            }
        }
        else {
            for (int i = 0; i < this->height; i++) {
                for (int j = 0; j < this->width; j++) {
                    // »щем минимум
                    min = std::min(min, image[i][j].a);
                    // »щем максимум
                    max = std::max(max, image[i][j].a);
                }
            }
        }
        offset = static_cast<int>(min);
        coefficient = 255.0 / (static_cast<int>(max) - static_cast<int>(min));
        std::cout << offset << " " << coefficient << "\n";
        for (int i = 0; i < this->height; i++) {
            for (int j = 0; j < this->width; j++) {
                this->image[i][j].a = correct_color(this->image[i][j].a, offset, coefficient);
                this->image[i][j].b = correct_color(this->image[i][j].b, offset, coefficient);
                this->image[i][j].c = correct_color(this->image[i][j].c, offset, coefficient);
            }
        }
	}
	if (type == 3) {
        unsigned char min = 255;
        unsigned char max = 0;
        for (int i = 0; i < this->height; i++) {
            for (int j = 0; j < this->width; j++) {
                // »щем минимум
                min = std::min(min, image[i][j].a);
                // »щем максимум
                max = std::max(max, image[i][j].a);
            }
        }
        offset = static_cast<int>(min);
        coefficient = 255.0 / (static_cast<int>(max) - static_cast<int>(min));
        std::cout << offset << " " << coefficient << "\n";
        if (this->format_1 == '6') {
            for (int i = 0; i < this->height; i++) {
                for (int j = 0; j < this->width; j++) {
                    this->image[i][j].a = correct_color(this->image[i][j].a, offset, coefficient);
                }
            }
        }
        else {
            for (int i = 0; i < this->height; i++) {
                for (int j = 0; j < this->width; j++) {
                    this->image[i][j].a = correct_color(this->image[i][j].a, offset, coefficient);
                    this->image[i][j].b = correct_color(this->image[i][j].b, offset, coefficient);
                    this->image[i][j].c = correct_color(this->image[i][j].c, offset, coefficient);;
                }
            }
        }
	}
	if (type == 4) {
		// —писок встречаемости €ркости цвета
        std::vector <unsigned long long int> color_count(256, 0);
        auto ignoring_count = static_cast<long long int>(0.03901 * this->width * this->height);
		// —читаем все встречаемости €ркостей
        for (int i = 0; i < this->height; i++) {
            for (int j = 0; j < this->width; j++) {
                color_count[this->image[i][j].a]++;
                color_count[this->image[i][j].b]++;
                color_count[this->image[i][j].c]++;
            }
        }
        int max_ignored_brightness = 255; // верхн€€ граница игнорировани€
        int min_ignored_brightness = 0; // нижн€€ граница игнорировани€
		while (ignoring_count > 0) {
            ignoring_count -= color_count[max_ignored_brightness];
			if (ignoring_count >= 0) {
                max_ignored_brightness--;
			}
		}
        ignoring_count = static_cast<long long int>(0.03901 * this->width * this->height);
        while (ignoring_count > 0) {
            ignoring_count -= color_count[min_ignored_brightness];
            if (ignoring_count >= 0) {
                min_ignored_brightness++;
            }
        }

        offset = static_cast<int>(min_ignored_brightness);
        coefficient = 255.0 / (static_cast<int>(max_ignored_brightness) - static_cast<int>(min_ignored_brightness));
        std::cout << offset << " " << coefficient << "\n";
		for (int i = 0; i < this->height; i++) {
            for (int j = 0; j < this->width; j++) {
                this->image[i][j].a = correct_color(this->image[i][j].a, offset, coefficient);
                this->image[i][j].b = correct_color(this->image[i][j].b, offset, coefficient);
                this->image[i][j].c = correct_color(this->image[i][j].c, offset, coefficient);
            }
        }
	}
    if (type == 5) {
        // —писок встречаемости €ркости цвета
        std::vector <long long int> color_count(256, 0);
        auto ignoring_count = static_cast<long long int>(0.03901 * this->width * this->height);
        // —читаем все встречаемости €ркостей
        for (int i = 0; i < this->height; i++) {
            for (int j = 0; j < this->width; j++) {
                color_count[this->image[i][j].a]++;
            }
        }
        int max_ignored_brightness = 255; // верхн€€ граница игнорировани€
        int min_ignored_brightness = 0; // нижн€€ граница игнорировани€
        while (ignoring_count > 0) {
            ignoring_count -= color_count[max_ignored_brightness];
            if (ignoring_count >= 0) {
                max_ignored_brightness--;
            }
        }
        ignoring_count = static_cast<long long int>(0.03901 * this->width * this->height);
        while (ignoring_count > 0) {
            ignoring_count -= color_count[min_ignored_brightness];
            if (ignoring_count >= 0) {
                min_ignored_brightness++;
            }
        }

        offset = static_cast<int>(min_ignored_brightness);
        coefficient = 255.0 / (static_cast<int>(max_ignored_brightness) - static_cast<int>(min_ignored_brightness));
        std::cout << offset << " " << coefficient << "\n";
    	if (this->format_1 == '6') {
            for (int i = 0; i < this->height; i++) {
                for (int j = 0; j < this->width; j++) {
                    this->image[i][j].a = correct_color(this->image[i][j].a, offset, coefficient);
                }
            }
        }
        else {
            for (int i = 0; i < this->height; i++) {
                for (int j = 0; j < this->width; j++) {
                    this->image[i][j].a = correct_color(this->image[i][j].a, offset, coefficient);
                    this->image[i][j].b = correct_color(this->image[i][j].b, offset, coefficient);
                    this->image[i][j].c = correct_color(this->image[i][j].c, offset, coefficient);
                }
            }
        }
    }
    if (type % 2 == 1) {
        // ѕереводим обратно в RGB
        for (int i = 0; i < this->height; i++) {
            for (int j = 0; j < this->width; j++) {
                this->image[i][j].YCbCr_601_to_RGB();
            }
        }
    }
}
