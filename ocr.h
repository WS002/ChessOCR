#ifndef OCR_H
#define OCR_H

#include "image.h"

class OCR: public BmpImage
{
private:
    unsigned char *horizontalDerivatives;
    unsigned char *verticalDerivatives;
    
public:
    OCR():BmpImage()
    {
        horizontalDerivatives = NULL;
        verticalDerivatives = NULL;
    };
    ~OCR();
    void cornerDetection();
    
    void computeHorizontalDerivatives();
    void saveHorizontalBMP(char* filename);
    void computeVerticalDerivatives();
    void saveVerticalBMP(char* filename);
};

#endif