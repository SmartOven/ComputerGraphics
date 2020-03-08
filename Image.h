#pragma once
#include <iostream>
#include <vector>
#include <string>

using std::cout;
using std::cin;
using byte = unsigned char;
using big = unsigned long long int;

struct Pixel {
	byte r;
	byte g;
	byte b;
};

int to_int(std::vector <char>& v);

class Image {
public:
	void set_vars(char* in, char* out);
	void read(char* in);
	void read();
	void write(char* out);
	void write();
	void inversion();
	void vertical_display();
	void horizontal_display();
	void rotate(bool type);
	~Image();
protected:
	void _read(char* in);
	void _write(char* out);
private:
	char* in;
	char* out;
	FILE* file_in;
	FILE* file_out;
	char mode[3]; // P5 or P6
	size_t w, h, d; // width, heigth and color depth
	std::vector <std::vector <byte>> bnw; // for P5
	std::vector <std::vector <Pixel>> rgb; // for P6
};