#pragma once

// stl
#include <iostream>
#include <string>
#include <vector>

// Eigen 3
#include <Eigen/Dense>


// local



class GeometryUtils
{
public:

	/*!
	*/
	static bool CalcBoundingBox(std::vector<Eigen::Vector3f>& points, Eigen::Vector3f& boundingbox_size);


	/*
	Identify the centroid of the object. 
	@param centroid - location to store the coordinates of the centroid
	*/
	static bool CalcGeometricCentroid(std::vector<Eigen::Vector3f>& points, Eigen::Vector3f& centroid);

};