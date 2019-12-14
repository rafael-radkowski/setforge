#pragma once
/*
class ReadImages

This class reads filenames of a specific type from a folder and 
returns a list with all filenames.

Rafael Radkowski
Iowa State University
rafael@iastate.edu
Jan 25, 2019
MIT license
---------------------------------------------------------------------------------
Last edited:

Dec 10, 2019, RR:
- Added #include "FileUtils.h" 
*/


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

// opencv
#include <opencv2/opencv.hpp>

// local
#include "FileUtils.h"

using namespace std;


class ReadImages
{
public:
    /*
    Get all files of a specific type from a folder
    @param path_to_image - string containg the path to the repo folder
    @param type - the image type. 
    @return - vector containing string to all files. 
    */
    static vector<string>  GetList(string path_to_image, string type = "jpeg");


    /*
    Get all files of a specific type from a folder. It can read the files from multiple folders
    @param path_to_image - a vector containing string with the relative or absolute path to folder. 
    @param type - the image type. 
    @return - vector containing string to all files. 
    */
    static vector<string>  GetList(vector<string> path_to_image, string type = "jpeg");


private:

    /*
    Read all files from a folder
    @param path - string containing the path to all files. 
    */
    static vector<string> getFileList(string path);

};