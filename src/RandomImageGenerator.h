#pragma once
/*
class RandomImageGenerator

This class reads filenames of a specific type from a folder and 
returns a list with all filenames.

Rafael Radkowski
Iowa State University
rafael@iastate.edu
Jan 25, 2019
MIT license

-------------------------------------
last edits:
Feb 5th, 2019, RR
- Added a normal map estimator to estimate normal maps from RGB images.
- Wrote the normal map with a stacked rendering to a file and logged the output.
May 3, 2019, RR
- Changed the csv parameter to roi_x, roi_y, roi_w, roi_h to align all tools.
Aug 8, 2019, RR
- Added a noise filter to the process
- Added a chromatic adaptation filter to the process. 
*/


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <time.h>

// opencv
#include <opencv2/opencv.hpp>

// local
#include "ReadImages.h" // read a set of random images from a file
#include "ImageLogReader.h" // load the log file 
#include "NormalMapSobel.h" // to generate normal maps
#include "MatHelpers.h"
#include "ImageFilter.h" // for chromatic adaptation
#include "NoiseFilter.h" // for noise

using namespace std;


class RandomImageGenerator
{
public:

	typedef enum {
		NOISE,
		CHROMATIC
	} Filtertype;

    /*
    Constructor
    @param image_height, image_width - the output image size in pixels. 
    */
    RandomImageGenerator(int image_height, int image_widht);
    ~RandomImageGenerator();

    /*
    Set the path to the folder containing the background images.
    @param image_path - vector of strings with the path to all files.
    @param type - the image type to be read.
    */
    void setImagePath(vector <string> image_paths, string type = "jpg");

     /*
    Set the path to the folder containing the rendered images along with the pose. 
    @param image_path - strings with the path to all files.
    @param type - the image type to be read.
    */
    void setRenderPath(string render_path, string type = "jpg");

    /*
    Set the output path and output type. 
    @param path - string containing the path to all output imags
    @param type - the output image type. 
    */
    void setOutputPath(string path);


	/*
	Set a filter and its parameters. The filter to be change is set with 'type'
	The parameters param1 and param2 depend on the filter to be set.
	@param type - a filter of type Filtertype, NOISE or CHROMATIC
	@param enable - enable or disable this filter with true or false. Default is false. 
	@param param1 - depends on the filter to set. 
				For NOISE: param1: sigma, param2: mean
				For CHROMATIC: no parameters
	*/
	void setFilter(Filtertype type, bool enable, float param1, float param2);

    /*
    Start processing.
	The function distinguises the "combine" mode and the "rendering only" mode using the 
	image path string (setImagePath(...)). If the string is empty, the tool assues that 
	only foreground renderings ought to be processed.
    @param num_images - integer with the number of images to generate. 
    @return - number of stored images
    */
    int process(int num_images);



private:

	/*
	Combine foreground images with rendering 
	@param num_images - integer with the number of images to generate. 
	@return - number of stored images
	*/
	int process_combine(int num_images);


	/*
	Just renders the forground image. No background image added
	@return - number of stored images
	*/
	int process_rendering(void);



    /*
    Adapt the aspect ratio of the image to meet the output aspect ratio
	@param iamge - the input image of type CV_8UC3
	@return - the adapted output image
    */
    cv::Mat adaptImage(cv::Mat& image);

	
    /*
    Adapt the aspect ratio of the rendering to meet the output aspect ratio.
	ToDo: Note that this function currently alsow looks for the ROI. 
	This is legacy code which can be removed. 
	@param iamge - the input image of type CV_8UC3
	@return - the adapted output image
    */
    cv::Mat adaptRendering(cv::Mat& image, int& x, int& y, int& width, int& height);


	/*
	Combine the background image with a foreground image. 
	Note that this function assumes that the entire background of the rendererd image is black. 
	If not, the function will not work since it applies a bitwise or operation to combine images 
	@param image1 - the background image. 
	@param image2 - the rendering, it must have a black background with all values (0,0,0)
	@param threshold - the threshold to segment the rendered background from its foreground. 
	@return the combine image stack. 
	*/
    cv::Mat combineImages(cv::Mat image1, cv::Mat image2, int threshold);

	/*
	Combine the background normal map with the foreground normal map. 
	Note that this function assumes that the entire background of the rendererd image is black. 
	If not, the function will not work since it applies a bitwise or operation to combine images 
	@param image1 - the background image. 
	@param image2 - the rendering, it must have a black background with all values (0,0,0)
	@param image_rgb - the color rendering. It is used to segment the bunny from its background. 
	@param threshold - the threshold to segment the rendered background from its foreground. 
	@return the combine image stack. 
	*/
	cv::Mat combineNormals(cv::Mat image1, cv::Mat image2, cv::Mat image_rgb, int threshold);

	//----------------------------------------
    // File output

	/*
	Create a data output file and create the header line. The file is of type csv. 
	*/
	bool writeHeader(void);

	/*
	Write the data of one image into the data file. 
	Note that this function is obsolete. 
	@param id - integer containing the image id. 
	@param image_rgb - the combined rgb image. 
	@param image_normal - the combined normal image. 
	@param data - additional log data such as the image path and files. 
	@param roi - the region of interest of the rendered object. 
	@return true - if the data was successfully written. 
	*/
	bool writeData(int id, cv::Mat& image_rgb, cv::Mat& image_normal, ImageLogReader::ImageLog& data, cv::Rect& roi);

	/*
	Write the data of one image into the data file. 

	@param id - integer containing the image id. 
	@param image_rgb - the combined rgb image. 
	@param image_normal - the combined normal image. 
	@param image_depth - the combined depth map.
	@param image_mask - the image mask. 
	@param data - additional log data such as the image path and files. 
	@param roi - the region of interest of the rendered object. 
	@return true - if the data was successfully written. 
	*/
	bool writeDataEx(int id, cv::Mat& image_rgb, cv::Mat& image_normal, cv::Mat& image_depth,cv::Mat& image_mask, ImageLogReader::ImageLog& data, cv::Rect& roi);

    //----------------------------------------
    // members

    vector<string>  image_filenames;
    vector<ImageLogReader::ImageLog>		rendered_files;

    vector<string>  _image_path;
    string          _image_type;


    string          _render_path;
    string          _render_type;

	string			_output_path;
	string			_output_file_name;

    int _image_height; 
    int _image_widht;

    int _rendering_height; 
    int _rendering_widht;


	ImageFilter		_imageFilter;
	bool			_with_chromatic_adpat; // enable the chromatic adaptation
	bool			_wtih_noise_adapt; // enable the noise filter
	float			_noise_sigma; // noise standard deviation
	float			_noise_mean;
};