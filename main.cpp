#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "image.h"

// TODO: add header file for image.cpp and import it instead. 
// TODO: add ocr files, put all vision algorithms there
// TODO: add button (main.cpp), on click to initiate image object, take a screenshot(image.cpp), save it(image.cpp), read it(image.cpp ) 
//       and calls the ocr algorithms.(all of this ocr.cpp)


int main(int argc, char *argv[])
{
    // Log file
    std::ofstream log;
    log.open ("log.txt");
    
    char imagePath[] = "unusedName.bmp";
    
    BmpImage bmp;
    // TODO: add constructor to image.cpp
    // Takes screenshot and saves it in BMP format. 
    bmp.TakeScreenShot(imagePath, log);
    
    // Reads the pixels of the screenshotted BMP image and saves them to pixels. 
    unsigned char* pixels;
    pixels = bmp.readBMP(imagePath, log);
    
    // pixels is a pointer that points to first element of the array. e.g Get 2nd element by either pixels[1] or *(pixels + 1), which retrieves the next address.
    log << "RED " <<(int)pixels[0] << "\n";
    log << "GREEN " <<(int)pixels[1] << "\n";
    log << "BLUE " <<(int)pixels[2] << "\n";
    
    log.close();
    
    // readBMP dynamically allocates memory for the pixels. Need to be deleted after use. TODO: This should be in the image.cpp class.
    delete pixels;

    return 0;
}

