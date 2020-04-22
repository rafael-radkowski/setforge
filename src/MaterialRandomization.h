#pragma once
/**
Class MaterialRandomization

@brief - generate a random color. 

The class generates a random color / material. 
The material is generated as a HSV color model and picks a random hue and saturation.
The brightness either remains constant or can also change. 

The values are converted to RGB values and returned as RGB.

For further information, see: https://docs.google.com/document/d/1xMMXdtCLFCR4rh1uQc7IpIVlAw9hsM409nWJOY54mX0

Rafael Radkowski
Iowa State University
rafael@iastate.edu
Dec. 12, 2019

MIT License
-----------------------------------------------------------------------------------------------------
Last edited:

April 21, 2020, RR
- Fixed a bug that mixed up std::min and std::max with the min/max macros. 

*/

// stl
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <random>
#include <time.h>
#include <iomanip>



class MaterialRandomization
{
public:

	MaterialRandomization();
	~MaterialRandomization();


	/**
	Set the hue value range. 
	@param min - a min value in the range from 0 to 1. 
	@param max - a max value in the range from 0 to 1. 
	Note that min must be smaller than max. 
	*/
	void setHueRange(float min, float max);


	/**
	Set the saturation value range. 
	@param min - a min value in the range from 0 to 1. 
	@param max - a max value in the range from 0 to 1. 
	Note that min must be smaller than max. 
	*/
	void setSaturationRange(float min, float max);


	/**
	Set the brightness value range. 
	@param min - a min value in the range from 0 to 1. 
	@param max - a max value in the range from 0 to 1. 
	Note that min must be smaller than max. 
	*/
	void setBrightnessRange(float min, float max);



	/**
	The brightness is not randomized by default. 
	Enable brighness randomizaton
	@param value true enables randomizatoin, default is false. 
	*/
	void setRandomizeBrightness(bool value);


	/**
	Create a random color value and return it as RGB value;
	*/
	std::vector<float> getRGB(void);

private:


	/*
	Create a random color value
	*/
	void createRandomColor(void);


	/*
	  @param - H(Hue): 0 - 360 degree (integer)
	  @param - S(Saturation): 0 - 1.00 (double)
	  @param - V(Value): 0 - 1.00 (double)
	  @return Output, array size 3, int
	 */
	std::vector<float> MaterialRandomization::HSVtoRGB(int H, double S, double V);


	float	_hue_min;
	float	_hue_max;
	float	_sat_min;
	float	_sat_max;
	float	_bright_min;
	float	_bright_max;

	bool	_randomize_brightness;

	// the random color value
	std::vector<float>		_hsv_values;

	bool	_verbose;

	std::random_device		_rd;	


};
