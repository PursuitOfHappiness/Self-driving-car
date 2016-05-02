


namespace imgPro
{
	class imageProcess
	{
	public:
		// Makes Mat single channel grayscale, adds blur, thresholds image
		// input = cv::Mat image you want to make greyscale 8-bit singel channel image of (the input image will be changed!)
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

		//Create a left and right ROI, 
		//input = cv::Mat you want to split, 
		//outputLeft = cv::Mat you want the left part of the image in,
		//outputRight = cv::Mat you want the right side of the image in
		static void ROISplitMaker(cv::Mat &input, cv::Mat &outputLeft, cv::Mat &outputRight);

		// Make greyscale image into skeleton-image,
		// Makes a skeleton image of a greyscale 8-bit one channel source image,
	    // input = cv::Mat you want to skeletonize, output = the cv::Mat you want the skeletonized image in
		static void skelMaker(cv::Mat &input, cv::Mat &output);
	};
}


