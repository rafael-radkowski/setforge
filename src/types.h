#ifndef __TYPES__
#define __TYPES__

#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>

using namespace std;

namespace arlab
{

typedef enum _CameraModel
{
	USER,
	SPHERE,
	POLY,
	TREE,
	POSE

}CameraModel;


inline CameraModel CameraModelEnum(std::string m)
{
	if (m.compare("SPHERE") == 0) {
		return SPHERE;
	}
	else if (m.compare("USER") == 0) {
		return USER;
	}
	else if (m.compare("POLY") == 0) {
		return POLY;
	}
	else if (m.compare("TREE") == 0) {
		return TREE;
	}
	else if (m.compare("POSE") == 0) {
		return POSE;
	}
}

inline string  CameraModelString(CameraModel m)
{
	if (m == SPHERE) {
		return "SPHERE";
	}
	else if (m == USER) {
		return "USER";
	}
	else if (m == POLY) {
		return "POLY";
	}
	else if (m == TREE) {
		return "TREE";
	}
	else if (m == POSE) {
		return "POSE";
	}
}

}



inline string type2str(int type) {
	string r;

	uchar depth = type & CV_MAT_DEPTH_MASK;
	uchar chans = 1 + (type >> CV_CN_SHIFT);

	switch ( depth ) {
	case CV_8U:  r = "8U"; break;
	case CV_8S:  r = "8S"; break;
	case CV_16U: r = "16U"; break;
	case CV_16S: r = "16S"; break;
	case CV_32S: r = "32S"; break;
	case CV_32F: r = "32F"; break;
	case CV_64F: r = "64F"; break;
	default:     r = "User"; break;
	}

	r += "C";
	r += (chans+'0');

  return r;
}



#endif
