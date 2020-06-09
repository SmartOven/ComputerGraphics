#include "Image.h"
#include <algorithm>
#include <stdexcept>
#include <cmath>
#include <string>
#include <vector>
#include <fstream>

// Парсим название шаблона файла, чтобы получить названия трех файлов
std::vector <std::string> parse_pattern(std::string pattern) {
    int separator_index = -1;
    for (size_t i = 0; i < pattern.size(); i++)
        if (pattern[i] == '.')
            separator_index = i;
    if(separator_index == -1)
        throw std::runtime_error("pattern has no dots, so doesn't match name.ext");

    const std::string name = pattern.substr(0, separator_index);
    const std::string format = pattern.substr(separator_index);
    return { name + "_1" + format,
    		 name + "_2" + format,
    		 name + "_3" + format };
}

// Конструктор
Pixel::Pixel(unsigned char a, unsigned char b, unsigned char c) : a(a), b(b), c(c) {}

// Конструктор
Image::Image(const std::string& filename, int count) {
    if(count == 1) {
    	// Открываем файл
        std::ifstream fin(filename, std::ios::binary);
        if (!fin.is_open())
            throw std::runtime_error("failed to open file");

    	// Читаем хедер
        char ch[2];
        fin >> ch[0] >> ch[1];
        if (ch[0] != 'P' || ch[1] != '6')
            throw std::runtime_error("expected P6 format");
        fin >> this->width >> this->height >> this->color_depth;
        if(this->color_depth != 255)
            throw std::runtime_error("only 255 color depth is supported");
        this->image.assign(this->height, std::vector <Pixel>(this->width));

    	// Читаем пиксели
        char color;
        fin.read(&color, 1);
        for (int i = 0; i < this->height; i++) {
            for (int j = 0; j < this->width; j++) {
                this->image[i][j] = Pixel();
            	
                fin.read(&color, sizeof(unsigned char));
                this->image[i][j].a = color;
            	
                fin.read(&color, sizeof(unsigned char));
                this->image[i][j].b = color;
            	
                fin.read(&color, sizeof(unsigned char));
                this->image[i][j].c = color;
            }
        }
        fin.close();
    }
	else {
		// Находим названия входных файлов
	    std::vector <std::string> files = parse_pattern(filename);
        if(files.size() != 3)
            throw std::runtime_error("unknown error during name generation");
		
        this->image.assign(this->height, std::vector <Pixel>(this->width));
        for(int i = 0; i < 3; i++) {
        	// Открываем файл
	        std::ifstream fin(files[i], std::ios::binary);
            if(!fin.is_open()) 
                throw std::runtime_error("failed to open file " + files[i]);

        	// Читаем хедер
            char ch[2];
            fin >> ch[0] >> ch[1];
            if (ch[0] != 'P' || ch[1] != '5')
                throw std::runtime_error("expected P5 format");
            fin >> this->width >> this->height >> this->color_depth;
            if (this->color_depth != 255)
                throw std::runtime_error("only 255 color depth is supported");

            // Читаем пиксели
            char color;
            fin.read(&color, 1);
            for (int i = 0; i < this->height; i++) {
                for (int j = 0; j < this->width; j++) {
                    fin.read(&color, sizeof(unsigned char));
                    if (i == 0)
                        this->image[i][j].a = color;
                    if (i == 1)
                        this->image[i][j].b = color;
                    if (i == 2)
                        this->image[i][j].c = color;
                }
            }
            fin.close();
        }
    }
}

// Запись картинки в файл(-ы)
void Image::write(std::string filename, int count) {
    if(count == 1) {
        std::ofstream fout(filename, std::ios::binary);
        if(!fout.is_open()) {
            throw std::runtime_error("cannot open output file");
        }
    	
        fout << "P6\n" << width << ' ' << height << '\n' << color_depth << '\n';
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                fout << image[i][j].a;
                fout << image[i][j].b;
                fout << image[i][j].c;
            }
        }
        fout.flush();
        fout.close();
    } else {
	    std::vector<std::string> files = parse_pattern(filename);
        if(files.size() != 3)
            throw std::runtime_error("unknown error during name generation");
    	
        for(int i = 0; i < 3; i ++) {
            std::ofstream fout(files[i], std::ios::binary);
            if(!fout.is_open()) {
                throw std::runtime_error("cannot open output file " + files[i]);
            }
        	
            fout << "P5\n" << width << ' ' << height << '\n' << color_depth << '\n';
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    if (i == 0)
                        fout << image[i][j].a;
                    if (i == 1)
                        fout << image[i][j].b;
                    if (i == 2)
                        fout << image[i][j].c;
                }
            }
            fout.flush();
            fout.close();
        }
    }
    
}

