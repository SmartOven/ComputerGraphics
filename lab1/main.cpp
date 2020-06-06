#include <iostream>
#include "Image.h"
using std::cout;
using std::cin;

int main(int argc, char** argv) {
	Image img1;
	if (argc != 4) {
		cout << "Unsupported count of arguments\n";
		exit(1);
	}
	img1.set_vars(argv[1], argv[2]);
	img1.read();
	if (argv[3][0] == '0')
		img1.inversion();
	else if (argv[3][0] == '1')
		img1.horizontal_display();
	else if (argv[3][0] == '2')
		img1.vertical_display();
	else if (argv[3][0] == '3')
		img1.rotate(0);
	else if (argv[3][0] == '4')
		img1.rotate(1);
	else {
		cout << "Unsupported type of operation\n";
		exit(1);
	}
	img1.write();
	return 0;
}