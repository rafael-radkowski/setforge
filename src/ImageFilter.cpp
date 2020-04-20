#include "ImageFilter.h"

/**
* Constructor for ImageFilter
* Default Chromatic Adaptation Algorithm is Gray World Algorithm
*/
ImageFilter::ImageFilter()
{
	_method = CHROMATIC;
}

/**
* Deconstructor for ImageFilter
*/
ImageFilter::~ImageFilter()
{
}


/**
* Set the Chromatic Adaptation Algorithm to Gray World Algorithm
*/
void ImageFilter::setFilterMethod(FilterMethod method) 
{
	_method = method;
}


/**
* Set the Chromatic Adaptation Algorithm to Template Image Adaptation
*
* @param img The template image Mat.
*/
void ImageFilter::setChromaticTemplate(cv::Mat& img)
{
	_template_img = img;
	_method = CHROMATIC;

	//cv::imshow("template", img);
	//cv::waitKey();
}



/*!
	Apply the filter currently set. Note that the filter
	expects to get an RGB image. It processes all three channels individually and merges them when finished/ 
	@param input - A CV_8UC3 image. 
	@return - A CV_8UC3 image with the  resolution of the input image
*/
bool ImageFilter::apply(cv::Mat& input, cv::Mat& dst) {


	if (input.rows == 0 || input.cols == 0) {
		std::cout << "[ERROR] - ImageFiter: no valid input image provided." << std::endl;
		return false;
	}

	switch (_method) {
	case GRAYWORLD:
		dst = grayworldFilter(input);
		break;
	case WHITEBALANCE:
		dst = whitebalanceFilter(input);
		break;
	case CHROMATIC:
		dst = chromaticFilter(input);
		break;
	}

	//cv::imshow("output", dst);
	//cv::waitKey();

	return true;

}

/*
Apply the grayworld filter. It creates a gray-tone image.
@param input - A CV_8UC3 image. 
@return - A CV_8UC3 image with the  resolution of the input image
*/
cv::Mat ImageFilter::grayworldFilter(cv::Mat& src)
{
	Mat dst;

	vector<Mat> RGBchannels;
	split(src, RGBchannels);
	Mat BChanel = RGBchannels.at(0);
	Mat GChanel = RGBchannels.at(1);
	Mat RChanel = RGBchannels.at(2);

	double BChannelAvg = 0;
	double GChannelAvg = 0;
	double RChannelAvg = 0;

	BChannelAvg = mean(BChanel)[0];
	GChannelAvg = mean(GChanel)[0];
	RChannelAvg = mean(RChanel)[0];

	double K = (BChannelAvg + GChannelAvg + RChannelAvg) / 3;
	double Kb = K / BChannelAvg;
	double Kg = K / GChannelAvg;
	double Kr = K / RChannelAvg;

	addWeighted(BChanel, Kb, 0, 0, 0, BChanel);
	addWeighted(GChanel, Kg, 0, 0, 0, GChanel);
	addWeighted(RChanel, Kr, 0, 0, 0, RChanel);

	merge(RGBchannels, dst);

	return dst;
}