void Image::change_color_space(std::string from, std::string to) {
    // Конвертация из from в RGB
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (from == "HSL")
                image[i][j].HSL_to_RGB();
            if (from == "HSV")
                image[i][j].HSV_to_RGB();
            if (from == "YCbCr.601")
                image[i][j].YCbCr_601_to_RGB();
            if (from == "YCbCr.709")
                image[i][j].YCbCr_709_to_RGB();
            if (from == "YCoCg")
                image[i][j].YCoCg_to_RGB();
            if (from == "CMY")
                image[i][j].CMY_to_RGB();
        }
    }

    // Конвертация из RGB в to
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (to == "HSL")
                image[i][j].RGB_to_HSL();
            if (to == "HSV")
                image[i][j].RGB_to_HSV();
            if (to == "YCbCr.601")
                image[i][j].RGB_to_YCbCr_601();
            if (to == "YCbCr.709")
                image[i][j].RGB_to_YCbCr_709();
            if (to == "YCoCg")
                image[i][j].RGB_to_YCoCg();
            if (to == "CMY")
                image[i][j].RGB_to_CMY();
        }
    }
}

void Pixel::HSL_to_RGB() {
	// H S L
	const double h = a / 255.0;
	const double s = b / 255.0;
	const double l = c / 255.0;
	
    double q;
    if (l < 0.5)
        q = l * (s + 1.0);
    else
        q = l + s - (l * s);
	const double p = l * 2 - q;

	// R
    double r;
    double tr = h + 1.0 / 3.0;
    if (tr < 0)
        tr += 1.0;
    else if (tr > 1.0)
        tr -= 1.0;
    if (tr < 1.0 / 6.0)
        r = p + (q - p) * 6.0 * tr;
    else if (tr >= 1.0 / 6.0 && tr < 0.5)
        r = q;
    else if (tr >= 0.5 && tr < 2.0 / 3.0)
        r = p + (q - p) * (2.0 / 3.0 - tr) * 6.0;
    else
        r = p;

	// G
    double g;
    double tg = h;
    if (tg < 0)
        tg += 1.0;
    else if (tg > 1.0)
        tg -= 1.0;
    if (tg < 1.0 / 6.0)
        g = p + (q - p) * 6.0 * tg;
    else if (tg >= 1.0 / 6.0 && tg < 0.5)
        g = q;
    else if (tg >= 0.5 && tg < 2.0 / 3.0)
        g = p + (q - p) * (2.0 / 3.0 - tg) * 6.0;
    else
        g = p;

	// B
    double b;
    double tb = h - 1.0 / 3.0;
    if (tb < 0)
        tb += 1.0;
    else if (tb > 1.0)
        tb -= 1.0;
    if(tb < 1.0 / 6.0)
        b = p + (q - p) * 6.0 * tb;
    else if(tb >= 1.0 / 6.0 && tb < 0.5)
        b = q;
    else if(tb >= 0.5 && tb < 2.0 / 3.0)
        b = p + (q - p) * (2.0 / 3.0 - tb) * 6.0;
    else
        b = p;

    // Output
    this->a = static_cast<unsigned char>(r * 255);
    this->b = static_cast<unsigned char>(g * 255);
    this->c = static_cast<unsigned char>(b * 255);
}

