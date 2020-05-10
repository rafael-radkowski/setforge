#pragma once


// stl
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <random>
#include <time.h>
#include <iomanip>

// opencv
#include <opencv2/opencv.hpp>

// local
#include "FileUtils.h"
#include "MaterialTypes.h"
#include "BRDFTypes.h"


class MaterialReaderWriter {

public:

	
	
	/*
	Read material parameters from a JSON file.
	Follow the documentation for the file format.
	@param path_and_file - string containing the path and filename to the file.
	@param MateriaHSVParams  - struct to return the hsv parameters. 
	@return true if file was succesfully read. 
	*/
	static bool readHSV(std::string path_and_file, MateriaHSVParams& hsv_params, bool verbose = false);



	/*
	Read BRDF parameters from a JSON file. 
	Follow the documentation for the file format.
	@param path_and_file - string containing the path and filename to the file.
	@param brdf_material  - location of a material type to return the material parameters
	@return true if file was succesfully read. 
	*/
	static bool readBRFD(std::string path_and_file, cs557::BRDFMaterial& brdf_material, bool verbose = false);


private:



};
