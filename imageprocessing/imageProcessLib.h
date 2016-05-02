#ifndef OPENCVTEST_IMAGEPROCESSLIB_H
#define OPENCVTEST_IMAGEPROCESSLIB_H

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>


class imageProcess
{
    //-----------//
    // Variables //
    //-----------//
private:
    bool usesMakeBinary;
    bool usesContrast;
    bool usesThreshold;
    bool usesfixLight;
    bool usesRoiMaker;
    bool usesSkelMaker;
    bool usesFilterWhiteAreas;
    bool usesHoughLinesPLR;

    // Original Frame width and height
    short origWidth;
    short origHeight;

    // Contrast() variables
    double alpha;
    double beta;

    //makeBinary()
    uchar threshold;

    // Threshold() variables
    uchar light;

    // fixLight() variables
    uchar range;

    // ROIMaker() variables
    short verticalPos;
    short horizontalPos;
    short skewed;

    // filterWhiteAreas() variables
    double whiteAreaMaxLimit;
    double whiteAreaMin;
    double whiteLengthLimit;

    //HoughLinesPLR() variables
    double houghThreshold;
    double minLineLength;
    double maxLineGap;

    cv::Mat* frame;

    //---------//
    // Methods //
    //---------//
public:

    imageProcess();

    void setSize(int width, int height);
    void resetSize();
    void setDefaultValues();
    void processImage(cv::Mat &image);
    void processImage(IplImage &image);

    void setContrast(double alpha, double beta, bool isActive);
    void setThreshold(uchar thes, bool isActive);
    void setCustomThreshold(uchar light, bool isActive);
    void setLightRange(uchar range, bool isActive);
    void setRoi(short verticalPos, short horizontalPos, short skewed, bool isActive);
    void setWhiteFilter(double areaMaxLimit, double areaMin, double areaLength, bool isActive);
    void setHoughLines(uchar threshold, double maxLineGap, double minLineLength, bool isActive);


private:

    void theInterestingPart();
    // Makes Mat single channel grayscale, adds blur, thresholds image
    // thres = double value for the threshold (0-255)
    void makeBinary();

    // Contrast the image
    void Contrast();

    // Custom threshold
    void Theshold();

    // Fix light
    void fixLight();

    // Create a ROI
    // verticalPos = how far from the left and rigth edges you want the roi int
    // horizontalPos = how fram from top and bottom you want the roi int
    // skewed = how many pixels you want the top corners to be skewed, ie closer together than the bottom ones int (0=default for rectangle)
    void ROIMaker();

    // Create a left and right ROI,
    // outputLeft = cv::Mat you want the left part of the image in,
    // outputRight = cv::Mat you want the right side of the image in
    // Used for HoughLinesPRL
    void ROISplitMaker(cv::Mat &outputLeft, cv::Mat &outputRight);

    // Make greyscale image into skeleton-image,
    // Makes a skeleton image of a greyscale 8-bit one channel source image,
    void skelMaker();

    // Algorithm that takes a single channel cv::Mat and detect polygon areas of a certain area and arclength and paints detected areas green on input and black on output
    // whiteAreaMaxLimit = double to set the threshold for how big areas should be detected and "erased"
    // whiteAreaMin = double to set the minimum area threshold for areas to be detected
    // whiteLengthLimit = double to set the archLength threshold for length of areas to be detected, lareas longer than this value will be ignored
    void filterWhiteAreas();

    // Detect HoughlinesP on left side and right side of an image,it uses the ROI split function to do this
    // houghThreshold = double to set the threshold for houghtransform probabilistic
    // minLineLength = double to set the shortest line lengths to be used
    // maxLineGap = double to set the maximum line gap to be used before considering a line segments to be separate
    void HoughlinesPLR();
};



#endif //OPENCVTEST_IMAGEPROCESSLIB_H