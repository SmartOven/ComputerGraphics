#include <iostream>
#include <string>
#include "Image.h"

int main(int argc, char *argv[]) {
    if(argc != 9 && argc != 10) { // аргументов меньше 9 или больше 10
	    std::cerr << "Invalid command line arguments!" << "\n";
        return 1;
    }
    const std::string fin = std::string(argv[1]);
    const std::string fout = std::string(argv[2]);
    double thickness, x_s, y_s, x_f, y_f, gamma = 2.4;
    bool SRGB = true;
    int brightness;
    try {
        brightness = atoi(argv[3]);
        thickness = std::stod(argv[4]);
        x_s = std::stod(argv[5]);
        y_s = std::stod(argv[6]);
        x_f = std::stod(argv[7]);
        y_f = std::stod(argv[8]);
    }
    catch (const std::exception& error) { // неправильные аргументы
	    std::cerr << error.what() << "\n";
        return 1;
    }
    if(argc == 10) {
        try {
            gamma = std::stod(argv[9]);
            SRGB = false;
        }
        catch (const std::exception& error) { // неправильные аргументы
	        std::cerr << error.what() << std::endl;
			return 1;
        }
    }

    Image* img;
    try {
        img = new Image(fin);
    }
    catch (const std::exception& error) { // ошибка, связанная с выделением памяти/открытием файла/чтением файла
	    std::cerr << error.what() << "\n";
        return 1;
    }

    img->draw_line(Point{ x_s, y_s }, Point{ x_f, y_f }, thickness, brightness, gamma, SRGB);
    try {
        img->write(fout);
    }
    catch (const std::exception& error) { // ошибка открытия файла/записи в файл
	    std::cerr << error.what() << "\n";
        delete img;
        return 1;
    }
    delete img;
    return 0;
}