/*
Apply a white-balance filter. 
@param input - A CV_8UC3 image. 
@return - A CV_8UC3 image with the  resolution of the input image
*/
cv::Mat ImageFilter::whitebalanceFilter(cv::Mat& src)
{
	Mat dst;

	int row = src.rows;
	int col = src.cols;
	dst = Mat(row, col, CV_8UC3);
	int HistRGB[767] = { 0 };
	int MaxVal = 0;
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			MaxVal = max(MaxVal, (int)src.at<Vec3b>(i, j)[0]);
			MaxVal = max(MaxVal, (int)src.at<Vec3b>(i, j)[1]);
			MaxVal = max(MaxVal, (int)src.at<Vec3b>(i, j)[2]);
			int sum = src.at<Vec3b>(i, j)[0] + src.at<Vec3b>(i, j)[1] + src.at<Vec3b>(i, j)[2];
			HistRGB[sum]++;
		}
	}
	int Threshold = 0;
	int sum = 0;
	for (int i = 766; i >= 0; i--) {
		sum += HistRGB[i];
		if (sum > row * col * 0.1) {
			Threshold = i;
			break;
		}
	}
	int AvgB = 0;
	int AvgG = 0;
	int AvgR = 0;
	int cnt = 0;
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			int sumP = src.at<Vec3b>(i, j)[0] + src.at<Vec3b>(i, j)[1] + src.at<Vec3b>(i, j)[2];
			if (sumP > Threshold) {
				AvgB += src.at<Vec3b>(i, j)[0];
				AvgG += src.at<Vec3b>(i, j)[1];
				AvgR += src.at<Vec3b>(i, j)[2];
				cnt++;
			}
		}
	}
	AvgB /= cnt;
	AvgG /= cnt;
	AvgR /= cnt;
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			int Blue = src.at<Vec3b>(i, j)[0] * MaxVal / AvgB;
			int Green = src.at<Vec3b>(i, j)[1] * MaxVal / AvgG;
			int Red = src.at<Vec3b>(i, j)[2] * MaxVal / AvgR;
			if (Red > 255) {
				Red = 255;
			}
			else if (Red < 0) {
				Red = 0;
			}
			if (Green > 255) {
				Green = 255;
			}
			else if (Green < 0) {
				Green = 0;
			}
			if (Blue > 255) {
				Blue = 255;
			}
			else if (Blue < 0) {
				Blue = 0;
			}
			dst.at<Vec3b>(i, j)[0] = Blue;
			dst.at<Vec3b>(i, j)[1] = Green;
			dst.at<Vec3b>(i, j)[2] = Red;
		}
	}

	return dst;
}


/*
Apply a choromatic filter. It adapts the color tone and hue to a 
given template. Use setChromaticTemplate() to set the template. 
The function will not proceed without a template. 
*/
cv::Mat ImageFilter::chromaticFilter(cv::Mat& src)
{
	Mat dst;

	if (_template_img.rows == 0 || _template_img.cols == 0) {
		std::cout << "[ERROR] - ImageFiter: no valid template image provided." << std::endl;
		return dst;
	}


	vector<Mat> RGBchannels;
	split(src, RGBchannels);
	Mat BChanel = RGBchannels.at(0);
	Mat GChanel = RGBchannels.at(1);
	Mat RChanel = RGBchannels.at(2);

	double BChannelAvg = 0;
	double GChannelAvg = 0;
	double RChannelAvg = 0;

	int numB = cv::countNonZero(BChanel);
	int numG = cv::countNonZero(GChanel);
	int numR = cv::countNonZero(RChanel);

	cv::Scalar sums = cv::sum(src);

	//BChannelAvg = mean(BChanel)[0];
	//GChannelAvg = mean(GChanel)[0];
	//RChannelAvg = mean(RChanel)[0];

	BChannelAvg = sums[0] / numB;
	GChannelAvg = sums[1] / numG;
	RChannelAvg = sums[2] / numR;
		
	vector<Mat> tmpRGBchannels;
	split(_template_img, tmpRGBchannels);
	Mat tmpBChanel = tmpRGBchannels.at(0);
	Mat tmpGChanel = tmpRGBchannels.at(1);
	Mat tmpRChanel = tmpRGBchannels.at(2);

	double tmpBChannelAvg = 0;
	double tmpGChannelAvg = 0;
	double tmpRChannelAvg = 0;

	tmpBChannelAvg = mean(tmpBChanel)[0];
	tmpGChannelAvg = mean(tmpGChanel)[0];
	tmpRChannelAvg = mean(tmpRChanel)[0];
		
		
	BChanel = BChanel*(tmpBChannelAvg / BChannelAvg);
	GChanel = GChanel*(tmpGChannelAvg / GChannelAvg);
	RChanel = RChanel*(tmpRChannelAvg / RChannelAvg);

	merge(RGBchannels, dst);

	return dst;
}


