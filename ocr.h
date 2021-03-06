#ifndef OCR_H
#define OCR_H

#include "image.h"
#include <vector>
#include <math.h>
#include <unordered_map>

#define PI 3.14159265
#define euler 2.71828182846

class OCR: public BmpImage
{
private:
    unsigned char *horizontalDerivatives;
    unsigned char *verticalDerivatives;
    
    // Vector with pairs: <index, score> of a corner
    std::vector<std::pair<int, double> > corners;
    // Vector with pairs: <index, score> of an edge
    std::vector<std::pair<int, double> > edges;
    double *pixelScores;
    int *pixelEdges;
    int *pixelCorners;
    
    void whitenImage();
    void filterCorners(int N);
    void sortCorners();
    void displayCorners();
    
    void filterEdges(int N);
	void filterLocalMaxima(std::vector<std::pair<int, double> > &source, int kernelSize);
	void filterLocalMaxima2(std::vector<std::pair<int, double> > &source, int kernelSize);
    
    void sortEdges();
    void displayEdges();

    void sort(std::vector<std::pair<int, double> > &v1);
    void merge(std::vector<std::pair<int, double> > &source, std::vector<std::pair<int, double> > &v1, std::vector<std::pair<int, double> > &v2);
    void erosion(unsigned char *source, int kernelSize, std::vector<int**> erosionKernels);
	
	double calculateGaussianKernel(int x, int y, double sigma);
    
    //If it is a square corner, return possible directions
	std::pair<int, int> isASquareCorner(int i);
    
    // 0 if the corner or its neighbourhood corners are already used for square detection 
    int isUsedSquareCorner(int i, std::unordered_map<int, int>& usedCorners, int neighbourhoodSize);
    
    void houghTransform();
public:
    OCR():BmpImage()
    {
        horizontalDerivatives = NULL;
        verticalDerivatives = NULL;
        pixelScores = NULL;
        pixelEdges = NULL;
        pixelCorners = NULL;
    };
    ~OCR();
    void cornerDetection();
    void chessBoardDetection();
    
    void computeHorizontalDerivatives();
    void saveHorizontalBMP(char* filename);
    void computeVerticalDerivatives();
    void saveVerticalBMP(char* filename);
};

#endif