void Pixel::RGB_to_HSL() {
	// R G B
	const double r = a * 1.0 / 255;
    const double g = b * 1.0 / 255;
    const double b = c * 1.0 / 255;
    const double max = std::max(r, std::max(g, b));
    const double min = std::min(r, std::min(g, b));

	// H S L
	double h;
    if(max == min)
        h = 0;
    else if(r == max && g >= b)
        h = (g - b) / (max - min) * 60;
    else if(r == max && g < b)
        h = (g - b) / (max - min) * 60 + 360;
    else if(max == g)
        h = (b - r) / (max-min) * 60 + 120;
    else
        h = (r - g) / (max-min) * 60 + 240;
	const double l = (max + min) / 2;
	const double s = (max - min) / (1 - abs(1 - (max + min)));

    // Output
    this->a = static_cast<unsigned char>(round(h * 255 / 360));
    this->b = static_cast<unsigned char>(round(s * 255));
    this->c = static_cast<unsigned char>(round(l * 255));
}

void Pixel::HSV_to_RGB() {
	// H S V
	const double h = a / 255.0 * 360.0;
	const double s = b / 255.0;
	const double v = c / 255.0;
	
	const double c = v * s;
	const double x = c * (1 - abs((static_cast<int>(h / 60)) % 2 + (h / 60 - static_cast<int>(h / 60)) - 1));
	const double m = v - c;

	// R G B
    double r, g, b;
    if(h >= 0 && h <= 60)
        r = c, g = x, b = 0;
    else if(h >= 60 && h <= 120)
        r = x, g = c, b = 0;
    else if(h >= 120 && h <= 180)
        r = 0, g = c, b = x;
    else if(h >= 180 && h <= 240)
        r = 0, g = x, b = c;
    else if(h >= 240 && h <= 300)
        r = x, g = 0, b = c;
    else
        r = c, g = 0, b = x;

	// Output
    this->a = static_cast<unsigned char>(std::min(255, std::max(0, static_cast<int>((r + m) * 255))));
    this->b = static_cast<unsigned char>(std::min(255, std::max(0, static_cast<int>((g + m) * 255))));
    this->c = static_cast<unsigned char>(std::min(255, std::max(0, static_cast<int>((b + m) * 255))));
}

void Pixel::RGB_to_HSV() {
	// R G B
	const double r = a * 1.0 / 255;
	const double g = b * 1.0 / 255;
	const double b = c * 1.0 / 255;

    // H S V
	const double v = std::max(r, std::max(g, b));
	const double diff = v - std::min(r, std::min(g, b));
    double h;
    if(diff == 0)
        h = 0;
    else if(v == r)
        h = (g - b) / diff;
    else if(v == g)
        h = 2 + (b - r) / diff;
    else
        h = 4 + (r - g) / diff;
    h *= 60.0;
    if(h < 0)
        h += 360;
	const double s = (v == 0 ? 0 : diff / v);

	// Output
    this->a = static_cast<unsigned char>(round(h / 360.0 * 255.0));
    this->b = static_cast<unsigned char>(round(s * 255.0));
    this->c = static_cast<unsigned char>(round(v * 255.0));
}

void Pixel::CMY_to_RGB() {
    this->a = static_cast<unsigned char>(255 - a);
    this->b = static_cast<unsigned char>(255 - b);
    this->c = static_cast<unsigned char>(255 - c);
}

void Pixel::RGB_to_CMY() {
    this->a = static_cast<unsigned char>(255 - a);
    this->b = static_cast<unsigned char>(255 - b);
    this->c = static_cast<unsigned char>(255 - c);
}

void Pixel::RGB_to_YCoCg() {
	// R G B
	const double r = a * 1.0 / 255;
	const double g = b * 1.0 / 255;
	const double b = c * 1.0 / 255;

	// Y Co Cg
    double y = std::min(0.0, std::max(1.0, static_cast<double>(r / 4 + g / 2 + b / 4)));
    double co = std::min(0.0, std::max(1.0, static_cast<double>(r / 2 - b / 2 + 0.5)));
    double cg = std::min(0.0, std::max(1.0, static_cast<double>(-r / 4 + g / 2 - b / 4 + 0.5)));

	// Output
    this->a = static_cast<unsigned char>(y * 255);
    this->b = static_cast<unsigned char>(co * 255);
    this->c = static_cast<unsigned char>(cg * 255);
}

