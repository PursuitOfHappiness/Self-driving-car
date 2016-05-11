#include "imageProcessLib.h"


imageProcess::imageProcess() :
    usesMakeBinary(false),
		usesContrast(false),
		usesThreshold(false),
		usesfixLight(false),
		usesRoiMaker(false),
		usesSkelMaker(false),
		usesFilterWhiteAreas(false),
    usesFilterWhiteLines(false),
		usesHoughLinesPLR(false),
		origWidth(0),
		origHeight(0),
		alpha(0),
		beta(0),
		threshold(0),
		light(0),
		range(0),
		verticalPos(0),
		horizontalPos(0),
		skewed(0),
		whiteAreaMaxLimit(0),
		angle(0),
    areaMaxLimit(0),
		houghThreshold(0),
		minLineLength(0),
		maxLineGap(0),
		frame()
    {}

// The actual call to process image (for Mat)
void imageProcess::processImage(cv::Mat &image) {
	frame = &image;
	if (frame == NULL) {
		std::cout << "Can't read frame" << std::endl;
	}
	else {
		theInterestingPart();
	}

}


// The function running all different image processes who's bool's set to true
void imageProcess::theInterestingPart() {
	setSize(320, 240);
	if (usesRoiMaker) {
		ROIMaker();
	}
	if (usesContrast) {
		Contrast();
	}
	if (usesfixLight) {
		fixLight();
	}
	if (usesMakeBinary) {
		makeBinary();
	}
	if (usesThreshold) {
		Theshold();
	}
	if (usesFilterWhiteAreas) {
		filterWhiteAreas();
	}
  if (usesFilterWhiteLines) {
    filterWhiteLines();
  }
	if (usesSkelMaker) {
		skelMaker();
	}
	if (usesHoughLinesPLR) {
		HoughlinesPLR();
	}
	resetSize();

}

void imageProcess::setSize(int width, int height) {
	origWidth = (short)frame->cols;
	origHeight = (short)frame->rows;
	cv::resize(*frame, *frame, cv::Size(width, height));
}

// Resets size to original
void imageProcess::resetSize() {
	cv::resize(*frame, *frame, cv::Size(origWidth, origHeight));
}

void imageProcess::setContrast(double alphaVal, double betaVal, bool isActive) {
	usesContrast = isActive;
	this->alpha = alphaVal;
	this->beta = betaVal;
}

void imageProcess::setThreshold(double thesVal, bool isActive) {
	usesMakeBinary = isActive;
	this->threshold = thesVal;
}

void imageProcess::setCustomThreshold(uchar lightVal, bool isActive) {
	usesThreshold = isActive;
	this->light = lightVal;
}

void imageProcess::setLightRange(uchar rangeVal, bool isActive) {
	usesfixLight = isActive;
	this->range = rangeVal;
}

void imageProcess::setRoi(short verticalPosVal, short horizontalPosVal, short skewedVal, bool isActive) {
	usesRoiMaker = isActive;
	this->verticalPos = verticalPosVal;
	this->horizontalPos = horizontalPosVal;
	this->skewed = skewedVal;
}

void imageProcess::setWhiteFilter(double areaMaxLimitVal, bool isActive) {
	usesFilterWhiteAreas = isActive;
	this->whiteAreaMaxLimit = areaMaxLimitVal;

}

void imageProcess::setFilterWhiteLines (float angleVal, double areaMaxLimitVal, bool isActive) {
  usesFilterWhiteLines = isActive;
  this->angle = angleVal;
  this->areaMaxLimit = areaMaxLimitVal;
}

void imageProcess::setHoughLines(uchar thresholdVal, double maxLineGapVal, double minLineLengthVal, bool isActive) {
	usesHoughLinesPLR = isActive;
	this->houghThreshold = thresholdVal;
	this->minLineLength = minLineLengthVal;
	this->maxLineGap = maxLineGapVal;

}

void imageProcess::setContrastBool(bool active){
	usesContrast = active;
}

void imageProcess::setMakeBinaryBool(bool active){
	usesMakeBinary = active;
}

void imageProcess::setCustomThresholdBool(bool active){
	usesThreshold = active;
}

void imageProcess::setFixLightBool(bool active){
	usesfixLight = active;
}

void imageProcess::setROIBool(bool active){
	usesRoiMaker = active;
}

void imageProcess::setWhiteFilterBool(bool active){
	usesFilterWhiteAreas = active;
}

void imageProcess::setFilterWhiteLines(bool active) {
  usesFilterWhiteLines = active;
}

void imageProcess::setHoughLinesPLRBool(bool active){
	usesHoughLinesPLR = active;
}

