#ifndef OCR_H
#define OCR_H

#include "image.h"
#include <vector>
#include <math.h>

#define PI 3.14159265

class OCR: public BmpImage
{
private:
    unsigned char *horizontalDerivatives;
    unsigned char *verticalDerivatives;
    
    // Vector with pairs: <index, score> of a corner
    std::vector<std::pair<int, double> > corners;
    // Vector with pairs: <index, score> of an edge
    std::vector<std::pair<int, double> > edges;
    
    
    void whitenImage();
    void filterCorners(int N);
    void sortCorners();
    void displayCorners();
    
    void filterEdges(int N);
	void filterLocalMaxima(std::vector<std::pair<int, double> > &source, int kernelSize);
    void sortEdges();
    void displayEdges();

    void sort(std::vector<std::pair<int, double> > &v1);
    void merge(std::vector<std::pair<int, double> > &source, std::vector<std::pair<int, double> > &v1, std::vector<std::pair<int, double> > &v2);
    void dilate();
    
    void houghTransform();
public:
    OCR():BmpImage()
    {
        horizontalDerivatives = NULL;
        verticalDerivatives = NULL;
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