#include "ocr.h"


OCR::~OCR()
{
    if(NULL != this->horizontalDerivatives)
    {
        delete[] horizontalDerivatives;
        this->horizontalDerivatives = NULL;
    }
    if(NULL != this->verticalDerivatives)
    {
        delete[] verticalDerivatives;
        this->verticalDerivatives = NULL;
    }
}

void OCR::cornerDetection()
{
 // Harris corner detection
 
 // optional: Gaussian blur the image
    //this->blur();

 // grayscale the image ( binarize = true?)
    this->grayscale();
    
    
    
 // compute horizontal derivatives image
    char horizontalImagePath[] = "whateverHorizontal.bmp";
    this->computeHorizontalDerivatives();
	//this->blur(horizontalDerivatives);
    this->saveHorizontalBMP(horizontalImagePath);
    
// compute vertical derivatives image
    char verticalImagePath[] = "whateverVertical.bmp";
    this->computeVerticalDerivatives();
	//this->blur(verticalDerivatives);
    this->saveVerticalBMP(verticalImagePath);
    
    double maxScore = 0.0f;
    int kernelSize = 3;
    int movePositions = kernelSize / 2;

        int c = 0;
// define gaussian kernel and the structure tensor matrix    
    for(int i = 3; i < this->size; i += 4)
    {      
    
        //Ignore border pixels
        if(!( i < this->width * 4 * movePositions) && !(i > this->size - (this->width * 4 * movePositions)) 
        &&  !(i % (4*this->width) < 3 + (4 * (movePositions-1) ) ) && !( i % (4*this->width) > ((4*this->width) - (4* (movePositions-1)) - 1 ) ))
        {
        
            double tensorMatrix[2][2];
            int kernel[kernelSize][kernelSize];
            
            // All channels have same intensities for the current pixel
            
            // Current
            int current = i - 1;
            kernel[movePositions][movePositions] = current;
            
            // Top 
            int topCounter = 1;
            while(topCounter <= movePositions)
            {
            
                int top = i + (topCounter * this->width * 4) - 1;
                kernel[movePositions][movePositions+topCounter] = top;
                
                int leftCounter = 1;
                while(leftCounter <= movePositions) 
                {
                    int left = top - leftCounter * 4;
                    kernel[movePositions - leftCounter][movePositions+topCounter] = left;
                    leftCounter++;
                }
                
                int rightCounter = 1;
                while(rightCounter <= movePositions) 
                {
                    int right = top + rightCounter * 4;    
                    kernel[movePositions + rightCounter][movePositions+topCounter] = right;
                    rightCounter++;
                }
                
                topCounter++;
            }
            
            // Bottom
            int bottomCounter = 1;
            while(bottomCounter <= movePositions)
            {
            
                int bottom = i - (bottomCounter * this->width * 4) - 1;
                kernel[movePositions][movePositions-bottomCounter] = bottom;
                
                int leftCounter = 1;
                while(leftCounter <= movePositions) 
                {
                    int left = bottom - leftCounter * 4;
                    kernel[movePositions - leftCounter][movePositions-bottomCounter] = left;
                    leftCounter++;
                }
                
                int rightCounter = 1;
                while(rightCounter <= movePositions) 
                {
                    int right = bottom + rightCounter * 4;    
                    kernel[movePositions + rightCounter][movePositions-bottomCounter] = right;
                    rightCounter++;
                }
                
                bottomCounter++;
            }
            
            //Left
            int leftCounter = 1;
            while(leftCounter <= movePositions) 
            {
                int left = current - leftCounter * 4;
                kernel[movePositions - leftCounter][movePositions] = left;
                leftCounter++;
            }
            
            //Right
            int rightCounter = 1;
            while(rightCounter <= movePositions) 
            {
                int right = current + rightCounter * 4;
                kernel[movePositions + rightCounter][movePositions] = right;
                rightCounter++;
            }
            
            
            double xDiff;
            double yDiff;
            double xyDiff;
            
            //tensorMatrix[0][0] Ix * Iy
            //tensorMatrix[0][1] Iy * Iy
            //tensorMatrix[1][0] Ix * Ix
            //tensorMatrix[1][1] Iy * Iy
            
            for(int kX = 0; kX < kernelSize; ++kX)
            {
                for(int kY = 0; kY < kernelSize; ++kY)
                {
                    //TODO: Gaussian ?
                    xDiff += (double)this->horizontalDerivatives[ kernel[kX][kY] ] * (double)this->horizontalDerivatives[ kernel[kX][kY] ];
                    yDiff += (double)this->verticalDerivatives[ kernel[kX][kY] ] * (double)this->verticalDerivatives[ kernel[kX][kY] ];
                    xyDiff += (double)this->verticalDerivatives[ kernel[kX][kY] ] * (double)this->horizontalDerivatives[ kernel[kX][kY] ];
                }
            }
            
          /* double xDiff = (double)this->horizontalDerivatives[current] * (double)this->horizontalDerivatives[current] * 0.25f 
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
          */ 
// Compute the score det(H) - k*(Trace(H))^2 and a certain threshold
            tensorMatrix[0][0] = xyDiff;
            tensorMatrix[0][1] = yDiff;
            tensorMatrix[1][0] = xDiff;
            tensorMatrix[1][1] = xyDiff;
            
            double trace = tensorMatrix[1][0] + tensorMatrix[0][1];
            double det = (tensorMatrix[1][0] * tensorMatrix[0][1]) - (tensorMatrix[0][0] * tensorMatrix[1][1]);
             
            double k = 0.04f;
            double score = det - (k*trace*trace);

            if(score > 10000000000000000000000.0f )
            {                   
                this->corners.push_back(std::make_pair(i, score));
            }else if(score < -1000000.0f) 
            {
                //this->edges.push_back(std::make_pair(i, score));
            }
                
                
            if(score > maxScore)
                maxScore = score;      
                      
        
        }
    }
    this->whitenImage();
    
    int cornerThreshold = 10000;
    int edgeThreshold = 1000;
    
   // this->filterCorners(cornerThreshold);
   
   // this->dilate();
     this->displayCorners();
    //this->filterEdges(edgeThreshold);
   // this->displayEdges();
    
    Log::getInstance().debug(maxScore);
}

