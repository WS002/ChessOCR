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
    
    if(NULL != this->pixelScores)
    {
        delete[] pixelScores;
        this->pixelScores = NULL;
    }
    
    if(NULL != this->pixelEdges)
    {
        delete[] pixelEdges;
        this->pixelEdges = NULL;
    }
}

double OCR::calculateGaussianKernel(int x, int y, double sigma)
{
	return (1/2*sigma*sigma*PI)*euler-((x*x + y*y)/2*sigma*sigma);
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
    int kernelSize = 1;
    int movePositions = kernelSize / 2;
    
    this->pixelScores = new double[this->size];
    // If 0, no edge; if 1, edge
    this->pixelEdges = new int[this->size];
    // populate with 0's
    for(int pE = 0; pE < this->size; pE++)
        this->pixelEdges[pE] = 0;

	double sigma = 0.2f;
	
// define gaussian kernel and the structure tensor matrix    
    for(int i = 3; i < this->size; i += 4)
    {   
  
        //Ignore border pixels
      if( !( i < this->width * 4 * movePositions) && !(i > this->size - (this->width * 4 * movePositions)) 
        &&  !(i % (4*this->width) < 4 + (4 * (movePositions-1) ) ) && !( i % (4*this->width) >= (4*this->width - 1) - 4* (movePositions-1))  )
        {
        
            double tensorMatrix[2][2];
            int kernel[kernelSize][kernelSize];
			double gaussian[kernelSize][kernelSize];
                        
            // Current
            int current = i - 1;
                                                       
            kernel[movePositions][movePositions] = current;
            gaussian[movePositions][movePositions] = this->calculateGaussianKernel(0,0,sigma);
            // Top 
            int topCounter = 1;
            while(topCounter <= movePositions)
            {
            
                int top = current + (topCounter * this->width * 4);     
                
                kernel[movePositions][movePositions+topCounter] = top;
                gaussian[movePositions][movePositions+topCounter] = this->calculateGaussianKernel(0,topCounter,sigma);
				
                int leftCounter = 1;
                while(leftCounter <= movePositions) 
                {
                    int left = top - leftCounter * 4;
                                    
                    kernel[movePositions - leftCounter][movePositions+topCounter] = left;
					gaussian[movePositions - leftCounter][movePositions+topCounter] =  this->calculateGaussianKernel(leftCounter,topCounter,sigma);
                    leftCounter++;
                }
                
                int rightCounter = 1;
                while(rightCounter <= movePositions) 
                {
                    int right = top + rightCounter * 4;    
                                    
                    kernel[movePositions + rightCounter][movePositions+topCounter] = right;
					gaussian[movePositions + rightCounter][movePositions+topCounter] = this->calculateGaussianKernel(rightCounter,topCounter,sigma);
                    rightCounter++;
                }
                
                topCounter++;
            }
            
            // Bottom
            int bottomCounter = 1;
            while(bottomCounter <= movePositions)
            {
            
                int bottom = current - (bottomCounter * this->width * 4);
                kernel[movePositions][movePositions-bottomCounter] = bottom;                         
                gaussian[movePositions][movePositions-bottomCounter] = this->calculateGaussianKernel(0,bottomCounter,sigma);
				
                int leftCounter = 1;
                while(leftCounter <= movePositions) 
                {
                    int left = bottom - leftCounter * 4;  
             
                    kernel[movePositions - leftCounter][movePositions-bottomCounter] = left;
					gaussian[movePositions - leftCounter][movePositions-bottomCounter] = this->calculateGaussianKernel(leftCounter,bottomCounter,sigma);
					
                    leftCounter++;
                }
                
                int rightCounter = 1;
                while(rightCounter <= movePositions) 
                {
                    int right = bottom + rightCounter * 4;    

                    kernel[movePositions + rightCounter][movePositions-bottomCounter] = right;
					gaussian[movePositions + rightCounter][movePositions-bottomCounter] = this->calculateGaussianKernel(rightCounter,bottomCounter,sigma);
					
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
				gaussian[movePositions - leftCounter][movePositions] = this->calculateGaussianKernel(leftCounter,0,sigma);
                leftCounter++;
            }
            
            //Right
            int rightCounter = 1;
            while(rightCounter <= movePositions) 
            {
                int right = current + rightCounter * 4;
                 
                kernel[movePositions + rightCounter][movePositions] = right;
				gaussian[movePositions + rightCounter][movePositions] = this->calculateGaussianKernel(rightCounter,0,sigma);
                rightCounter++;
            }

    
            double xDiff = 0.0f;
            double yDiff = 0.0f;
            double xyDiff = 0.0f;
            
            //tensorMatrix[0][0] Ix * Iy
            //tensorMatrix[0][1] Iy * Iy
            //tensorMatrix[1][0] Ix * Ix
            //tensorMatrix[1][1] Iy * Iy
            
            for(int kX = 0; kX < kernelSize; ++kX)
            {
                for(int kY = 0; kY < kernelSize; ++kY)
                {                     
                    xDiff += ((double)this->horizontalDerivatives[kernel[kX][kY]] * (double)this->horizontalDerivatives[kernel[kX][kY]]); //* gaussian[kX][kY] );
                    yDiff += ((double)this->verticalDerivatives[kernel[kX][kY]] * (double)this->verticalDerivatives[kernel[kX][kY]]); //* gaussian[kX][kY] );
                    xyDiff +=((double)this->verticalDerivatives[kernel[kX][kY]] * (double)this->horizontalDerivatives[kernel[kX][kY]]); //* gaussian[kX][kY] );
                }
            }

// Compute the score det(H) - k*(Trace(H))^2 and a certain threshold
            tensorMatrix[0][0] = xyDiff;
            tensorMatrix[0][1] = yDiff;
            tensorMatrix[1][0] = xDiff;
            tensorMatrix[1][1] = xyDiff;
            
            double trace = tensorMatrix[1][0] + tensorMatrix[0][1];
            double det = (tensorMatrix[1][0] * tensorMatrix[0][1]) - (tensorMatrix[0][0] * tensorMatrix[1][1]);
             
            double k = 0.04f;
            double score = det - (k*trace*trace);
           
            this->pixelScores[i] = score;
            this->pixelScores[i-1] = score;
            this->pixelScores[i-2] = score;
            this->pixelScores[i-3] = score;
           
            if(score > 0.0f )
            {                   
                this->corners.push_back(std::make_pair(i, score));
            }else if(score < 0.0f) 
            {
                this->edges.push_back(std::make_pair(i, score));
            }
                
                
            if(score > maxScore)
                maxScore = score;      
                      
        
        }
    }
    
    
    this->whitenImage();
    
    int cornerThreshold = this->corners.size()/5;
    int edgeThreshold = this->edges.size()/5;
    
    this->filterEdges(edgeThreshold);
    for(int edge = 0; edge < this->edges.size(); edge++)
    {
        this->pixelEdges[this->edges[edge].first] = 1;
    }
    
    this->displayEdges();
    
    //this->filterCorners(cornerThreshold);  
   // this->dilate();
    //this->displayCorners();
   
    
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
   this->filterLocalMaxima2(this->corners, 5);
 
   //Just to be safe....
  // if(N > this->corners.size())
    //    N = this->corners.size();
   
   //Global maxima
  // this->sortCorners();    
  // this->corners.erase(this->corners.begin(), this->corners.end() - N);

    
}

void OCR::filterEdges(int N)
{  


    this->filterLocalMaxima2(this->edges, 3);
     

  //Just to be safe....
  // if(N > this->edges.size())
  //      N = this->edges.size();

   // this->sortEdges();    
    //this->edges.erase(this->edges.begin(), this->edges.end() - N);
    
}

// Filter based on neighbourhood around pixel
void OCR::filterLocalMaxima2(std::vector<std::pair<int, double> > &source, int kernelSize)
{

    int movePositions = kernelSize / 2;
    std::vector<std::pair<int, double> > filtered;
    
    for(int i = 0; i < source.size(); i++)
    {
    
        //Ignore border pixels
        if( !( source[i].first < this->width * 4 * movePositions) && !(source[i].first > this->size - (this->width * 4 * movePositions)) 
        &&  !(source[i].first % (4*this->width) < 4 + (4 * (movePositions-1) ) ) && !( source[i].first % (4*this->width) >= (4*this->width - 1) - 4* (movePositions-1))  )
        {
            int kernel[kernelSize][kernelSize];
            double localMaxima = 0.0f;  
            std::vector<int> maxIndices;
            // Current
            int current = source[i].first;
                                                       
            kernel[movePositions][movePositions] = current;
            
            // Top 
            int topCounter = 1;
            while(topCounter <= movePositions)
            {
            
                int top = current + (topCounter * this->width * 4);     
                
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
            
                int bottom = current - (bottomCounter * this->width * 4);
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

            
            for(int kX = 0; kX < kernelSize; ++kX)
            {
                for(int kY = 0; kY < kernelSize; ++kY)
                {  
                    if(this->pixelScores[ kernel[kX][kY] ] >= localMaxima)
                    {
                        localMaxima = this->pixelScores[ kernel[kX][kY] ];
                        maxIndices.push_back(kernel[kX][kY]);
                    }
                }
                
            }
            
            for(int x = 0; x < maxIndices.size(); x++)
            {
                filtered.push_back(std::make_pair(maxIndices[x], this->pixelScores[maxIndices[x]]));
            }
            
        }
    }
    source = filtered;
}

// Filter based on bins

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
        if(binIndex >= numberOfBins)
            continue;
            
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
    // First int is the size of the square, the second is a pair with    the number of squares with this size found  and the edge indices of these squares
    std::unordered_map<int, std::pair<int, std::vector<int> > > squares;
    //Extract chessboard 
    for(int i = 3; i < this->size; i+=4)
    {
        if(pixelEdges[i] == 0)
            continue;		
    
        //Ignore border pixels
         if(!( i < this->width * 4) && !(i > this->size - (this->width * 4)) 
         &&  !(i % (4*this->width) == 3) && !((i+1) % (4*this->width) == 0) )
         {
            int bottomLeftCorner = 0;                            
            // Find bottom left corner
            if(pixelEdges[i] == 1 && pixelEdges[i-4] == 0 && pixelEdges[i - (this->width * 4)] == 0 && pixelEdges[i+4] == 1 && pixelEdges[i + (this->width * 4)] == 1)
            {
               
			   
                //bottom left corner found
                int squareSize = 1;
                int currentI = (i + (this->width * 4));
                bottomLeftCorner = currentI;
			
                while( currentI < this->size && pixelEdges[currentI] == 1) 
                {
                    int topLeftCorner = 0;
                    //top left corner found
                    if( (currentI +(this->width*4) ) < this->size && pixelEdges[currentI  +(this->width*4) ] == 0 && pixelEdges[currentI - 4 ] == 0 &&  pixelEdges[currentI + 4 ] == 1)
                    {
                        topLeftCorner = currentI;
                        int squareSize2 = 1;
						
					
                        while( currentI < this->size && pixelEdges[currentI] == 1) 
                        {
							if(squareSize2 > squareSize)
								break;
										this->pixels[currentI - 1] = 0.0f;
						this->pixels[currentI - 2] = 255.0f;
						this->pixels[currentI - 3] = 0.0f;
                            int topRightCorner = 0;
                            //top right corner found
                            if( (currentI + 4)  % this->width < (this->width - 1)  && pixelEdges[currentI  + 4 ] == 0 && pixelEdges[currentI +(this->width*4) ] == 0 &&  pixelEdges[currentI -(this->width*4) ] == 1)
                            {
														
			
                                topRightCorner = currentI;
                                // Break if not a square by now
                                if(squareSize != squareSize2)
                                    break;
                                    
                                    int squareSize3 = 1;
                                    while( currentI > 0 && pixelEdges[currentI] == 1) 
                                    {
										if(squareSize3 > squareSize)
											break;
                                        int bottomRightCorner = 0;
                                        // Bottom right corner found
                                         //top right corner found
                                        if( (currentI -(this->width*4) ) > 0  && pixelEdges[currentI  + 4 ] == 0 && pixelEdges[currentI - 4 ] == 1 &&  pixelEdges[currentI -(this->width*4) ] == 0)
                                        {
                                            bottomRightCorner = currentI;
                                            // Break if not a square by now
                                            if(squareSize != squareSize3)
                                                break;
                                                
                                            if(bottomRightCorner - squareSize*4 == bottomLeftCorner)
                                            {
                                                // Found a square with size squareSize
                                                std::vector<int> corners;
                                                corners.push_back(bottomLeftCorner);
                                                corners.push_back(topLeftCorner);
                                                corners.push_back(topRightCorner);
                                                corners.push_back(bottomRightCorner);
                                                
                                                if(squares[squareSize].first == NULL)
                                                    squares[squareSize] = make_pair(1, corners);
                                                else
                                                    squares[squareSize].first++;
                                                    
                                                Log::getInstance().debug("Square Found... count is..");
                                                Log::getInstance().debug(squares[squareSize].first);
                                            }
                                            
                                        }
                                        
                                        currentI -= (this->width * 4);
                                        squareSize3++;
                                    }
                                
                                
                            }
                            currentI += 4;
                            squareSize2++;
                        }
                    }
                   
                    currentI += (this->width * 4);
                    squareSize++;
                }
                
            }
        }
    }
    
     for ( auto it = squares.begin(); it != squares.end(); ++it )
     {
        if(it->second.first == 64)
        {
            for(int sqI = 0; sqI < it->second.second.size(); sqI++)
            {
                this->pixels[ it->second.second[sqI] -1 ] = 0.0f;
                this->pixels[ it->second.second[sqI] -2 ] = 255.0f;
                this->pixels[ it->second.second[sqI] -3 ] = 0.0f;
            }
        
        }
        
     }

}

void OCR::houghTransform()
{
    // Get only horizontal and vertical thetas
    int thetaSpace[2];
    thetaSpace[0] = 0;
    thetaSpace[1] = 90;
    
    int rSpace = sqrt(this->width*this->width + this->height*this->height);

    // Accumulator array: consists of vector of thetas, each theta consists of vector of r's
    // The value of theta and r is a pair of (int) vector of indices and (int) accumulatorValue
    std::vector< std::vector<std::pair<std::vector<int>, int> > > accumulator;
    
    // First element of the vector would be the r for theta = 0, second for theta = 1
    std::unordered_map<int, std::vector<int> > cornerToRAndTheta;
    
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
        
        std::vector<int> mapVec;
        cornerToRAndTheta[index] = mapVec;
        // Transform to polar coordinates
        // For every value of theta, calculate r for this x and y and increment the accumulator
        for(int theta = 0; theta < 2; theta++)
        {
            int r =  abs((int) ( (double)x * cos((double) thetaSpace[theta] * (double) PI/180) + (double)y*sin((double) thetaSpace[theta] * (double) PI/180) ));
           
            accumulator[theta][r].second += 1;
            accumulator[theta][r].first.push_back(index);
            // First, the r for theta = 0 will be pushed back, then for theta = 1
            cornerToRAndTheta[index].push_back(r);
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


