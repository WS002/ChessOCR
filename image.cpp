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
	Sleep(2000);
	
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

