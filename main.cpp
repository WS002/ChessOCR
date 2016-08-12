
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "image.cpp"

// TODO: add header file for image.cpp and import it instead. 
// TODO: add ocr files, put all vision algorithms there
// TODO: add button, on click to initiate image object, which automatically takes a screenshot, saves it, reads it( all of this image.cpp ) 
//       and calls the ocr algorithms.(all of this ocr.cpp)


int main(int argc, char *argv[])
{
    // Log file
    std::ofstream log;
    log.open ("log.txt");
    
    char imagePath[] = "unusedName.bmp";
    
    // Takes screenshot and saves it in BMP format. TODO: This should be replace with an image constructor
    TakeScreenShot(imagePath, log);
    
    // Reads the pixels of the screenshotted BMP image and saves them to pixels. TODO: This should be in the image.cpp constructor.
    unsigned char* pixels;
    pixels = readBMP(imagePath, log);
    
    // pixels is a pointer that points to first element of the array. e.g Get 2nd element by either pixels[1] or *(pixels + 1), which retrieves the next address.
    log << "RED " <<(int)pixels[0] << "\n";
    log << "GREEN " <<(int)pixels[1] << "\n";
    log << "BLUE " <<(int)pixels[2] << "\n";
    
    log.close();
    
    // readBMP dynamically allocates memory for the pixels. Need to be deleted after use. TODO: This should be in the image.cpp class.
    delete pixels;
    
    return 0;
}

