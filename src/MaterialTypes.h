#pragma once



typedef struct MateriaHSVParams
{

	// hue parameters, give in a range between 0 to 255
	float hue_min;
	float hue_max;

	// staturation parameters, given in a range between 0 to 1.
	float saturation_min;
	float saturation_max;

	// the v value given in a range from 0 to 1. 
	float brightness_value_min;
	float brightness_value_max;

	bool with_v;

	MateriaHSVParams()
	{
		hue_min = 30.0;
		hue_max = 240.0;
		saturation_min = 0.5;
		saturation_max = 1;
		brightness_value_min = 0.8;
		brightness_value_max = 1.0;
		with_v = false;
	}

}MateriaHSVParams;
