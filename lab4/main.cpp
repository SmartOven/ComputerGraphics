#include <iostream>
#include <string>
#include <vector>
#include "Image.h"

int main(int argc, char* argv[]) {
	// Обработка аргументов командной строки
	std::string from, to, input, output;
    int input_count = 0;
	int output_count = 0;
    for(int i = 1; i < argc; i++) {
        if(std::string(argv[i]) == "-f") {
            from = std::string(argv[i + 1]);
        }
        if(std::string(argv[i]) == "-t") {
            to = std::string(argv[i + 1]);
        }
        if(std::string(argv[i]) == "-i") {
            input_count = atoi(argv[i + 1]);
            input = std::string(argv[i + 2]);
        }
        if(std::string(argv[i]) == "-o") {
            output_count = atoi(argv[i + 1]);
            output = std::string(argv[i + 2]);
        }
    }

	// Проверка корректности всех аргументов командной строки
    bool input_format_is_correct = false;
	bool output_format_is_correct = false;
	// Номера форматов (указаны в классе Pixel): 1      2      3         4            5          6       7
	std::vector <std::string> color_spaces = { "RGB", "HSL", "HSV", "YCbCr.601", "YCbCr.709", "YCoCg", "CMY" };
    for (const auto& color_space : color_spaces) {
        if(color_space == from)
            input_format_is_correct = true;
        if(color_space == to)
            output_format_is_correct = true;
    }
    if (!input_format_is_correct ||
        !output_format_is_correct ||
        argc != 11 ||
        from.empty() || to.empty() || input.empty() || output.empty() ||
        input_count != 1 && input_count != 3 ||
        output_count != 1 && output_count != 3)
    {
	    std::cerr << "command line arguments are invalid" << "\n";
        return 1;
    }

	// Работа с объектом класса Image
    Image* img;
    try {
        img = new Image(input, input_count);
    }
    catch(const std::exception& error) {
	    std::cerr << error.what() << '\n';
        return 1;
    }

    img->change_color_space(from, to);
    try {
        img->write(output, output_count);
    }
    catch(const std::exception& error) {
	    std::cerr << error.what() << '\n';
        delete img;
        return 1;
    }
	
    delete img;
    return 0;
}