
#include <opencv2/highgui/highgui.hpp> //for Mat (and Core is also inlcuded through this)
#include <opencv2/imgproc/imgproc.hpp> //Canny
#include "opencv2/photo/photo.hpp"
#include "opencv2/features2d.hpp"

#include <iostream>


using namespace cv;
using namespace std;


int main(int argc, const char** argv)
{

	Mat imgrey, imgthres,imgthres2, imgthres3;
	
	// Threshold parameters
	double threshold1 = 203;
	double threshold2 = 255;

	//Parameter for the threshold of search of white pixelareas to block out
	double whiteAreaLimit = 4500.00;

	//Hough parameters
	int minLineLength = 20,
		maxLineGap = 2,
		houghThreshold = 40;;
	
		
	/*   Different videos I load into the program  */
	//VideoCapture cap("road.mp4");
   // VideoCapture cap("road2.mp4");
	//VideoCapture cap("track.mp4");
	//VideoCapture cap("track2.webm");
	//VideoCapture cap("track3_320_lowexp.mp4");
	VideoCapture cap("track4.mp4");
	//VideoCapture cap("track6.mp4");

	//Check 
	if (!cap.isOpened()) {
		cout << "Cannot open video file" << endl;
		return -1;
	}

	
	
	//Getting viedo info into variables
	int width = 0, height = 0, fps = 0, fourcc = 0;
	width = static_cast<int>(cap.get(CV_CAP_PROP_FRAME_WIDTH));
	height = static_cast<int>(cap.get(CV_CAP_PROP_FRAME_HEIGHT));
	fps = static_cast<int>(cap.get(CV_CAP_PROP_FPS));
	fourcc = static_cast<int>(cap.get(CV_CAP_PROP_FOURCC));
	

	// The 4-points of ROI need to be set for different sizes
	vector<Point2i> roiPoints;
	roiPoints.push_back(Point2i(30, height-140)); // Lower left corner ROI, set value to change the ROI
	roiPoints.push_back(Point2i(width-60, height-140)); // Lower right corner ROI
	roiPoints.push_back(Point2i(width - 120,160)); // Upper right corner ROI
	roiPoints.push_back(Point2i(180, 160)); // Upper left corner ROI

    // The 4-points of ROI LEFT, basically a rectangle of original frame
	vector<Point2i> LeftPoints;
	LeftPoints.push_back(Point2i(0, height)); // Lower left corner ROI
	LeftPoints.push_back(Point2i(width / 2, height)); // Lower right corner ROI
	LeftPoints.push_back(Point2i(width/2, 0)); // Upper right corner ROI
	LeftPoints.push_back(Point2i(0, 0)); // Upper left corner ROI
	
	// The 4-points of ROI RIGHT basically a rectangle of original frame
	vector<Point2i> RightPoints;
	RightPoints.push_back(Point2i(width/2, height)); // Lower left corner ROI
	RightPoints.push_back(Point2i(width, height)); // Lower right corner ROI
	RightPoints.push_back(Point2i(width, 0)); // Upper right corner ROI
	RightPoints.push_back(Point2i(width / 2, 0)); // Upper left corner ROI
	

	while (1) {

		Mat frame, frameCopy, LeftImage, RightImage;


		vector<Vec4i> lines;
		vector<vector<Point>> contours; // Vector for storing contour of large white pixels areas
		
		bool aSuccess = cap.read(frame);
		
		if (!aSuccess) {
			cout << "Cannot read video file" << endl;
			break;
		}
		 
		   //Mat for the main ROI mat mask
			Mat roi(frame.size(), CV_8U, cv::Scalar(0));
			



			//Create mat to hold left and right masks of ROI of image, all black
			Mat LeftROI(frame.size(), CV_8UC1, cv::Scalar(0));
			Mat RightROI(frame.size(), CV_8UC1, cv::Scalar(0));
			
			//Below the code is using the points for ROI for left and right to draw write polygons for the masks

			vector<Point2i> ROI_Poly;
			vector<Point2i> ROILeft_Poly;
			vector<Point2i> ROIRight_Poly;

			approxPolyDP(roiPoints, ROI_Poly, 1.0, true);
			approxPolyDP(LeftPoints, ROILeft_Poly, 1.0, true);
			approxPolyDP(RightPoints, ROIRight_Poly, 1.0, true);

			// Fill polygons white
			fillConvexPoly(roi, &ROI_Poly[0], ROI_Poly.size(), 255, 8, 0);
			fillConvexPoly(LeftROI, &ROILeft_Poly[0], ROILeft_Poly.size(), 255, 8, 0);
			fillConvexPoly(RightROI, &ROIRight_Poly[0], ROIRight_Poly.size(), 255, 8, 0);
			
		
			

			frame.copyTo(frameCopy, roi);
			
			//Just a check, left it here
			if (frameCopy.empty()) //check whether the image is loaded or not
			{
				cout << "Error : Image cant be loaded, FOOL!" << endl;
				system("pause"); //wait for a key press
				return -1;
			}

			
					
			Mat skel(frameCopy.size(), CV_8U, cv::Scalar(0));
			Mat temp(frameCopy.size(), CV_8U);
			Mat eroded(frameCopy.size(), CV_8U);
			

			// Grayscale - put into new Mat imgrey
			cvtColor(frameCopy, imgrey, COLOR_BGR2GRAY); //make a grey image from original
			
			// Blur on the same Mat imgrey
			GaussianBlur(imgrey, imgrey, Size(5, 5), 2, 2);

			//Threshold			
			threshold(imgrey, imgthres, threshold1, threshold2, THRESH_BINARY); //set threshold 
						
			//Separate copy of threshold so we make skeleton of the original and block out light looking for pixels imghtres2
			imgthres.copyTo(imgthres2);
			
			//Do a skeleton using the copy of thresholded image, imghtres, might be slowing down too much
			Mat element = getStructuringElement(MORPH_CROSS, Size(3, 3));
			bool done;
			do
			{
				cv::erode(imgthres, eroded, element);
				cv::dilate(eroded, temp, element); // temp = open(img)
				cv::subtract(imgthres, temp, temp);
				cv::bitwise_or(skel, temp, skel);
				eroded.copyTo(imgthres);

				done = (cv::countNonZero(imgthres) == 0);
			} while (!done);


			
			//find contours will change the src image, imgthres2, so we use a copy to find large white cluster of pixels
			findContours(imgthres2, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
			vector<vector<Point> > contours_poly(contours.size());

			for (size_t i = 0; i < contours.size(); i++)
			{
				
				double a = contourArea(contours[i], false);
				//Prints out the areas found of white pixels
				cout << a << endl;
				if ((a>whiteAreaLimit)) {
					
					approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
					//Draws a green polygon on the frame for testing/demo
					drawContours(frame, contours_poly, i, Scalar(0, 255, 0), -1, 8, vector<Vec4i>(), 0, Point());
					//Draws a black polygon on the binary image where we get too much light
					drawContours(skel, contours_poly, i, Scalar(0, 0, 0), -1, 8, vector<Vec4i>(), 0, Point());
				}

			}
		
			//Copy skeleton version of image to the left and right roi mats
			skel.copyTo(LeftImage, LeftROI);
			skel.copyTo(RightImage, RightROI);

			//To store the found lines for left and right
			vector<Vec4i> linesHL;
			vector<Vec4i> linesHR;


			// detect lines LeftImage using regular HoughLines
			/*HoughLines(LeftImage, linesHL, 1, CV_PI / 180, 60, 0, 0);*/

			// detect lines LeftImage using HoughLinesProbalistic
			HoughLinesP(LeftImage, linesHL, 1, CV_PI / 180, houghThreshold, minLineLength, maxLineGap);


			// detect lines RightImage using HoughLinesProbalistic
			/*HoughLines(RightImage, linesHR, 1, CV_PI / 180, 60, 0, 0);*/

			// detect lines RightImage using regular HoughLines
			HoughLinesP(RightImage, linesHR, 1, CV_PI / 180, houghThreshold, minLineLength, maxLineGap);

			

			for (size_t i = 0; i < linesHL.size(); i++)
			{
				Vec4i l = linesHL[i];
				// draw the lines
				line(frame, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0, 255),3);

			}
			for (size_t i = 0; i < linesHR.size(); i++)
			{
				Vec4i l = linesHR[i];
				// draw the lines
				line(frame, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255),3);
				
			}
	


		/*   Different windows using OpenCV imshow for viewing different Mat's   */
		imshow("Lane Detection", frame); //show the frame in the window
		//imshow("frameCopy", frameCopy); //show frameCopy
		//imshow("skel", skel); //show skeleton
		//imshow("imgthres", imgthres); //show imgthres
		//imshow("imgthres3", imgthres3); //show imgthres3
		//imshow("imgthres2", imgthres2); //show imgthres
		//imshow("LeftImage", LeftImage); //show LeftImage
		//imshow("RightImage", RightImage); //show RightImage
		
		if (waitKey(30) == 27) {
			cout << "User pressed esc key for 30 secs" << endl;
			break;
		}
	
	}
	return 0;

}


