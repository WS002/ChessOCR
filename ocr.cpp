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
    this->grayscale(1);
    
    
    
 // compute horizontal derivatives image
    char horizontalImagePath[] = "whateverHorizontal.bmp";
    this->computeHorizontalDerivatives();
	this->blur(horizontalDerivatives);
    this->saveHorizontalBMP(horizontalImagePath);
    
// compute vertical derivatives image
    char verticalImagePath[] = "whateverVertical.bmp";
    this->computeVerticalDerivatives();
	this->blur(verticalDerivatives);
    this->saveVerticalBMP(verticalImagePath);
    
    double maxScore = 0.0f;
    int threshold = 1000;
    
// define gaussian kernel and the structure tensor matrix    
    for(int i = 3; i < this->size; i += 4)
    {        
        //Ignore border pixels
        if(!( i < this->width * 4) && !(i > this->size - (this->width * 4)) 
        &&  !(i % (4*this->width) == 3) && !((i+1) % (4*this->width) == 0) )
        {
            double tensorMatrix[2][2];
            // All channels have same intensities for the current pixel
            int current = i - 1;
            int left = i - 5;
            int right = i + 5;
            int top = i + (this->width * 4) - 1;
            int topLeft = top - 4;
            int topRight = top + 4;
            int bottom = i - (this->width * 4) - 1;
            int bottomLeft = bottom - 4;
            int bottomRight = bottom + 4;
            
            //tensorMatrix[0][0] Ix * Iy
            //tensorMatrix[0][1] Iy * Iy
            //tensorMatrix[1][0] Ix * Ix
            //tensorMatrix[1][1] Iy * Iy
            
           double xDiff = (double)this->horizontalDerivatives[current] * (double)this->horizontalDerivatives[current] * 0.25f 
                        + (double)this->horizontalDerivatives[left] *(double)this->horizontalDerivatives[left] * 0.125f
                        + (double)this->horizontalDerivatives[right] * (double)this->horizontalDerivatives[right] * 0.125f
                        + (double)this->horizontalDerivatives[top] * (double)this->horizontalDerivatives[top] * 0.125f
                        + (double)this->horizontalDerivatives[bottom] * (double)this->horizontalDerivatives[bottom] * 0.125f
                        + (double)this->horizontalDerivatives[topLeft] * (double)this->horizontalDerivatives[topLeft] * 0.0625f
                        + (double)this->horizontalDerivatives[topRight] * (double)this->horizontalDerivatives[topRight] * 0.0625f
                        + (double)this->horizontalDerivatives[bottomLeft] * (double)this->horizontalDerivatives[bottomLeft] * 0.0625f
                        + (double)this->horizontalDerivatives[bottomRight] * (double)this->horizontalDerivatives[bottomRight] * 0.0625f;
                        
           double yDiff = (double)this->verticalDerivatives[current] * (double)this->verticalDerivatives[current] * 0.25f 
                        + (double)this->verticalDerivatives[left] *(double)this->verticalDerivatives[left] * 0.125f
                        + (double)this->verticalDerivatives[right] * (double)this->verticalDerivatives[right] * 0.125f
                        + (double)this->verticalDerivatives[top] * (double)this->verticalDerivatives[top] * 0.125f
                        + (double)this->verticalDerivatives[bottom] * (double)this->verticalDerivatives[bottom] * 0.125f
                        + (double)this->verticalDerivatives[topLeft] * (double)this->verticalDerivatives[topLeft] * 0.0625f
                        + (double)this->verticalDerivatives[topRight] * (double)this->verticalDerivatives[topRight] * 0.0625f
                        + (double)this->verticalDerivatives[bottomLeft] * (double)this->verticalDerivatives[bottomLeft] * 0.0625f
                        + (double)this->verticalDerivatives[bottomRight] * (double)this->verticalDerivatives[bottomRight] * 0.0625f;
                        
          double xyDiff = (double)this->verticalDerivatives[current] * (double)this->horizontalDerivatives[current] * 0.25f 
                        + (double)this->verticalDerivatives[left] *(double)this->horizontalDerivatives[left] * 0.125f
                        + (double)this->verticalDerivatives[right] * (double)this->horizontalDerivatives[right] * 0.125f
                        + (double)this->verticalDerivatives[top] * (double)this->horizontalDerivatives[top] * 0.125f
                        + (double)this->verticalDerivatives[bottom] * (double)this->horizontalDerivatives[bottom] * 0.125f
                        + (double)this->verticalDerivatives[topLeft] * (double)this->horizontalDerivatives[topLeft] * 0.0625f
                        + (double)this->verticalDerivatives[topRight] * (double)this->horizontalDerivatives[topRight] * 0.0625f
                        + (double)this->verticalDerivatives[bottomLeft] * (double)this->horizontalDerivatives[bottomLeft] * 0.0625f
                        + (double)this->verticalDerivatives[bottomRight] * (double)this->horizontalDerivatives[bottomRight] * 0.0625f;
           
// Compute the score det(H) - k*(Trace(H))^2 and a certain threshold
            tensorMatrix[0][0] = xyDiff;
            tensorMatrix[0][1] = yDiff;
            tensorMatrix[1][0] = xDiff;
            tensorMatrix[1][1] = xyDiff;
            
            double trace = tensorMatrix[1][0] + tensorMatrix[0][1];
            double det = (tensorMatrix[1][0] * tensorMatrix[0][1]) - (tensorMatrix[0][0] * tensorMatrix[1][1]);
            
            double k = 0.04f;
            double score = det - (k*trace*trace);
            if(score > 0.0f )
            {               
                this->corners.push_back(std::make_pair(i, score));
            }
                
                
            if(score > maxScore)
                maxScore = score;      
                      
        }
    }
    
    this->filterCorners(threshold);
    this->displayCorners();
    Log::getInstance().debug(maxScore);
}

