#include <iostream>
#include <string>
#include "Image.h"

int main(int argc, char *argv[]) {
    if(argc != 7) { // аргументов не 7
        std::cerr << "Invalid command line arguments!" << "\n";
        return 1;
    }
	
    const std::string fin = std::string(argv[1]);
    const std::string fout = std::string(argv[2]);
    bool gradient;
    int type, bit;
    bool SRGB = false;
    double gamma;
	
    try {
        gradient = (std::string(argv[3]) == "1");
        type = atoi(argv[4]);
        bit = atoi(argv[5]);
        if(std::string(argv[6]) != "0" && std::string(argv[6]) != "0.0") {
	        gamma = std::stold(argv[6]);
        }
    	else {
	        gamma = 2.4;
	        SRGB = true;
        }
    }
    catch (const std::exception& error) {
	    std::cerr << error.what() << "\n";
        return 1;
    }
    
    Image* image;
    try {    
        image = new Image(fin, gradient, gamma, SRGB);
    }
    catch (const std::exception& error) {
	    std::cerr << error.what() << "\n";
        return 1;
    }

    image->dithering(bit, type);
    try {
        image->write(fout, gamma, SRGB, bit);
    }
    catch (const std::exception& error) {
	    std::cerr << error.what() << "\n";
        delete image;
        return 1;
    }
	
    delete image;
    return 0;
}
