#pragma once



// stl
#include <iostream>
#include <fstream>
#include <string>
#include <strstream>
#include <vector>
#include <list>
#include <numeric>
#include <experimental/filesystem>

// Eigen
#include "Eigen/Dense"

#include "types.h"

using namespace std;

class LogFileReader
{
public:

	/*
	Constructor
	*/
	LogFileReader();
	~LogFileReader();


	bool read(string path_and_file);

	vector<PosePrediction> getData(void);


private:

	vector<string>  LogFileReader::split(string str, char delimiter);


	vector<PosePrediction>	_data;

};