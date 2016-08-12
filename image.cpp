#include "image.h"

BmpImage::BmpImage()
{

}

BmpImage::~BmpImage()
{

}

void BmpImage::TakeScreenShot(char* filename, std::ofstream& log)
{   
  
    keybd_event(VK_SNAPSHOT, 0x45, KEYEVENTF_EXTENDEDKEY, 0);
    keybd_event(VK_SNAPSHOT, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
    HBITMAP h;
    
    OpenClipboard(NULL);
    h = (HBITMAP)GetClipboardData(CF_BITMAP);
    CloseClipboard();
    HDC hdc=NULL;
    FILE*fp=NULL;
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
        if((fp = fopen(filename,"wb"))==NULL)
        {            
            log << "ERROR: Unable to Create Bitmap File.\n";
            break;
        }         
        
        bmpFileHeader.bfReserved1=0;
        bmpFileHeader.bfReserved2=0;
        bmpFileHeader.bfSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+bmpInfo.bmiHeader.biSizeImage;
        bmpFileHeader.bfType='MB';
        bmpFileHeader.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER); 
        fwrite(&bmpFileHeader,sizeof(BITMAPFILEHEADER),1,fp);
        fwrite(&bmpInfo.bmiHeader,sizeof(BITMAPINFOHEADER),1,fp);
        fwrite(pBuf,bmpInfo.bmiHeader.biSizeImage,1,fp); 
    }while(false); 
    
    if(hdc)ReleaseDC(NULL,hdc); 
    if(pBuf) free(pBuf); 
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

    int size = 3 * width * height;
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