void imageProcess::makeBinary()
{

	cv::cvtColor(*frame, *frame, CV_BGR2GRAY); // make 8-bit single channel
	cv::GaussianBlur(*frame, *frame, cv::Size(5, 5), 2, 2); //Add some blur
	cv::threshold(*frame, *frame, threshold, 255, cv::THRESH_BINARY); // Threshold the image
}

void imageProcess::Contrast() {

	cv::Mat temp = cv::Mat::zeros(frame->size(), frame->type());

	for (int y = 0; y < frame->rows; y++) {
		for (int x = 0; x < frame->cols; x++) {
			for (int c = 0; c < 3; c++) {
				temp.at<cv::Vec3b>(y, x)[c] =
					cv::saturate_cast<uchar>(alpha * (frame->at<cv::Vec3b>(y, x)[c]) + beta);
			}
		}
	}
	temp.copyTo(*frame);
}

void imageProcess::Theshold() {

	cv::Mat temp;
	frame->copyTo(temp);

	//uchar val = 0;

	for (int y = 0; y < frame->rows; y++) {
		for (int x = 0; x < frame->cols; x++) {
			uchar c;
			c = (frame->at<cv::Vec3b>(y, x)[0] / 3) + (frame->at<cv::Vec3b>(y, x)[1] / 3) + (frame->at<cv::Vec3b>(y, x)[2] / 3);
			if (c >= light) {
				temp.at<cv::Vec3b>(y, x)[0] = c;
				temp.at<cv::Vec3b>(y, x)[1] = c;
				temp.at<cv::Vec3b>(y, x)[2] = c;
			}
			else {
				temp.at<cv::Vec3b>(y, x)[0] = 0;
				temp.at<cv::Vec3b>(y, x)[1] = 0;
				temp.at<cv::Vec3b>(y, x)[2] = 0;
			}
		}
	}
	temp.copyTo(*frame);

}

void imageProcess::fixLight() {

	cv::Mat temp;
	frame->copyTo(temp);
	if (frame->channels() == 3)
		cvtColor(*frame, temp, CV_RGB2GRAY);
	else if (frame->channels() == 1) {
		frame->copyTo(temp);
	}
	else {
		return;
	}

	for (int i = 0; i < frame->rows; i++) {

		uchar c = 0;
		for (int j = 0; j < frame->cols; j++) {

			if (c < temp.at<uchar>(i, j)) {

				c = temp.at<uchar>(i, j);

			}

		}
		for (int k = 0; k < frame->cols; k++) {


			if ((c - range) > temp.at<uchar>(i, k)) {

				temp.at<uchar>(i, k) = 0;

			}


		}

	}

	temp.copyTo(*frame);

}

void imageProcess::ROIMaker() {

	//Mat for the main ROI mat mask
	cv::Mat roi(frame->size(), CV_8U, cv::Scalar(0));
	cv::Mat temp;

	//Getting size info into variables
	int width = 0, height = 0;
	width = frame->size().width;
	height = frame->size().height;

	std::vector<cv::Point2i> roiPoints;
	roiPoints.push_back(cv::Point2i(verticalPos, height - horizontalPos)); // Lower left corner ROI, set value to change the ROI
	roiPoints.push_back(cv::Point2i(width - verticalPos, height - horizontalPos)); // Lower right corner ROI
	roiPoints.push_back(cv::Point2i(width - verticalPos - skewed, horizontalPos)); // Upper right corner ROI
	roiPoints.push_back(cv::Point2i(verticalPos + skewed, horizontalPos)); // Upper left corner ROI

	std::vector<cv::Point2i> ROI_Poly;
	cv::approxPolyDP(roiPoints, ROI_Poly, 1.0, true);
	cv::fillConvexPoly(roi, &ROI_Poly[0], ROI_Poly.size(), 255, 8, 0);


	frame->copyTo(temp, roi);
	temp.copyTo(*frame);
}

void imageProcess::skelMaker() {
	cv::Mat skel(frame->size(), CV_8U, cv::Scalar(0));
	cv::Mat temp(frame->size(), CV_8U);
	cv::Mat eroded(frame->size(), CV_8U);
	cv::Mat original(frame->size(), CV_8U);
	frame->copyTo(original);
	cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));
	bool done;
	do
	{
		cv::erode(original, eroded, element);
		cv::dilate(eroded, temp, element); // temp = open(img)
		cv::subtract(original, temp, temp);
		cv::bitwise_or(skel, temp, skel);
		eroded.copyTo(original);

		done = (cv::countNonZero(original) == 0);
	} while (!done);
	//Copy the skeleton to the output variable
	skel.copyTo(*frame);
}