void Pixel::YCoCg_to_RGB() {
	// Y Co Cg
	const double y = a * 1.0 / 255;
	const double co = b * 1.0 / 255 - 0.5;
	const double cg = c * 1.0 / 255 - 0.5;

	// R G B
	const double r = std::min(0.0, std::max(1.0, static_cast<double>(y + co - cg)));
	const double g = std::min(0.0, std::max(1.0, static_cast<double>(y + cg)));
	const double b = std::min(0.0, std::max(1.0, static_cast<double>(y - co - cg)));

	// Output
    this->a = static_cast<unsigned char>(r * 255);
    this->b = static_cast<unsigned char>(g * 255);
    this->c = static_cast<unsigned char>(b * 255);
}

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
    this->a = static_cast<unsigned char>(std::min(255, std::max(0, static_cast<int>(Y * 255))));
    this->b = static_cast<unsigned char>(std::min(255, std::max(0, static_cast<int>((Cb + 0.5) * 255))));
    this->c = static_cast<unsigned char>(std::min(255, std::max(0, static_cast<int>((Cr + 0.5) * 255))));
}

void Pixel::YCbCr_601_to_RGB() {
	// Y Cb Cr
	const double Y = a / 255.0;
	const double Cb = b / 255.0 - 0.5;
	const double Cr = c / 255.0 - 0.5;
	
    // Inverted coefficients for R G B
	const double coef_r = 0.587;
	const double coef_g = 0.114;
	const double coef_b = 0.299;

	// R G B
	const double r = Y + (2.0 - 2.0 * coef_r) * Cr;
	const double g = Y - coef_b * (2.0 - 2.0 * coef_b) * Cb / coef_g - coef_r * (2 - 2.0 * coef_r) * Cr / coef_g;
	const double b = Y + (2.0 - 2.0 * coef_b) * Cb;

	// Output
    this->a = static_cast<unsigned char>(std::min(255, std::max(0, static_cast<int>(r * 255))));
    this->b = static_cast<unsigned char>(std::min(255, std::max(0, static_cast<int>(g * 255))));
    this->c = static_cast<unsigned char>(std::min(255, std::max(0, static_cast<int>(b * 255))));
}

void Pixel::RGB_to_YCbCr_709() {
	// R G B
	const double r = a / 255.0;
	const double g = b / 255.0;
	const double b = c / 255.0;

	// Coefficients for R G B
	const double coef_r = 0.2126;
	const double coef_g = 0.7152;
    const double coef_b = 0.0722;

	// Y Cb Cr
	const double Y = coef_r * r + coef_g * g + coef_b * b;
	const double Cb = (b - Y) / (2.0 * (1.0 - coef_b));
	const double Cr = (r - Y) / (2.0 * (1.0 - coef_r));

	// Output
    this->a = static_cast<unsigned char>(std::min(255, std::max(0, static_cast<int>(Y * 255))));
    this->b = static_cast<unsigned char>(std::min(255, std::max(0, static_cast<int>((Cb + 0.5) * 255))));
    this->c = static_cast<unsigned char>(std::min(255, std::max(0, static_cast<int>((Cr + 0.5) * 255))));
}

void Pixel::YCbCr_709_to_RGB() {
	// Y Cb Cr
	const double Y = a / 255.0;
	const double Cb = b / 255.0 - 0.5;
	const double Cr = c / 255.0 - 0.5;

    // Inverted coefficients for R G B
	const double coef_b = 0.0722;
	const double coef_r = 0.2126;
	const double coef_g = 0.7152;

	// R G B
	const double r = Y + (coef_r * -2.0 + 2.0) * Cr;
	const double g = Y - coef_b * (2.0 - 2.0 * coef_b) * Cb / coef_g - coef_r * (2.0 - 2.0 * coef_r) * Cr / coef_g;
	const double b = Y + (coef_b * -2.0 + 2.0) * Cb;

	// Output
    this->a = static_cast<unsigned char>(std::min(255, std::max(0, static_cast<int>(r * 255))));
    this->b = static_cast<unsigned char>(std::min(255, std::max(0, static_cast<int>(g * 255))));
    this->c = static_cast<unsigned char>(std::min(255, std::max(0, static_cast<int>(b * 255))));
}