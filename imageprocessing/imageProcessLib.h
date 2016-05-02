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
	bool usesRoiSplitMaker;
	bool usesSkelMaker;
	bool usesFilterWhiteAreas;
	bool usesHoughLinesPLR;

	// Original Frame width and height
	int origWidth;
	int origHeight;
	
	// Contrast() variables
	double alpha;
	double beta;

	//makeBinary()
	double threshold;

	// Threshold() variables
	uchar light;

	// fixLight() variables
	uchar range;

	// ROIMaker() variables
	int verticalPos;
	int horizontalPos;
	int skewed;

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

	void imageProcess::setSize(int width, int height);
	void imageProcess::resetSize();
	void imageProcess::setDefaultValues();
	void processImage(cv::Mat &image);
	void processImage(IplImage &image);

	// Makes Mat single channel grayscale, adds blur, thresholds image
	// input = cv::Mat image you want to make greyscale 8-bit single channel image of (the input image will be changed!)
	// thres = double value for the threshold (0-255)
	void makeBinary(cv::Mat &input);

	// Contrast the image
	void Contrast(cv::Mat &input);

	// Custom threshold
	void Theshold(cv::Mat &input);

	// Fix light
	void fixLight(cv::Mat &input);

	// Create a ROI
	// input = cv::Mat you want to extract ROI from
	// verticalPos = how far from the left and rigth edges you want the roi int
	// horizontalPos = how fram from top and bottom you want the roi int
	// skewed = how many pixels you want the top corners to be skewed, ie closer together than the bottom ones int (0=default for rectangle)
	void ROIMaker(cv::Mat &input);

	// Create a left and right ROI,
	// input = cv::Mat you want to split,
	// outputLeft = cv::Mat you want the left part of the image in,
	// outputRight = cv::Mat you want the right side of the image in
	void ROISplitMaker(cv::Mat &input, cv::Mat &outputLeft, cv::Mat &outputRight);

	// Make greyscale image into skeleton-image,
	// Makes a skeleton image of a greyscale 8-bit one channel source image,
	// input = cv::Mat you want to skeletonize, output = the cv::Mat you want the skeletonized image in
	void skelMaker(cv::Mat &input);

	// Algorithm that takes a single channel cv::Mat and detect polygon areas of a certain area and arclength and paints detected areas green on input and black on output
	// input = cv::Mat single channel that you want to detect large white areas in, will be altered(green areas found will be painted here)
	// output = cv::Mat single channel that will have light pixel areas detected "erased" by filling the with black
	// whiteAreaMaxLimit = double to set the threshold for how big areas should be detected and "erased"
	// whiteAreaMin = double to set the minimum area threshold for areas to be detected
	// whiteLengthLimit = double to set the archLength threshold for length of areas to be detected, lareas longer than this value will be ignored
	void filterWhiteAreas(cv::Mat &input);

	// Detect HoughlinesP on left side and right side of an image,it uses the ROI split function to do this
	// The actual lines will be drawn on the output cv::Mat, the summary of search on both left and right sides
	// houghThreshold = double to set the threshold for houghtransform probabilistic
	// minLineLength = double to set the shortest line lengths to be used
	// maxLineGap = double to set the maximum line gap to be used before considering a line segments to be separate
	void HoughlinesPLR(cv::Mat &input);
};



#endif //OPENCVTEST_IMAGEPROCESSLIB_H
