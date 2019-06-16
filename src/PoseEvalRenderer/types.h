#pragma once

// stl
#include <iostream>
#include <string>
#include <strstream>
#include <vector>
#include <list>
#include <numeric>
#include <experimental/filesystem>

// Eigen
#include "Eigen/Dense"



typedef struct PosePrediction {

	// ground truth
	Eigen::Vector3f		t0;
	Eigen::Vector3f		t1;

	// prediction
	Eigen::Quaternionf	q0;
	Eigen::Quaternionf	q1;


}PosePrediction;