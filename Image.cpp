#define _CRT_SECURE_NO_WARNINGS
#include "Image.h"

int to_int(std::vector <char>& v) {
	int result = 0;
	size_t size = v.size();
	for (size_t i = 0; i < size; i++) {
		result += (int)std::pow(10.0, (double)i) * (int)(v[size - i - 1] - '0');
	}
	return result;
};

void Image::set_vars(char* in, char* out) {
	this->in = in;
	this->out = out;
};

void Image::read(char* in) {
	return _read(in);
};

void Image::read() {
	if (!this->in) {
		cout << "Input file is not defined\n";
		exit(1);
	}
	else return _read(this->in);
};

void Image::_read(char* in) {
	this->file_in = fopen(in, "rb");
	if (!this->file_in) {
		cout << "Unable to open input file\n";
		exit(1);
	}
	fseek(this->file_in, 0, SEEK_END); // end of file
	big current_size = ftell(this->file_in); // size of file
	if (current_size < 16) { // not enough even for header
		cout << "File is too small\n";
		exit(1);
	}
	fseek(this->file_in, 0, SEEK_SET); // start of file

	// Reading Header
	fread(this->mode, 1, 2, this->file_in); // reading format (P5 or P6)
	fseek(this->file_in, 1, SEEK_CUR); // passing '\n'
	char buffer = fgetc(this->file_in);
	while (buffer != ' ') { // reading W
		this->_w.push_back(buffer);
		buffer = fgetc(this->file_in);
	}
	buffer = fgetc(this->file_in);
	while (buffer != '\n') { // reading H
		this->_h.push_back(buffer);
		buffer = fgetc(this->file_in);
	}
	buffer = fgetc(this->file_in);
	while (buffer != '\n') { // reading D
		this->_d.push_back(buffer);
		buffer = fgetc(this->file_in);
	}
	this->w = to_int(this->_w);
	this->h = to_int(this->_h);
	this->d = to_int(this->_d);
	
	big required_size = ftell(this->file_in);
	if (this->mode[1] == '5')
		required_size += (big)(this->w * this->h);
	else
		required_size += (big)(3 * this->w * this->h);
	if (current_size < required_size) {
		cout << "File size is too small\n";
		exit(1);
	}
	if (this->mode[1] == '5') {
		this->bnw.assign(h, std::vector<byte>(w)); // setting size of vector
		for (size_t i = 0; i < this->h; i++) {
			for (size_t j = 0; j < this->w; j++) {
				this->bnw[i][j] = fgetc(this->file_in);
			}
		}
	}
	else if (this->mode[1] == '6') {
		this->rgb.assign(h, std::vector<Pixel>(w)); // setting size of vector
		for (size_t i = 0; i < this->h; i++) {
			for (size_t j = 0; j < this->w; j++) {
				this->rgb[i][j].r = fgetc(this->file_in);
				this->rgb[i][j].g = fgetc(this->file_in);
				this->rgb[i][j].b = fgetc(this->file_in);
			}
		}
	}
	else {
		cout << "Unsupported format\n"; // if format is not P5 or P6
		exit(1);
	}
	fclose(this->file_in);
};

void Image::write(char* out) {
	return _write(out);
};

void Image::write() {
	if (!this->out) {
		cout << "Output file is not defined\n";
		exit(1);
	}
	else return _write(this->out);
};

void Image::_write(char* out) {
	this->file_out = fopen(out, "wb");
	if (!this->file_out) {
		cout << "Unable to open output file\n";
		exit(1);
	}
	fprintf(this->file_out, "%c%c\n%i %i\n%i\n", mode[0], mode[1], w, h, d); // writing header
	// writing pixels
	if (this->mode[1] == '6') {
		for (size_t i = 0; i < this->h; i++) {
			for (size_t j = 0; j < this->w; j++) {
				fprintf(this->file_out, "%c%c%c", this->rgb[i][j].r, this->rgb[i][j].g, this->rgb[i][j].b);
			}
		}
	}
	else {
		for (size_t i = 0; i < this->h; i++) {
			for (size_t j = 0; j < this->w; j++) {
				fprintf(this->file_out, "%c", this->bnw[i][j]);
			}
		}
	}
	fclose(this->file_out);
};

