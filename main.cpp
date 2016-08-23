#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include "ocr.h"

// TODO: add header file for image.cpp and import it instead. 
// TODO: add ocr files, put all vision algorithms there
// TODO: add button (main.cpp), on click to initiate image object, take a screenshot(image.cpp), save it(image.cpp), read it(image.cpp ) 
//       and calls the ocr algorithms.(all of this ocr.cpp)


int main(int argc, char *argv[])
{
    char imagePath[] = "whatever.bmp";
    char horizontalImagePath[] = "whateverHorizontal.bmp";
    char verticalImagePath[] = "whateverVertical.bmp";
    
    OCR *ocr = new OCR();
    // TODO: add constructor to image.cpp
    // Takes screenshot and saves it in BMP format. 
    ocr->TakeScreenShot();    
    ocr->blur();
    ocr->grayscale();
	ocr->saveBMP(imagePath);
    ocr->computeHorizontalDerivatives();
    ocr->saveHorizontalBMP(horizontalImagePath);
    ocr->computeVerticalDerivatives();
    ocr->saveVerticalBMP(verticalImagePath);
    
	delete ocr;
    return 0;
}