void imageProcess::ROISplitMaker(cv::Mat &outputLeft, cv::Mat &outputRight) {

	//Getting size info into variables
	int width = 0, height = 0;
	width = frame->size().width;
	height = frame->size().height;

	// The 4-points of ROI LEFT, basically a rectangle of original frame
	std::vector<cv::Point2i> LeftPoints;
	LeftPoints.push_back(cv::Point2i(0, height)); // Lower left corner ROI
	LeftPoints.push_back(cv::Point2i(width / 2, height)); // Lower right corner ROI
	LeftPoints.push_back(cv::Point2i(width / 2, 0)); // Upper right corner ROI
	LeftPoints.push_back(cv::Point2i(0, 0)); // Upper left corner ROI

											 // The 4-points of ROI RIGHT basically a rectangle of original frame
	std::vector<cv::Point2i> RightPoints;
	RightPoints.push_back(cv::Point2i(width / 2, height)); // Lower left corner ROI
	RightPoints.push_back(cv::Point2i(width, height)); // Lower right corner ROI
	RightPoints.push_back(cv::Point2i(width, 0)); // Upper right corner ROI
	RightPoints.push_back(cv::Point2i(width / 2, 0)); // Upper left corner ROI

													  //Create mat to hold left and right masks of ROI of image, all black
	cv::Mat LeftROI(frame->size(), CV_8UC1, cv::Scalar(0));
	cv::Mat RightROI(frame->size(), CV_8UC1, cv::Scalar(0));

	std::vector<cv::Point2i> ROILeft_Poly;
	std::vector<cv::Point2i> ROIRight_Poly;

	cv::fillConvexPoly(LeftROI, &ROILeft_Poly[0], ROILeft_Poly.size(), 255, 8, 0);
	cv::fillConvexPoly(RightROI, &ROIRight_Poly[0], ROIRight_Poly.size(), 255, 8, 0);

	//Copy skeleton version of image to the left and right roi mats
	frame->copyTo(outputLeft, LeftROI);
	frame->copyTo(outputRight, RightROI);
}

void imageProcess::filterWhiteAreas() {

	std::vector<std::vector<cv::Point>> contours; // Vector for storing contour of large white pixels areas
	cv::Mat temp; //Temp Mat to not change the original
	std::vector<cv::Vec4i> hierarchy;

	frame->copyTo(temp);
	//find contours will change the src image, so we use a copy to find large white cluster of pixels
	cv::findContours(temp, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
	std::vector<std::vector<cv::Point>> contours_poly(contours.size());
	std::vector<cv::RotatedRect> boundRect(contours.size());

	for (size_t i = 0; i < contours.size(); i++)
	{

		double a = contourArea(contours[i], false);


		if ((a > whiteAreaMaxLimit)) {

			approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 8, true);
			drawContours(*frame, contours_poly, i, cv::Scalar(0, 0, 0), -1, 8, hierarchy, 0, cv::Point(11, 11));
		}

	}
}


void imageProcess::filterWhiteLines() {

std::vector<std::vector<cv::Point> > contours; // Vector for storing contour of hite pixels areas
cv::Mat temp; //Temp Mat to not change the original
std::vector<cv::Vec4i> hierarchy;
frame->copyTo(temp);
cv::findContours(temp, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
std::vector<cv::RotatedRect> rotRect(contours.size());

for (size_t i = 0; i < contours.size(); i++)
{
  rotRect[i] = cv::minAreaRect(cv::Mat(contours[i]));
  double a = contourArea(contours[i], false);
  double c = rotRect[i].size.width;
  double d = rotRect[i].size.height;
  float e = rotRect[i].angle;

  if ((e > angle) && (c > d)&& (a < areaMaxLimit)) {

    drawContours(*frame, contours,i, cv::Scalar(0, 0, 0), -1, CV_AA, hierarchy, 2, cv::Point(0, 0));

  }

}
}

void imageProcess::HoughlinesPLR() {

	//To store the found lines for left and right
	std::vector<cv::Vec4i> linesHL;
	std::vector<cv::Vec4i> linesHR;
	cv::Mat Left;
	cv::Mat Right;

	ROISplitMaker(Left, Right);

	// detect lines LeftImage using HoughLinesProbalistic
	HoughLinesP(Left, linesHL, 1, CV_PI / 180, houghThreshold, minLineLength, maxLineGap);

	// detect lines RightImage using HoughLinesProbalistic
	HoughLinesP(Right, linesHR, 1, CV_PI / 180, houghThreshold, minLineLength, maxLineGap);

	for (size_t i = 0; i < linesHL.size(); i++)
	{
		cv::Vec4i l = linesHL[i];
		// draw the lines
		cv::line(*frame, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255, 255, 255), 3);

	}
	for (size_t i = 0; i < linesHR.size(); i++)
	{
		cv::Vec4i r = linesHR[i];
		// draw the lines
		cv::line(*frame, cv::Point(r[0], r[1]), cv::Point(r[2], r[3]), cv::Scalar(255, 255, 255), 3);

	}



}
