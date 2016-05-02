

namespace imgPro
{
	class imageProcess
	{
	public:
		
		// Makes Mat single channel grayscale, adds blur, thresholds image
		// input = cv::Mat image you want to make greyscale 8-bit single channel image of (the input image will be changed!)
		// thres = double value for the threshold (0-255)
		static void makeBinary(cv::Mat &input, double thres);
		
		// Contrast the image
		static void Contrast(cv::Mat &input, double alpha, double beta);

		// Custom threshold 
		static void TH(cv::Mat &input, uchar light);

		// Fix light
		static void fixLight(cv::Mat &input, cv::Mat &output, uchar range);

		// Create a ROI
		// input = cv::Mat you want to extract ROI from
		// verticalPos = how far from the left and rigth edges you want the roi int
		// horizontalPos = how fram from top and bottom you want the roi int
		// skewed = how many pixels you want the top corners to be skewed, ie closer together than the bottom ones int (0=default for rectangle)
		static void ROIMaker(cv::Mat &input, cv::Mat &output, int verticalPos, int horizontalPos, int skewed);

		// Create a left and right ROI, 
		// input = cv::Mat you want to split, 
		// outputLeft = cv::Mat you want the left part of the image in,
		// outputRight = cv::Mat you want the right side of the image in
		static void ROISplitMaker(cv::Mat &input, cv::Mat &outputLeft, cv::Mat &outputRight);

		// Make greyscale image into skeleton-image,
		// Makes a skeleton image of a greyscale 8-bit one channel source image,
	    // input = cv::Mat you want to skeletonize, output = the cv::Mat you want the skeletonized image in
		static void skelMaker(cv::Mat &input, cv::Mat &output);

		// Algorithm that takes a single channel cv::Mat and detect polygon areas of a certain area and arclength and paints detected areas green on input and black on output
		// input = cv::Mat single channel that you want to detect large white areas in, will be altered(green areas found will be painted here)
		// output = cv::Mat single channel that will have light pixel areas detected "erased" by filling the with black
		// whiteAreaMaxLimit = double to set the threshold for how big areas should be detected and "erased"
		// whiteAreaMin = double to set the minimum area threshold for areas to be detected
		// whiteLengthLimit = double to set the archLength threshold for length of areas to be detected, lareas longer than this value will be ignored
		static void filterWhiteAreas(cv::Mat &input, cv::Mat &output, double whiteAreaMaxLimit, double whiteAreaMin, double whiteLengthLimit);

		// Detect HoughlinesP on left side and right side of an image,it uses the ROI split function to do this
		// The actual lines will be drawn on the output cv::Mat, the summary of search on both left and right sides
		// houghThreshold = double to set the threshold for houghtransform probabilistic
		// minLineLength = double to set the shortest line lengths to be used
		// maxLineGap = double to set the maximum line gap to be used before considering a line segments to be separate
		static void HoughlinesPLR(cv::Mat &input, cv::Mat output, double houghThreshold, double minLineLength, double maxLineGap);
	};
}