void OCR::displayCorners()
{
    //Set all pixels to white
    for(int i = 3; i < this->size; i += 4)
    {
        this->pixels[i-3] = (unsigned char) 255.0f;
        this->pixels[i-2] = (unsigned char) 255.0f;
        this->pixels[i-1] = (unsigned char) 255.0f;
    }
    
    for(int j = 0; j < this->corners.size(); ++j)
    {
        //B
        this->pixels[this->corners[j].first - 1] = 0.0f;
        //G
        this->pixels[this->corners[j].first - 2] = 0.0f;
        //R
        this->pixels[this->corners[j].first - 3] = 0.0f;
    }
}

void OCR::filterCorners(int N)
{  

    this->sortCorners();    
    this->corners.erase(this->corners.begin(), this->corners.end() - N);
    
}

void OCR::sortCorners()
{     
    this->sort(this->corners); 
}

// Merge sort
void OCR::sort(std::vector<std::pair<int, double> > &source)
{
    if(source.size() < 2)
        return;
    
    std::size_t const half_size = source.size() / 2;
    std::vector<std::pair<int, double> > split_lo(source.begin(), source.begin() + half_size);
    std::vector<std::pair<int, double> > split_hi(source.begin() + half_size, source.end());  
   
    //Sort left
    this->sort(split_lo);
    //Sort right
    this->sort(split_hi);
    //Merge left and right
    this->merge(source, split_lo, split_hi);
}

void OCR::merge(std::vector<std::pair<int, double> > &source, std::vector<std::pair<int, double> > &v1, std::vector<std::pair<int, double> > &v2)
{
    // indices of v1 and v2
    unsigned int x = 0;
    unsigned int y = 0;
    
    for(int i = 0; i < source.size(); ++i)
    {
        if( v1.size() == x)
        {
            source[i] = v2[y];
            y++;
            continue;
        }
        
        if( v2.size() == y)
        {
            source[i] = v1[x];
            x++;
            continue;
        }
                    
        if(v1[x].second < v2[y].second)
        {
            source[i] = v1[x];
            x++;
        }else{
            source[i] = v2[y];
            y++;
        }
    }
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


