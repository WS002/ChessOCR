#ifndef IMAGE_H
#define IMAGE_H

#include "log.h"
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

class BmpImage
{
protected:

LONG width;
LONG height;
DWORD size;
unsigned char *pixels;
BITMAPINFO bmpInfo;
BITMAPFILEHEADER bmpFileHeader;

public:
	BmpImage();
	~BmpImage();
	void TakeScreenShot();
	//unsigned char* readBMP(char* filename);
	void saveBMP(char* filename);
    void grayscale(bool binarize = false);
    void blur();
	
	//Setter + getter
	unsigned char* getPixels();
	long getWidth();
	long getHeight();
	unsigned int getSize();
};

#endif