void OCR::dilate()
{
    for(int i = 3; i < this->size; i += 4)
    {
         //Ignore border pixels
        if(!( i < this->width * 4) && !(i > this->size - (this->width * 4)) 
        &&  !(i % (4*this->width) == 3) && !((i+1) % (4*this->width) == 0) )
        {
            int kernel[9];
            
            kernel[0] = i - 1;
            kernel[1] = i - 5;
            kernel[2] = i + 5;
            kernel[3] = i + (this->width * 4) - 1;
            kernel[4] = kernel[3] - 4;
            kernel[5] = kernel[3] + 4;
            kernel[6] = i - (this->width * 4) - 1;
            kernel[7] = kernel[6] - 4;
            kernel[8] = kernel[6] + 4;
            
            // Get max value
            int maxR = 0;
            int maxB = 0;
            int maxG = 0;
            
            for(int j = 0; j < 9; ++j)
            {
                if(this->pixels[kernel[j]] > maxB)
                    maxB = this->pixels[kernel[j]];
                    
                if(this->pixels[kernel[j]] > maxG)
                    maxG = this->pixels[kernel[j] - 1];
                    
                if(this->pixels[kernel[j]] > maxR)
                    maxR = this->pixels[kernel[j] - 2];
            }
            
            //Set all pixels to max value
            for(int j = 0; j < 9; ++j)
            {
                this->pixels[kernel[j]] = maxB;
                this->pixels[kernel[j]-1] = maxG;
                this->pixels[kernel[j]-2] = maxR;
            }
            
        }
    }
}

void OCR::whitenImage()
{
    //Set all pixels to white
    for(int i = 3; i < this->size; i += 4)
    {
        this->pixels[i-3] = (unsigned char) 255.0f;
        this->pixels[i-2] = (unsigned char) 255.0f;
        this->pixels[i-1] = (unsigned char) 255.0f;
    }
}

void OCR::displayCorners()
{        
    for(int j = 0; j < this->corners.size(); ++j)
    {
        //R
        this->pixels[this->corners[j].first - 1] = (unsigned char) 255.0f;
        //G
        this->pixels[this->corners[j].first - 2] = (unsigned char) 0.0f;
        //B
        this->pixels[this->corners[j].first - 3] = (unsigned char) 0.0f;
    }
}

void OCR::displayEdges()
{        
    for(int j = 0; j < this->edges.size(); ++j)
    {
        //R
        this->pixels[this->edges[j].first - 1] = (unsigned char) 0.0f;
        //G
        this->pixels[this->edges[j].first - 2] = (unsigned char) 0.0f;
        //B
        this->pixels[this->edges[j].first - 3] = (unsigned char) 0.0f;
    }
}

void OCR::filterCorners(int N)
{  
    //Local maxima
  /* for(int i = 9; i > 1; i--)
   {
        if(this->width % i == 0 && this->height % i == 0)
        {
            this->filterLocalMaxima(this->corners, i);
            break;
        }
   }*/
   
   //Just to be safe....
   if(N > this->corners.size())
        N = this->corners.size();
   
   //Global maxima
   this->sortCorners();    
   this->corners.erase(this->corners.begin(), this->corners.end() - N);
    
}

