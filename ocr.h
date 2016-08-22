#ifndef OCR_H
#define OCR_H

#include "image.h"

class OCR: public BmpImage
{
private:
    double *horizontalDerivatives;
    double *verticalDerivatives;
    
public:
    OCR():BmpImage()
    {
        horizontalDerivatives = NULL;
        verticalDerivatives = NULL;
    };
    ~OCR();
    void cornerDetection();
    
    void computeHorizontalDerivatives();
    void computeVerticalDerivatives();
};

#endif