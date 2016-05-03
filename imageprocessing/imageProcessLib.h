//------------------------------ USAGE ------------------------------//
/*
First you need to create a ImageProcess object: imageProcess Obj;
Then you need to set the values for each function you want to use with the setter-methods,
for example setContrast(value,value, bool isActive), where you set the parameters and can set the bool of the function through the call

Only the functions with it's corresponding bool=true will be active, the constructor for the object initializes all bool's with false
Obj.setDefaultValues(); can be used to set some values for all functions we consider "OK" as default,
Values for all functions will need to be set before using them

Obj.processImage(Mat); or Obj.processImage(IplImage); will run the selected functions on the image with the selected values.
*/
//------------------------------ END OF USAGE ------------------------------//


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
	bool usesMakeBinary; // true = makeBinary() will run
	bool usesContrast;   // true = Contrast() will run
	bool usesThreshold;  // true = Threshold() will run
	bool usesfixLight;   // true = fixLight() will run
	bool usesRoiMaker;   // true = ROIMaker() will run
	bool usesSkelMaker;  // true = skelMaker will run
	bool usesFilterWhiteAreas; // true = whiteAreaFilter() will run
	bool usesHoughLinesPLR; //// true = HoughLinesPLR() will run

							// Original Frame width and height, used when image is resized and then reset to original size
	short origWidth;
	short origHeight;

	// Contrast() variables
	double alpha;
	double beta;

	//makeBinary()
	uchar threshold; // thres = value for the threshold1 (0-255), threshold2 already set to 255

					 // Threshold() variables
	uchar light;

	// fixLight() variables
	uchar range;

	// ROIMaker() variables
	short verticalPos; //verticalPos = how far from the left and right edges you want the roi int
	short horizontalPos; //horizontalPos = how fram from top and bottom you want the roi int
	short skewed; //skewed = how many pixels you want the top corners to be skewed,ie closer together than the bottom ones int (0=default for rectangle)

				  // filterWhiteAreas() variables
	double whiteAreaMaxLimit; // whiteAreaMaxLimit = double to set the threshold for how big areas should be detected and "erased"
	double whiteAreaMin;  // whiteAreaMin = double to set the minimum area threshold for areas to be detected
	double whiteLengthLimit; // whiteLengthLimit = double to set the archLength threshold for length of areas to be detected, areas longer than this value will be ignored

							 //  HoughLinesPLR() variables
	double houghThreshold; // houghThreshold = double to set the threshold for houghtransform probabilistic
	double minLineLength; // minLineLength = double to set the shortest line lengths to be used
	double maxLineGap; // maxLineGap = double to set the maximum line gap to be used before considering a line segments to be separate

					   //  The pointer to the image which will be processed
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

	void activateContrast(bool active);
	void activateMakeBinary(bool active);
	void activateCustomThreshold(bool active);
	void activateFixLight(bool active);
	void activateROI(bool active);
	void activateWhiteFilter(bool active);
	void activateHoughLinesPLR(bool active);

private:
	// the functions that runs all image process functions that are activated by their bool
	void theInterestingPart();

	// Makes Mat single channel grayscale, adds blur, thresholds image
	void makeBinary();

	// Contrast the image
	void Contrast();

	// Custom threshold
	void Theshold();

	// Fix light
	void fixLight();

	// Create a ROI of image
	void ROIMaker();

	/* Create a left and right ROI,
	outputLeft = cv::Mat you want the left part of the image in,
	outputRight = cv::Mat you want the right side of the image in
	Used for HoughLinesPRL */
	void ROISplitMaker(cv::Mat &outputLeft, cv::Mat &outputRight);

	/* Make greyscale image into skeleton-image,
	Makes a skeleton image of a greyscale 8-bit one channel source image */
	void skelMaker();

	// Algorithm that takes a single channel cv::Mat and detect polygon areas of a certain area and arclength and paints detected areas black
	void filterWhiteAreas();

	// Detect HoughlinesP on left side and right side of an image,it uses the ROI split function to do this
	void HoughlinesPLR();
};



#endif //OPENCVTEST_IMAGEPROCESSLIB_H
