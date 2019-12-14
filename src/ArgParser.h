#ifndef __ARGPARSER__
#define __ARGPARSER__

// stl
#include <iostream>
#include <string>
#include <vector>

// local
#include "types.h"

using namespace std;

namespace arlab
{

typedef struct _Arguments
{
	CameraModel cam;

	string	model_path_and_file;
	string	output_path;
	string	output_filename;
	string	intrincic_params_file;
	string	current_path;

	int		windows_width;
	int		window_height;
	int		image_width;
	int		image_height;
	

	// sphere arguments 
	int		segments;
	int		rows;
	float	camera_distance;

	// polyheder arguments
	int		subdivisions;

	// balance pose tree levels
	int		bpt_levels;

	// helpers
	bool		verbose;
	bool		valid;

	// for random image generator
	int		num_images;
	float	lim_px;
	float	lim_nx;
	float	lim_py;
	float	lim_ny;
	float	lim_pz;
	float	lim_nz;


	// only get images from the upper hemisphere
	bool	upright;

	_Arguments()
	{
		cam = POLY;

		current_path = "";
		model_path_and_file = "";
		output_path = "output";
		output_filename = "rendering";

		windows_width = 1280;
		window_height = 1280;
		image_width = 1280;
		image_height = 1024;

		segments = 10;
		rows = 10;
		camera_distance = 1.3f;

		subdivisions = 0;
		bpt_levels = 0;

		num_images = 6000;
		lim_px = 1.0;
		lim_nx = -1.0;
		lim_py = 1.0;
		lim_ny = -1.0;
		lim_pz = -1.5;
		lim_nz = -3.0;

		upright = false;

		verbose = false;
		valid = false;
	}


}Arguments;


class ArgParser
{
public:

	/*
	Parse the arguments
	@param argc - number of arguments
	@param argv - the argument line
	@return struct with arguments
	*/
	static Arguments Parse(int& argc, char** argv);

	/*
	Display help
	*/
	static void Help(void);

	/*
	Display all arguments
	*/
	static void Display(void);

private:

	/*
	Extract the current path
	*/
	static void Path(char* path);

	/*
	Extract the current path
	*/
	static void ParamError(string option);
};



}// namespace arlab


#endif