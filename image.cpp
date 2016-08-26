#include "image.h"


BmpImage::BmpImage()
{
    
}

BmpImage::~BmpImage()
{
	free(pixels);
}

void BmpImage::TakeScreenShot()
{   
	// Simulate print screen
	//keybd_event(VK_MENU, 0, 0, 0); //Alt Press
    keybd_event(VK_SNAPSHOT, 0, 0, 0); // Printscreen press
	
    keybd_event(VK_SNAPSHOT, 0, KEYEVENTF_KEYUP, 0); // Printscreen release
	//keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0); //Alt Release

	// Wait for it to be copied to clipboard
	Sleep(100);
	
    HBITMAP h;
    
    OpenClipboard(NULL);
    h = (HBITMAP)GetClipboardData(CF_BITMAP);
    CloseClipboard();
    HDC hdc=NULL;    
    LPVOID pBuf=NULL;
    BITMAPINFO bmpInfo;
    BITMAPFILEHEADER bmpFileHeader;
    do
    { 
        hdc=GetDC(NULL);
        ZeroMemory(&bmpInfo,sizeof(BITMAPINFO));
        bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
        GetDIBits(hdc,h,0,0,NULL,&bmpInfo,DIB_RGB_COLORS); 
        
        if(bmpInfo.bmiHeader.biSizeImage<=0)
            bmpInfo.bmiHeader.biSizeImage=bmpInfo.bmiHeader.biWidth*abs(bmpInfo.bmiHeader.biHeight)*(bmpInfo.bmiHeader.biBitCount+7)/8;
		
        if((pBuf = malloc(bmpInfo.bmiHeader.biSizeImage))==NULL)
        {
            Log::getInstance().debug("ERROR: Unable to Allocate Bitmap Memory.");
            break;
        } 
        bmpInfo.bmiHeader.biCompression=BI_RGB;
        GetDIBits(hdc,h,0,bmpInfo.bmiHeader.biHeight,pBuf, &bmpInfo, DIB_RGB_COLORS);
       
        Log::getInstance().debug("Image size: ");
	   
		this->width = bmpInfo.bmiHeader.biWidth;
		this->height = abs(bmpInfo.bmiHeader.biHeight);
		this->size 	= bmpInfo.bmiHeader.biWidth * abs(bmpInfo.bmiHeader.biHeight) * 4;
		this->pixels = (unsigned char*) pBuf;
		this->bmpInfo = bmpInfo;
		this->bmpFileHeader = bmpFileHeader;
        
        
    }while(false); 
    
    if(hdc)ReleaseDC(NULL,hdc); 
    
}

void BmpImage::saveBMP(char* filename)
{
	FILE*fp=NULL;
	if((fp = fopen(filename,"wb"))==NULL)
	{      
        Log::getInstance().debug("ERROR: Unable to Create Bitmap File.");
		return;
	}
		
	// pixels is a pointer that points to first element of the array. e.g Get 2nd element by either pixels[1] or *(pixels + 1), which retrieves the next address.
    Log::getInstance().debug("Pixels RGB: ");
    Log::getInstance().debug((int)this->pixels[0]);
    Log::getInstance().debug((int)this->pixels[1]);
    Log::getInstance().debug((int)this->pixels[2]);
    
    Log::getInstance().debug(this->width);
    Log::getInstance().debug(this->height);

	
	this->bmpFileHeader.bfReserved1=0;
	this->bmpFileHeader.bfReserved2=0;
	this->bmpFileHeader.bfSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+bmpInfo.bmiHeader.biSizeImage;
	this->bmpFileHeader.bfType='MB';
	this->bmpFileHeader.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER); 
	fwrite(&this->bmpFileHeader,sizeof(BITMAPFILEHEADER),1,fp);
	fwrite(&this->bmpInfo.bmiHeader,sizeof(BITMAPINFOHEADER),1,fp);
	fwrite((LPVOID)this->pixels,this->bmpInfo.bmiHeader.biSizeImage,1,fp); 

	if(fp)fclose(fp);
}


