#include <iostream>
#include <string>
#include "Image.h"

int main(int argc, char* argv[]) {
	// Обработка аргументов командной строки и проверка их корректности
    double offset = -1;
    double coefficient = -1;
	if (argc < 4) {
        std::cerr << "Invalid command line arguments!" << "\n";
        return 1;
	}
    std::string input = std::string(argv[1]);
	std::string output = std::string(argv[2]);
    int type = atoi(argv[3]);
	if (type < 0 || type > 5) {
        std::cout << type << "\n";
        std::cerr << "Invalid type of operation!" << "\n";
        return 1;
	}
    if (type < 2) {
        if (argc == 6) {
            offset = atof(argv[4]);
            coefficient = atof(argv[5]);
        }
        else {
            std::cerr << "Invalid command line arguments!" << "\n";
            return 1;
        }
    }

	/*
	const std::string input = "wb.pgm";
	const std::string output = "wb_0.pgm";
	const int type = 4;
	const int offset = 50;
	const double coefficient = 1.25;
	*/
	// Работа с объектом класса Image
    Image* img;
    try {
        img = new Image(input);
    }
    catch(const std::exception& error) {
	    std::cerr << error.what() << '\n';
        return 1;
    }

    img->correct_brightness(type, offset, coefficient);
    try {
        img->write(output);
    }
    catch(const std::exception& error) {
	    std::cerr << error.what() << '\n';
        delete img;
        return 1;
    }
	
    delete img;
    return 0;
}
