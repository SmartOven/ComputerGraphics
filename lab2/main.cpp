#include <iostream>
#include <string>
#include "Image.h"

int main(int argc, char *argv[]) {
    if(argc != 9 && argc != 10) { // ���������� ������ 9 ��� ������ 10
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
    catch (const std::exception& error) { // ������������ ���������
	    std::cerr << error.what() << "\n";
        return 2;
    }
    if(argc == 10) {
        try {
            gamma = std::stod(argv[9]);
            SRGB = false;
        }
        catch (const std::exception& error) { // ������������ ���������
	        std::cerr << error.what() << std::endl;
			return 3;
        }
    }

    Image* img;
    try {
        img = new Image(fin);
    }
    catch (const std::exception& error) { // ������, ��������� � ���������� ������/��������� �����/������� �����
	    std::cerr << error.what() << "\n";
        return 4;
    }

    img->draw_line(Point{ x_s, y_s }, Point{ x_f, y_f }, thickness, brightness, gamma, SRGB);
    try {
        img->write(fout);
        delete img;
    }
    catch (const std::exception& error) { // ������ �������� �����/������ � ����
	    std::cerr << error.what() << "\n";
        return 5;
    }
    return 0;
}