void BmpImage::grayscale(bool binarize)
{
    for(int i = 3; i < this->size; i+=4)
    {
        //Red i - 3
        //Green i - 2
        //Blue i - 1
        //Reserved(alpha) i
        
        double red =( (double) this->pixels[i-3] )/ 255.0f;
        double green =( (double) this->pixels[i-2])/ 255.0f; 
        double blue =( (double) this->pixels[i-1] )/ 255.0f;
        
        double grayColor = 0.2126f * red + 0.7152f * green + 0.0722f * blue;
                
        if(binarize) 
            if(grayColor > 0.5f)
                grayColor = 1.0f;
            else grayColor = 0.0f;
        
        
        double scaledGray = 255.0f * grayColor;
        
        this->pixels[i-3] = (unsigned char) scaledGray;
        this->pixels[i-2] = (unsigned char) scaledGray;
        this->pixels[i-1] = (unsigned char) scaledGray;
        this->pixels[i] =  (unsigned char) 0;
                
    }
}

// Gaussian blur
void BmpImage::blur()
{
     // Apply the gaussian kernel 3x3
    for(int i = 3; i < this->size; i += 4)
    {        
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
            
            // Blue
            int currentB = i - 1;
            int leftB = i - 5;
            int rightB = i + 5;
            int topB = i + (this->width * 4) - 1;
            int topLeftB = topB - 4;
            int topRightB = topB + 4;
            int bottomB = i - (this->width * 4) - 1;
            int bottomLeftB = bottomB - 4;
            int bottomRightB = bottomB + 4;
            
            // Green
            int currentG = i - 2;
            int leftG = leftB - 1;
            int rightG = rightB - 1;
            int topG = topB - 1;
            int topLeftG = topLeftB - 1;
            int topRightG = topRightB - 1;
            int bottomG = bottomB - 1;
            int bottomLeftG = bottomLeftB - 1;
            int bottomRightG = bottomRightB - 1;
            
            // Red
            int currentR = i - 3;
            int leftR = leftG - 1;
            int rightR = rightG - 1;
            int topR = topG - 1;
            int topLeftR = topLeftG - 1;
            int topRightR = topRightG - 1;
            int bottomR = bottomG - 1;
            int bottomLeftR = bottomLeftG - 1;
            int bottomRightR = bottomRightG - 1;
            
            //R
            double redValue = (double)this->pixels[currentR] * 0.25f 
                            + (double)this->pixels[leftR] * 0.125f
                            + (double)this->pixels[rightR] * 0.125f
                            + (double)this->pixels[topR] * 0.125f
                            + (double)this->pixels[bottomR] * 0.125f
                            + (double)this->pixels[topLeftR] * 0.0625f
                            + (double)this->pixels[topRightR] * 0.0625f
                            + (double)this->pixels[bottomLeftR] * 0.0625f
                            + (double)this->pixels[bottomRightR] * 0.0625f;
            
            this->pixels[i-3] = (unsigned char) redValue;
            //G
            double greenValue =   (double)this->pixels[currentG] * 0.25f 
                                + (double)this->pixels[leftG] * 0.125f
                                + (double)this->pixels[rightG] * 0.125f
                                + (double)this->pixels[topG] * 0.125f
                                + (double)this->pixels[bottomG] * 0.125f
                                + (double)this->pixels[topLeftG] * 0.0625f
                                + (double)this->pixels[topRightG] * 0.0625f
                                + (double)this->pixels[bottomLeftG] * 0.0625f
                                + (double)this->pixels[bottomRightG] * 0.0625f;
            this->pixels[i-2] = (unsigned char) greenValue;
            //B
            double blueValue = (double)this->pixels[currentB] * 0.25f 
                             + (double)this->pixels[leftB] * 0.125f
                             + (double)this->pixels[rightB] * 0.125f
                             + (double)this->pixels[topB] * 0.125f
                             + (double)this->pixels[bottomB] * 0.125f
                             + (double)this->pixels[topLeftB] * 0.0625f
                             + (double)this->pixels[topRightB] * 0.0625f
                             + (double)this->pixels[bottomLeftB] * 0.0625f
                             + (double)this->pixels[bottomRightB] * 0.0625f;
            this->pixels[i-1] = (unsigned char) blueValue;
            //alpha
            this->pixels[i] =(unsigned char) 0.0f;
        }
    }
}