Image::~Image() {
	if (this->in) fclose(this->file_in);
	if (this->out) fclose(this->file_out);
};

void Image::inversion() {
	if (this->mode[1] == '5')
		for (size_t i = 0; i < this->h; ++i)
			for (size_t j = 0; j < this->w; ++j)
				this->bnw[i][j] = byte(this->d - this->bnw[i][j]);
	else
		for (size_t i = 0; i < this->h; ++i)
			for (size_t j = 0; j < this->w; ++j) {
				this->rgb[i][j].r = byte(this->d - this->rgb[i][j].r);
				this->rgb[i][j].g = byte(this->d - this->rgb[i][j].g);
				this->rgb[i][j].b = byte(this->d - this->rgb[i][j].b);
			}
};

void Image::vertical_display() {
	if (this->mode[1] == '5')
		for (size_t i = 0; i < this->h; i++)
			for (size_t j = 0; j < this->w / 2; j++)
				std::swap(this->bnw[i][j], this->bnw[i][w - j - 1]);
	else
		for (size_t i = 0; i < this->h; i++)
			for (size_t j = 0; j < this->w / 2; j++) {
				std::swap(this->rgb[i][j].r, this->rgb[i][w - j - 1].r);
				std::swap(this->rgb[i][j].g, this->rgb[i][w - j - 1].g);
				std::swap(this->rgb[i][j].b, this->rgb[i][w - j - 1].b);
			}
};

void Image::horizontal_display() {
	if (this->mode[1] == '5')
		for (size_t i = 0; i < this->h / 2; i++)
			for (size_t j = 0; j < this->w; j++)
				std::swap(this->bnw[i][j], this->bnw[h - i - 1][j]);
	else
		for (size_t i = 0; i < this->h / 2; i++)
			for (size_t j = 0; j < this->w; j++) {
				std::swap(this->rgb[i][j].r, this->rgb[h - i - 1][j].r);
				std::swap(this->rgb[i][j].g, this->rgb[h - i - 1][j].g);
				std::swap(this->rgb[i][j].b, this->rgb[h - i - 1][j].b);
			}
};

void Image::rotate(bool type) {
	// 0 - clock wise
	// 1 - counter clock wise
	if (this->mode[1] == '5') {
		std::vector <std::vector <byte>> new_bnw;
		int new_h = this->w;
		int new_w = this->h;
		new_bnw.assign(this->w, std::vector<byte>(this->h));
		if (!type)
			for (size_t i = 0; i < this->w; i++)
				for (size_t j = 0; j < this->h; j++)
					new_bnw[i][j] = this->bnw[new_w - j - 1][i];
		else
			for (size_t i = 0; i < this->w; i++)
				for (size_t j = 0; j < this->h; j++)
					new_bnw[i][j] = this->bnw[j][new_h - i - 1];
		this->w = new_w;
		this->h = new_h;
		this->bnw = new_bnw;
	}
	else {
		std::vector <std::vector <Pixel>> new_rgb;
		int new_h = this->w;
		int new_w = this->h;
		new_rgb.assign(this->w, std::vector<Pixel>(this->h));
		if (!type)
			for (size_t i = 0; i < this->w; i++)
				for (size_t j = 0; j < this->h; j++) {
					new_rgb[i][j].r = this->rgb[new_w - j - 1][i].r;
					new_rgb[i][j].g = this->rgb[new_w - j - 1][i].g;
					new_rgb[i][j].b = this->rgb[new_w - j - 1][i].b;
				}
		else
			for (size_t i = 0; i < this->w; i++)
				for (size_t j = 0; j < this->h; j++) {
					new_rgb[i][j].r = this->rgb[j][new_h - i - 1].r;
					new_rgb[i][j].g = this->rgb[j][new_h - i - 1].g;
					new_rgb[i][j].b = this->rgb[j][new_h - i - 1].b;
				}
		this->w = new_w;
		this->h = new_h;
		this->rgb = new_rgb;
	}
};