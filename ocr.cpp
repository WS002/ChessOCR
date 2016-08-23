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
    this->blur();

 // grayscale the image ( binarize = true?)
    this->grayscale();
    
    
    
 // compute horizontal derivatives image
    char horizontalImagePath[] = "whateverHorizontal.bmp";
    this->computeHorizontalDerivatives();
    this->saveHorizontalBMP(horizontalImagePath);
    
// compute vertical derivatives image
    char verticalImagePath[] = "whateverVertical.bmp";
    this->computeVerticalDerivatives();
    this->saveVerticalBMP(verticalImagePath);

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
    this->horizontalDerivatives = new unsigned char[this->size];
    
    // copy border pixels
    for(int border = 3; border < this->size; border += 4)
    {
        // Borders in this order: bottom, top, left, right
        if(( border < this->width * 4) || (border > this->size - (this->width * 4)) 
        ||  (border % (4*this->width) == 3) || ((border+1) % (4*this->width) == 0) )
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
    
    // Apply the sobel kernel 3x3
    for(int i = 3; i < this->size; i += 4)
    {
        /*  
            Filter for horizontal differences in a 3x3 neighbourhood.
            Explore and apply to all i (not border) pixels
            
            |-1|0|1|
            |-2|i|2| 
            |-1|0|1| 
            
        */
        
        //Ignore border pixels
        if(!( i < this->width * 4) && !(i > this->size - (this->width * 4)) 
        &&  !(i % (4*this->width) == 3) && !((i+1) % (4*this->width) == 0) )
        {
            // current pixels       i
            // left pixel           i - 4
            // right pixel          i + 4
            // top pixel            i + (width*4)
            // top left pixel       top pixel - 4
            // top right pixels     top pixel + 4
            // bottom pixel         i - (width*4)
            // bottom left pixel    bottom pixel - 4
            // bottom right pixel   bottom pixel + 4
            int left = i - 5;
            int right = i + 5;
            int top = i + (this->width * 4) - 1;
            int topLeft = top - 4;
            int topRight = top + 4;
            int bottom = i - (this->width * 4) - 1;
            int bottomLeft = bottom - 4;
            int bottomRight = bottom + 4;
            
            double pixelValue = abs(this->pixels[left] * -2.0f + this->pixels[right] * 2.0f - this->pixels[topLeft] + this->pixels[topRight] - this->pixels[bottomLeft] + this->pixels[bottomRight]);
            if(pixelValue > 255.0f)
                pixelValue = 255.0f;
            //R
            this->horizontalDerivatives[i-3] = (unsigned char) pixelValue;
            //G
            this->horizontalDerivatives[i-2] = (unsigned char) pixelValue;
            //B
            this->horizontalDerivatives[i-1] = (unsigned char) pixelValue;
            //alpha
            this->horizontalDerivatives[i] =(unsigned char) 0.0f;
            
        }
        
    }
    
    
}

void OCR::saveHorizontalBMP(char* filename)
{
	FILE*fp=NULL;
	if((fp = fopen(filename,"wb"))==NULL)
	{      
        Log::getInstance().debug("ERROR: Unable to Create Bitmap File.");
		return;
	}

	
	this->bmpFileHeader.bfReserved1=0;
	this->bmpFileHeader.bfReserved2=0;
	this->bmpFileHeader.bfSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+bmpInfo.bmiHeader.biSizeImage;
	this->bmpFileHeader.bfType='MB';
	this->bmpFileHeader.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER); 
	fwrite(&this->bmpFileHeader,sizeof(BITMAPFILEHEADER),1,fp);
	fwrite(&this->bmpInfo.bmiHeader,sizeof(BITMAPINFOHEADER),1,fp);
	fwrite((LPVOID)this->horizontalDerivatives,this->bmpInfo.bmiHeader.biSizeImage,1,fp); 

	if(fp)fclose(fp);
}

void OCR::computeVerticalDerivatives()
{
     if(NULL != this->verticalDerivatives)
    {
        delete verticalDerivatives;
        this->verticalDerivatives = NULL;
    }
    // Use sobel operator
    this->verticalDerivatives = new unsigned char[this->size];
    
        // copy border pixels
    for(int border = 3; border < this->size; border += 4)
    {
        // Borders in this order: bottom, top, left, right
        if(( border < this->width * 4) || (border > this->size - (this->width * 4)) 
        ||  (border % (4*this->width) == 3) || ((border+1) % (4*this->width) == 0) )
        {
            //R
            this->verticalDerivatives[border-3] = this->pixels[border-3];
            //G
            this->verticalDerivatives[border-2] = this->pixels[border-2];
            //B
            this->verticalDerivatives[border-1] = this->pixels[border-1];
            //alpha
            this->verticalDerivatives[border] = this->pixels[border];
        
        }
                           
    }
    
     // Apply the sobel kernel 3x3
    for(int i = 3; i < this->size; i += 4)
    {
        /*  
            Filter for horizontal differences in a 3x3 neighbourhood.
            Explore and apply to all i (not border) pixels
            
            |-1|-2|-1|
            |0 |0 | 0| 
            |1 |2 | 1| 
            
        */
        
        //Ignore border pixels
        if(!( i < this->width * 4) && !(i > this->size - (this->width * 4)) 
        &&  !(i % (4*this->width) == 3) && !((i+1) % (4*this->width) == 0) )
        {
            // current pixels       i
            // left pixel           i - 4
            // right pixel          i + 4
            // top pixel            i + (width*4)
            // top left pixel       top pixel - 4
            // top right pixels     top pixel + 4
            // bottom pixel         i - (width*4)
            // bottom left pixel    bottom pixel - 4
            // bottom right pixel   bottom pixel + 4
            int left = i - 5;
            int right = i + 5;
            int top = i + (this->width * 4) - 1;
            int topLeft = top - 4;
            int topRight = top + 4;
            int bottom = i - (this->width * 4) - 1;
            int bottomLeft = bottom - 4;
            int bottomRight = bottom + 4;
            
            double pixelValue = abs(this->pixels[top] * -2.0f + this->pixels[bottom] * 2.0f - this->pixels[topLeft] - this->pixels[topRight] + this->pixels[bottomLeft] + this->pixels[bottomRight]);
            if(pixelValue > 255.0f)
                pixelValue = 255.0f;
            //R
            this->verticalDerivatives[i-3] = (unsigned char) pixelValue;
            //G
            this->verticalDerivatives[i-2] = (unsigned char) pixelValue;
            //B
            this->verticalDerivatives[i-1] = (unsigned char) pixelValue;
            //alpha
            this->verticalDerivatives[i] =(unsigned char) 0.0f;
            
        }
        
    }
}

void OCR::saveVerticalBMP(char* filename)
{
	FILE*fp=NULL;
	if((fp = fopen(filename,"wb"))==NULL)
	{      
        Log::getInstance().debug("ERROR: Unable to Create Bitmap File.");
		return;
	}

	
	this->bmpFileHeader.bfReserved1=0;
	this->bmpFileHeader.bfReserved2=0;
	this->bmpFileHeader.bfSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+bmpInfo.bmiHeader.biSizeImage;
	this->bmpFileHeader.bfType='MB';
	this->bmpFileHeader.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER); 
	fwrite(&this->bmpFileHeader,sizeof(BITMAPFILEHEADER),1,fp);
	fwrite(&this->bmpInfo.bmiHeader,sizeof(BITMAPINFOHEADER),1,fp);
	fwrite((LPVOID)this->verticalDerivatives,this->bmpInfo.bmiHeader.biSizeImage,1,fp); 

	if(fp)fclose(fp);
}