// Gaussian blur
void BmpImage::blur(unsigned char* image)
{
     // Apply the gaussian kernel 3x3
    for(int i = 3; i < this->size; i += 4)
    {        
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
            
            // Blue
            int currentB = i - 1;
            int leftB = i - 5;
            int rightB = i + 5;
            int topB = i + (this->width * 4) - 1;
            int topLeftB = topB - 4;
            int topRightB = topB + 4;
            int bottomB = i - (this->width * 4) - 1;
            int bottomLeftB = bottomB - 4;
            int bottomRightB = bottomB + 4;
            
            // Green
            int currentG = i - 2;
            int leftG = leftB - 1;
            int rightG = rightB - 1;
            int topG = topB - 1;
            int topLeftG = topLeftB - 1;
            int topRightG = topRightB - 1;
            int bottomG = bottomB - 1;
            int bottomLeftG = bottomLeftB - 1;
            int bottomRightG = bottomRightB - 1;
            
            // Red
            int currentR = i - 3;
            int leftR = leftG - 1;
            int rightR = rightG - 1;
            int topR = topG - 1;
            int topLeftR = topLeftG - 1;
            int topRightR = topRightG - 1;
            int bottomR = bottomG - 1;
            int bottomLeftR = bottomLeftG - 1;
            int bottomRightR = bottomRightG - 1;
            
            //R
            double redValue = (double)image[currentR] * 0.25f 
                            + (double)image[leftR] * 0.125f
                            + (double)image[rightR] * 0.125f
                            + (double)image[topR] * 0.125f
                            + (double)image[bottomR] * 0.125f
                            + (double)image[topLeftR] * 0.0625f
                            + (double)image[topRightR] * 0.0625f
                            + (double)image[bottomLeftR] * 0.0625f
                            + (double)image[bottomRightR] * 0.0625f;
            
            image[i-3] = (unsigned char) redValue;
            //G
            double greenValue =   (double)image[currentG] * 0.25f 
                                + (double)image[leftG] * 0.125f
                                + (double)image[rightG] * 0.125f
                                + (double)image[topG] * 0.125f
                                + (double)image[bottomG] * 0.125f
                                + (double)image[topLeftG] * 0.0625f
                                + (double)image[topRightG] * 0.0625f
                                + (double)image[bottomLeftG] * 0.0625f
                                + (double)image[bottomRightG] * 0.0625f;
            image[i-2] = (unsigned char) greenValue;
            //B
            double blueValue = (double)image[currentB] * 0.25f 
                             + (double)image[leftB] * 0.125f
                             + (double)image[rightB] * 0.125f
                             + (double)image[topB] * 0.125f
                             + (double)image[bottomB] * 0.125f
                             + (double)image[topLeftB] * 0.0625f
                             + (double)image[topRightB] * 0.0625f
                             + (double)image[bottomLeftB] * 0.0625f
                             + (double)image[bottomRightB] * 0.0625f;
            image[i-1] = (unsigned char) blueValue;
            //alpha
            image[i] =(unsigned char) 0.0f;
        }
    }
}

/*
unsigned char* BmpImage::readBMP(char* filename)
{
    int i;
    FILE* f = fopen(filename, "rb");
    unsigned char info[54];
    fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

    // extract image height and width from header
    int width = *(int*)&info[18];
    int height = *(int*)&info[22];

	this->width = (LONG) width;
	this->height = (LONG) height;
	
	
    int size = 3 * width * height;
	this->size 	= (DWORD) size;
    unsigned char* data = new unsigned char[size]; // allocate 3 bytes per pixel
    fread(data, sizeof(unsigned char), size, f); // read the rest of the data at once
    fclose(f);

    for(i = 0; i < size; i += 3)
    {
            unsigned char tmp = data[i];
            data[i] = data[i+2];
            data[i+2] = tmp;
    }

    return data;
}
*/

// Setter and Getters
unsigned char* BmpImage::getPixels() 
{
	return this->pixels;
}

long BmpImage::getWidth()
{
	return (long) this->width;
}

long BmpImage::getHeight()
{
	(long) this->height;
}

unsigned int BmpImage::getSize()
{
	(unsigned int) this->size;
}

