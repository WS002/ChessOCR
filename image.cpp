#include "image.h"


BmpImage::BmpImage()
{

}

BmpImage::~BmpImage()
{
	free(pixels);
}

void BmpImage::TakeScreenShot(std::ofstream& log)
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
            log << "ERROR: Unable to Allocate Bitmap Memory.\n";  
            break;
        } 
        bmpInfo.bmiHeader.biCompression=BI_RGB;
        GetDIBits(hdc,h,0,bmpInfo.bmiHeader.biHeight,pBuf, &bmpInfo, DIB_RGB_COLORS);
       
		log << bmpInfo.bmiHeader.biSizeImage << "\n";
	   
		this->width = bmpInfo.bmiHeader.biWidth;
		this->height = abs(bmpInfo.bmiHeader.biHeight);
		this->size 	= bmpInfo.bmiHeader.biSizeImage;
		this->pixels = (unsigned char*) pBuf;
		this->bmpInfo = bmpInfo;
		this->bmpFileHeader = bmpFileHeader;
        
        
    }while(false); 
    
    if(hdc)ReleaseDC(NULL,hdc); 
    
}

void BmpImage::saveBMP(char* filename, std::ofstream& log)
{
	FILE*fp=NULL;
	if((fp = fopen(filename,"wb"))==NULL)
	{            
		log << "ERROR: Unable to Create Bitmap File.\n";
		return;
	}
		
		
	// pixels is a pointer that points to first element of the array. e.g Get 2nd element by either pixels[1] or *(pixels + 1), which retrieves the next address.
    log << "RED " <<(int)this->pixels[0] << "\n";
    log << "GREEN " <<(int)this->pixels[1] << "\n";
    log << "BLUE " <<(int)this->pixels[2] << "\n";
    log << sizeof(this->pixels) << "\n";
	log << this->size << "\n";
	
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


unsigned char* BmpImage::readBMP(char* filename, std::ofstream& log)
{
    int i;
    FILE* f = fopen(filename, "rb");
    unsigned char info[54];
    fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

    // extract image height and width from header
    int width = *(int*)&info[18];
    int height = *(int*)&info[22];

	this->width = (LONG) width;;
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

