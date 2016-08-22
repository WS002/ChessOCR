#include "ocr.h"


OCR::~OCR()
{
    if(NULL != this->horizontalDerivatives)
    {
        delete horizontalDerivatives;
        this->horizontalDerivatives = NULL;
    }
    if(NULL != this->verticalDerivatives)
    {
        delete verticalDerivatives;
        this->verticalDerivatives = NULL;
    }
}

void OCR::cornerDetection()
{
    // Harris corner detection
    
    // optional: Gaussian blur the image
    
    // compute horizontal derivatives image
    // compute vertical derivatives image
    
    // define gaussian kernel and the structure tensor matrix
    
    // Compute the score det(H) - k*(Trace(H))^2 and a certain threshold
}

void OCR::computeHorizontalDerivatives()
{
    if(NULL != this->horizontalDerivatives)
    {
        delete horizontalDerivatives;
        this->horizontalDerivatives = NULL;
    }
    // Use sobel operator
    this->horizontalDerivatives = new double[this->size];
    
    // copy border pixels
    for(int border = 3; border < this->size; border += 4)
    {
        // Bottom border
        if(border < this->width * 4)
        {
            //R
            this->horizontalDerivatives[border-3] = this->pixels[border-3];
            //G
            this->horizontalDerivatives[border-2] = this->pixels[border-2];
            //B
            this->horizontalDerivatives[border-1] = this->pixels[border-1];
            //alpha
            this->horizontalDerivatives[border] = this->pixels[border];
        
        }
        // Top border
        if(border > this->size - (this->width * 4))
        {
            //R
            this->horizontalDerivatives[border-3] = this->pixels[border-3];
            //G
            this->horizontalDerivatives[border-2] = this->pixels[border-2];
            //B
            this->horizontalDerivatives[border-1] = this->pixels[border-1];
            //alpha
            this->horizontalDerivatives[border] = this->pixels[border];
            
        }
        // Left border
        if(border % (4*this->width) == 3)
        {
            //R
            this->horizontalDerivatives[border-3] = this->pixels[border-3];
            //G
            this->horizontalDerivatives[border-2] = this->pixels[border-2];
            //B
            this->horizontalDerivatives[border-1] = this->pixels[border-1];
            //alpha
            this->horizontalDerivatives[border] = this->pixels[border];
        }
        
        // Right border
        if((border+1) % (4*this->width) == 0)
        {
             //R
            this->horizontalDerivatives[border-3] = this->pixels[border-3];
            //G
            this->horizontalDerivatives[border-2] = this->pixels[border-2];
            //B
            this->horizontalDerivatives[border-1] = this->pixels[border-1];
            //alpha
            this->horizontalDerivatives[border] = this->pixels[border];        
        }
        
    }
}

void OCR::computeVerticalDerivatives()
{
     if(NULL != this->verticalDerivatives)
    {
        delete verticalDerivatives;
        this->verticalDerivatives = NULL;
    }
    // Use sobel operator
    this->verticalDerivatives = new double[this->size];
}