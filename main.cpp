
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <fstream>



void TakeScreenShot(char* filename, std::ofstream &log)
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
            MessageBox( NULL, "Unable to Allocate Bitmap Memory", "Error", MB_OK|MB_ICONERROR);
            break;
        } 
        bmpInfo.bmiHeader.biCompression=BI_RGB;
        GetDIBits(hdc,h,0,bmpInfo.bmiHeader.biHeight,pBuf, &bmpInfo, DIB_RGB_COLORS);
        if((fp = fopen(filename,"wb"))==NULL)
        {            
            log << "ERROR: Unable to Create Bitmap File.\n";
            MessageBox( NULL, "Unable to Create Bitmap File", "Error", MB_OK|MB_ICONERROR);
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
		
int main()
{
    std::ofstream log;
    log.open ("log.txt");
    char savePath[] = "Screenshot.bmp";
    TakeScreenShot(savePath, log);
    log.close();
    return 0;
}
