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

	_Arguments()
	{
		cam = POLY;

		current_path = "";
		model_path_and_file = "";
		output_path = "output";
		output_filename = "rendering";

		windows_width = 1280;
		window_height = 1024;
		image_width = 1280;
		image_height = 1024;

		segments = 10;
		rows = 10;
		camera_distance = 1.3;

		subdivisions = 0;
		bpt_levels = 0;

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