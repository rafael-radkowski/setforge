#pragma once

// stl
#include <iostream>
#include <string>
#include <vector>

// local
#include "types.h"

using namespace std;

namespace arlab
{




class Parser
{
public:
	typedef struct _Arguments
	{
		CameraModel cam;


		string	background_images_path;
		string	background_images_type;

		string	rendered_images_log_file;
		string	rendered_images_type;

		string  current_path;
		string	output_path;

		int		image_width;
		int		image_height;

		int		num_images;

		bool		valid; // helper to indicate that the parameter set is valid
		bool		verbose;

		float	noise_sigma;
		bool	with_noise;
		bool	with_chromatic;

		_Arguments()
		{
			background_images_path = "";
			background_images_type = "";
			rendered_images_log_file  = "";
			rendered_images_type = "png";
			current_path = "";
			output_path = "./batch";

			image_width = 512;
			image_height = 512;

			noise_sigma = 0.1;
			with_noise = false;
			with_chromatic = false;

			num_images = 10000;
			verbose = false;
			valid = false;
		}


	}Arguments;



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

