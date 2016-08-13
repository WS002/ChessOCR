#ifndef IMAGE_H
#define IMAGE_H


#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

class BmpImage
{
private:

LONG width;
LONG height;
DWORD size;
unsigned char *pixels;
BITMAPINFO bmpInfo;
BITMAPFILEHEADER bmpFileHeader;

public:
	BmpImage();
	~BmpImage();
	void TakeScreenShot(std::ofstream& log);
	unsigned char* readBMP(char* filename, std::ofstream& log);
	void saveBMP(char* filename, std::ofstream& log);
	
	//Setter + getter
	unsigned char* getPixels();
	long getWidth();
	long getHeight();
	unsigned int getSize();
};

#endif
