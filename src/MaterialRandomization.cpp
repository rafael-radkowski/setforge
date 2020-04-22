#include "MaterialRandomization.h"


MaterialRandomization::MaterialRandomization()
{
	
	_hue_min = 0.1;
	_hue_max = 0.9;
	_sat_min = 0.5;
	_sat_max = 1.0;
	_bright_min = 0.6;
	_bright_max = 0.8;

	_randomize_brightness = false;
	_verbose = false;

	_hsv_values.push_back(0.0);
	_hsv_values.push_back(0.0);
	_hsv_values.push_back(0.0);
}


MaterialRandomization::~MaterialRandomization()
{

}


/**
Set the hue value range. 
@param min - a min value in the range from 0 to 1. 
@param max - a max value in the range from 0 to 1. 
Note that min must be smaller than max. 
*/
void MaterialRandomization::setHueRange(float min, float max)
{
	if (min > max) {
		float temp = min;
		min = max;
		max = temp;
	}
		
	_hue_min = (std::min)(1.0f, (std::max)(0.0f, min));
	_hue_max = (std::min)(1.0f, (std::max)(0.0f, max));

}


/**
Set the saturation value range. 
@param min - a min value in the range from 0 to 1. 
@param max - a max value in the range from 0 to 1. 
Note that min must be smaller than max. 
*/
void MaterialRandomization::setSaturationRange(float min, float max)
{
	if (min > max) {
		float temp = min;
		min = max;
		max = temp;
	}
		
	_sat_min = (std::min)(1.0f, (std::max)(0.0f, min));
	_sat_max = (std::min)(1.0f, (std::max)(0.0f, max));
}


/**
Set the brightness value range. 
@param min - a min value in the range from 0 to 1. 
@param max - a max value in the range from 0 to 1. 
Note that min must be smaller than max. 
*/
void MaterialRandomization::setBrightnessRange(float min, float max)
{
	if (min > max) {
		float temp = min;
		min = max;
		max = temp;
	}
		
	_bright_min = (std::min)(1.0f, (std::max)(0.0f, min));
	_bright_max = (std::min)(1.0f, (std::max)(0.0f, max));
}



/**
The brightness is not randomized by default. 
Enable brighness randomizaton
@param value true enables randomizatoin, default is false. 
*/
void MaterialRandomization::setRandomizeBrightness(bool value)
{
	_randomize_brightness = value;
}


/**
Create a random color value and return it as RGB value;
*/
std::vector<float> MaterialRandomization::getRGB(void)
{
	// create random color
	createRandomColor();

	// convert the color to rgb
	std::vector<float> rgb = HSVtoRGB( int( _hsv_values[0] ) , _hsv_values[1], _hsv_values[2]);

	/*if (_verbose) {
		std::cout << std::fixed;
		std::cout << std::setprecision(2);
		std::cout << "[INFO] - Random color r " << rgb[0] << "\tg " << rgb[1] << "\tv " << rgb[2]  << std::endl;
	}*/

	return rgb;

}



/*
Create a random color value
*/
void MaterialRandomization::createRandomColor(void)
{
	// position
	std::mt19937 generator(_rd());
    std::uniform_real_distribution<float> distribution_hue(_hue_min,_hue_max);
	std::uniform_real_distribution<float> distribution_sat(_sat_min,_sat_max);
	std::uniform_real_distribution<float> distribution_bright(_bright_min,_bright_max);
    float h = distribution_hue(generator) * 360.0; 
	float s = distribution_sat(generator); 
	float v = _bright_max;
	
	if(_randomize_brightness)
		v = distribution_bright(generator); 

	if (_verbose) {
		std::cout << std::fixed;
		std::cout << std::setprecision(2);
		std::cout << "[INFO] - Random color h " << h << "\ts " << s << "\tv " << v  << std::endl;
	}
	
	_hsv_values[0] = h;
	_hsv_values[1] = s;
	_hsv_values[2] = v;
}



/*
 * H(Hue): 0 - 360 degree (integer)
 * S(Saturation): 0 - 1.00 (double)
 * V(Value): 0 - 1.00 (double)
 * 
 * output[3]: Output, array size 3, int
 */
std::vector<float> MaterialRandomization::HSVtoRGB(int H, double S, double V) {
	
	double C = S * V;
	double X = C * (1 - abs(fmod(H / 60.0, 2) - 1));
	double m = V - C;
	double Rs, Gs, Bs;

	if(H >= 0 && H < 60) {
		Rs = C;
		Gs = X;
		Bs = 0;	
	}
	else if(H >= 60 && H < 120) {	
		Rs = X;
		Gs = C;
		Bs = 0;	
	}
	else if(H >= 120 && H < 180) {
		Rs = 0;
		Gs = C;
		Bs = X;	
	}
	else if(H >= 180 && H < 240) {
		Rs = 0;
		Gs = X;
		Bs = C;	
	}
	else if(H >= 240 && H < 300) {
		Rs = X;
		Gs = 0;
		Bs = C;	
	}
	else {
		Rs = C;
		Gs = 0;
		Bs = X;	
	}
	
	std::vector<float> output(3,0.0);

	output[0] = (Rs + m) * 255;
	output[1] = (Gs + m) * 255;
	output[2] = (Bs + m) * 255;

	return output;
}


