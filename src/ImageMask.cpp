#include "ImageMask.h"


/*

*/
//static 
bool ImageMask::Extract(cv::Mat& image, cv::Mat& mask_out )
{
	float threshold_value = 0;
	cv::Mat img, grayscaleMat;
    img = image.clone();
    cvtColor(img, grayscaleMat, CV_RGB2GRAY);
    cv::threshold(grayscaleMat, mask_out, threshold_value, 255, CV_THRESH_BINARY);

    //cv::Mat result, mask_inv;
   // cv::bitwise_not(mask, mask_out);

	//std::cout << type2str(mask_out.type()) << std::endl;

	//cv::imshow("mask", mask_out);

	return true;
}



