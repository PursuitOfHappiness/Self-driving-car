#include "imageProcessLib.h"
#include <opencv2/highgui/highgui.hpp> //for Mat (and Core is also inlcuded through this)
#include <opencv2/imgproc/imgproc.hpp> //Canny
#include "opencv2/photo/photo.hpp"
#include "opencv2/features2d.hpp"


namespace imgPro
{

	void imageProcess::makeBinary(cv::Mat &input, double thres)
	{
		cv::cvtColor(input, input, cv::COLOR_BGR2GRAY); // make 8-bit singel channel
		cv::GaussianBlur(input, input, cv::Size(5, 5), 2, 2); //Add some blur
		cv::threshold(input, input, thres, 255, cv::THRESH_BINARY); // Threshold the image 
	}

	void imageProcess::Contrast(cv::Mat &input, double alpha, double beta) {

		cv::Mat temp = cv::Mat::zeros(input.size(), input.type());

		for (int y = 0; y < input.rows; y++) {
			for (int x = 0; x < input.cols; x++) {
				for (int c = 0; c < 3; c++) {
					temp.at<cv::Vec3b>(y, x)[c] =
						cv::saturate_cast<uchar>(alpha * (input.at<cv::Vec3b>(y, x)[c]) + beta);
				}
			}
		}
		temp.copyTo(input);
	}

	void imageProcess::TH(cv::Mat &input, uchar light) {

		cv::Mat temp;
		input.copyTo(temp);

		uchar val = 0;

		for (int y = 0; y < input.rows; y++) {
			for (int x = 0; x < input.rows; x++) {
				uchar c;
				c = (input.at<cv::Vec3b>(y, x)[0] / 3) + (input.at<cv::Vec3b>(y, x)[1] / 3) + (input.at<cv::Vec3b>(y, x)[2] / 3);
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
		temp.copyTo(input);

	}

	void imageProcess::fixLight(cv::Mat &input, cv::Mat &output, uchar range) {

		cv::Mat temp;
		input.copyTo(temp);
		if (input.channels() == 3)
			cvtColor(input, temp, CV_RGB2GRAY);
		else if (input.channels() == 1) {
			input.copyTo(temp);
		}
		else {
			return;
		}

		for (int i = 0; i < input.rows; i++) {

			uchar c = 0;
			for (int j = 0; j < input.cols; j++) {

				if (c < temp.at<uchar>(i, j)) {

					c = temp.at<uchar>(i, j);

				}

			}
			for (int k = 0; k < input.cols; k++) {


				if ((c - range) > temp.at<uchar>(i, k)) {

					temp.at<uchar>(i, k) = 0;

				}


			}

		}

		temp.copyTo(output);

	}
		
	void imageProcess::ROIMaker(cv::Mat &input, cv::Mat &output, int verticalPos, int horizontalPos,int skewed) {
		
		//Mat for the main ROI mat mask
		cv::Mat roi(input.size(), CV_8U, cv::Scalar(0));

		
		//Getting size info into variables
		int width = 0, height = 0;
		width = input.size().width;
		height = input.size().height;
		
		std::vector<cv::Point2i> roiPoints;
		roiPoints.push_back(cv::Point2i(verticalPos, height - horizontalPos)); // Lower left corner ROI, set value to change the ROI
		roiPoints.push_back(cv::Point2i(width - verticalPos, height - horizontalPos)); // Lower right corner ROI
		roiPoints.push_back(cv::Point2i(width - verticalPos - skewed, horizontalPos)); // Upper right corner ROI
		roiPoints.push_back(cv::Point2i(verticalPos + skewed, horizontalPos)); // Upper left corner ROI

		std::vector<cv::Point2i> ROI_Poly;
		cv::approxPolyDP(roiPoints, ROI_Poly, 1.0, true);
		cv::fillConvexPoly(roi, &ROI_Poly[0], ROI_Poly.size(), 255, 8, 0);


		input.copyTo(output, roi);
	}

	
	void imageProcess::skelMaker(cv::Mat &input, cv::Mat &output) {
		cv::Mat skel(input.size(), CV_8U, cv::Scalar(0));
		cv::Mat temp(input.size(), CV_8U);
		cv::Mat eroded(input.size(), CV_8U);
		cv::Mat original(input.size(), CV_8U);
		input.copyTo(original);
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
		skel.copyTo(output);
	}

	void imageProcess::ROISplitMaker(cv::Mat &input, cv::Mat &outputLeft, cv::Mat &outputRight) {
	
		//Getting size info into variables
		int width = 0, height = 0;
		width = input.size().width;
		height = input.size().height;

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
		cv::Mat LeftROI(input.size(), CV_8UC1, cv::Scalar(0));
		cv::Mat RightROI(input.size(), CV_8UC1, cv::Scalar(0));

		std::vector<cv::Point2i> ROILeft_Poly;
		std::vector<cv::Point2i> ROIRight_Poly;

		cv::fillConvexPoly(LeftROI, &ROILeft_Poly[0], ROILeft_Poly.size(), 255, 8, 0);
		cv::fillConvexPoly(RightROI, &ROIRight_Poly[0], ROIRight_Poly.size(), 255, 8, 0);

		//Copy skeleton version of image to the left and right roi mats
		input.copyTo(outputLeft, LeftROI);
		input.copyTo(outputRight, RightROI);
	}


}
	

