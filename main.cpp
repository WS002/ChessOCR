#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include "image.h"

// TODO: add header file for image.cpp and import it instead. 
// TODO: add ocr files, put all vision algorithms there
// TODO: add button (main.cpp), on click to initiate image object, take a screenshot(image.cpp), save it(image.cpp), read it(image.cpp ) 
//       and calls the ocr algorithms.(all of this ocr.cpp)


int main(int argc, char *argv[])
{
    char imagePath[] = "whatever.bmp";
    
    BmpImage *bmp = new BmpImage();
    // TODO: add constructor to image.cpp
    // Takes screenshot and saves it in BMP format. 
    bmp->TakeScreenShot();
    bmp->grayscale(true);
	bmp->saveBMP(imagePath);
    
    // Reads the pixels of the screenshotted BMP image and saves them to pixels. 
    unsigned char* pixels;
    //pixels = bmp.readBMP(imagePath, log);
	pixels = bmp->getPixels();

	/*do 
	{
		std::cout << '\n' << "Press a key to continue...";
	} while (std::cin.get() != '\n');
	*/
	delete bmp;
    return 0;
}

