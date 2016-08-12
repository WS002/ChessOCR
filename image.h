#ifndef IMAGE_H
#define IMAGE_H


#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

class BmpImage
{
public:
	BmpImage();
	~BmpImage();
	void TakeScreenShot(char* filename, std::ofstream& log);
	unsigned char* readBMP(char* filename, std::ofstream& log);
};

#endif