void OCR::filterLocalMaxima(std::vector<std::pair<int, double> > &source, int kernelSize)
{
	   // number of bins = (this->width / kernel_size) * (this->height / kernel_size);
    int numberOfBins = (this->width / kernelSize) * (this->height / kernelSize);
    // bins/row = this->width / kernelSize
    int binsPerRow = this->width / kernelSize;
			
    //Default values for bins 
    std::pair<int, double>* bins = new std::pair<int, double>[numberOfBins];
	
    for(int b = 0; b < numberOfBins; b++)
    {
        //Each bin has a pair of index and localMaxima 
        bins[b] = std::make_pair(-1, 0.0f);
    }
    
    for(int j = 0; j < source.size(); ++j)
    {

        int index = source[j].first;
        if(index < 0)
            continue;
            
        double score = source[j].second;
        
        int x = index % (this->width);
        int y = index / (this->width * 4);
        
        //( int(y / kernel size) * binsPerRow) + int(x / kernel size) = bin index
        int binIndex = ( (y/kernelSize) * binsPerRow ) + ( x/kernelSize );
        // If score of current corner is bigger than the current local maxima of the bin
        if(score > bins[binIndex].second)
        {
            // set new local maxima for this bin
            bins[binIndex].second = score;
            // set this corner to be the current max corner for this bin
            bins[binIndex].first = index;
        }        
    }
    
    std::vector<std::pair<int, double> > filtered;
    for(int b = 0; b < numberOfBins; b++)
    {
        if(bins[b].first > -1)
            filtered.push_back(std::make_pair(bins[b].first, bins[b].second) );
    }

	delete[] bins;
    source = filtered;  
}


void OCR::sortCorners()
{     
    this->sort(this->corners); 
}

void OCR::filterEdges(int N)
{  

 //Local maxima
   for(int i = 9; i > 1; i--)
   {
        if(this->width % i == 0 && this->height % i == 0)
        {
            this->filterLocalMaxima(this->edges, i);
            break;
        }
   }

  //Just to be safe....
   if(N > this->edges.size())
        N = this->edges.size();

    this->sortEdges();    
    this->edges.erase(this->edges.begin(), this->edges.end() - N);
    
}

void OCR::sortEdges()
{     
    this->sort(this->edges); 
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
    
    for(int i = 0; i < source.size(); i++)
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
        delete[] horizontalDerivatives;
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


void OCR::chessBoardDetection()
{ 
    this->cornerDetection();

    //Implement Hough transform
    //this->houghTransform();
    
    //Extract chessboard 

}

void OCR::houghTransform()
{
    // Get only horizontal and vertical thetas
    int thetaSpace[2];
    thetaSpace[0] = 0;
    thetaSpace[0] = 90;
    
    int rSpace = sqrt(this->width*this->width + this->height*this->height);

    // Accumulator array: consists of vector of thetas, each theta consists of vector of r's
    // The value of theta and r is a pair of (int) vector of indices and (int) accumulatorValue
    std::vector< std::vector<std::pair<std::vector<int>, int> > > accumulator;
    
    for(int theta = 0; theta < 2; theta++)
    {
        std::vector<std::pair<std::vector<int>, int> > rVec;
        for(int r = 0; r < rSpace; r++)
        {
            std::vector<int> involvedIndices;
            rVec.push_back(std::make_pair(involvedIndices, 0));
        }
        accumulator.push_back(rVec);
    }
    // Loop through all corners
    for(int i = 0; i < this->corners.size(); i++)
    {    
        int index = this->corners[i].first;
        int x = index % (this->width);
        int y = index / (this->width * 4);
        
        // Transform to polar coordinates
        // For every value of theta, calculate r for this x and y and increment the accumulator
        for(int theta = 0; theta < 2; theta++)
        {
            int r =  abs((int) ( (double)x * cos((double) thetaSpace[theta] * (double) PI/180) + (double)y*sin((double) thetaSpace[theta] * (double) PI/180) ));
           
            accumulator[theta][r].second += 1;
            accumulator[theta][r].first.push_back(index);
        }
        
    }
    
    //local maxima + filter for chessboard extraction
   // int kernelSize = 9;
 
    //for(int r = kernelSize; r < rSpace; r+=kernelSize)
    //{
       // int max = 0;
      //  int maxR = r;
        
       // for(int k = kernelSize; k > 0; k--)
       // {
       //     if(accumulator[0][r-k].second > max)
       //     {
       //         max = accumulator[0][r-k].second;
       //         maxR = r-k;
        //    }
        //}
        
        for(int r = 0; r < rSpace; r++)
        {
              
            //if(accumulator[theta][maxR].second > 7 && accumulator[theta][maxR].second < 50)
           // {
                for(int j = 0; j < accumulator[0][r].first.size(); j++)
                {
                    for(int k = 0; k < accumulator[1][r].first.size(); k++)
                    {   
                        if(accumulator[0][r].first[j] == accumulator[1][r].first[k])
                        {
                            //B
                            this->pixels[accumulator[0][r].first[j] - 3] = 0.0f;
                            //G
                            this->pixels[accumulator[0][r].first[j] - 2] = 255.0f;
                            //R
                            this->pixels[accumulator[0][r].first[j] - 1] = 0.0f;
                        }
                    }
                }
                
           // }
            
        }
   // }
    
    
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
        delete[] verticalDerivatives;
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


