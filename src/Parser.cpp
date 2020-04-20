#include "Parser.h"


using namespace arlab;


namespace ParserTypes{

	Parser::Arguments opt;
	int error_count = 0;
}

using namespace ParserTypes;


/*
Parse the arguments
@param argc - number of arguments
@param argv - the argument line
@return struct with arguments
*/
Parser::Arguments Parser::Parse(int& argc, char** argv)
{
	//cout << argc << endl;
	//cout << argv[0] << endl;

	if (argc < 2) {
		Help();
		return opt;
	}

	opt.valid = true;

	// extract the path
	Path(argv[0]);

	int pos = 1;
	while(pos < argc)
	{
		string c_arg(argv[pos]); 

		if (c_arg.compare("-o") == 0) { // output path
			if (argc >= pos+1) opt.output_path = string(argv[pos+1]);
			else ParamError(c_arg);
		}
		else if(c_arg.compare("-img_w") == 0){ // image width 
			if (argc >= pos) opt.image_width = atoi( string(argv[pos+1]).c_str() );
			else ParamError(c_arg);
		}
		else if(c_arg.compare("-img_h") == 0){ // image width 
			if (argc >= pos) opt.image_height =atoi(  string(argv[pos+1]).c_str() );
			else ParamError(c_arg);
		}
		else if (c_arg.compare("-ipath") == 0) { // input images path
			if (argc >= pos+1) opt.background_images_path = string(argv[pos+1]);
			else ParamError(c_arg);
		}
		else if (c_arg.compare("-itype") == 0) { // input images type
			if (argc >= pos+1) opt.background_images_type = string(argv[pos+1]);
			else ParamError(c_arg);
		}
		else if (c_arg.compare("-rlog") == 0) { // rendered images path
			if (argc >= pos+1) opt.rendered_images_log_file = string(argv[pos+1]);
			else ParamError(c_arg);
		}
		else if (c_arg.compare("-n") == 0) { // number of images
			if (argc >= pos+1) opt.num_images = atoi(string(argv[pos+1]).c_str());
			else ParamError(c_arg);
		}
		else if (c_arg.compare("-noise") == 0) { // number of images
			opt.with_noise = true;
			if (argc >= pos+1) opt.noise_sigma = atof(string(argv[pos+1]).c_str());
			else ParamError(c_arg);
		}
		else if(c_arg.compare("-chromatic") == 0){
			opt.with_chromatic = true;
		}
		else if(c_arg.compare("-help") == 0 || c_arg.compare("-h") == 0){ // help
			Help();
		}
		else if(c_arg.compare("-verbose") == 0 ){ // help
			opt.verbose = true;
		}

		pos++;
	}


	if (opt.verbose)
		Display();

	return opt;
}


/*
Display help
*/
//static
void Parser::Help(void)
{
	cout << "[ERROR] - Missing parameters\nUsage:" << endl;
	cout << "ImageGen -ipath [path to image] -rpath [path to renderings]" << endl;
	cout << "Parameters:" << endl;
	cout << "\t-o [param] - set the output path" << endl;
	cout << "\t-img_w [param] \t- set the widht of the output image in pixels (integer)." << endl;
	cout << "\t-img_h [param] \t- set the height of the output image in pixels (integer)." << endl;
	cout << "\t-ipath [param] \t- set the path of all background images." << endl;
	cout << "\t-itype [param] \t- set the path of all input image types, e.g., jpeg, jpg, png." << endl;
	cout << "\t-rlog [param] \t- set the path and filename to the logfile DatasetRenderer created." << endl;
	cout << "\t-n [param] \t- set the number of images to be generated (integer)" << endl;
	cout << "\tOptional:" << endl;
	cout << "\t-h \t- shows this help dialog" << endl;
	cout << "\t-noise [param] \t- enable noise and set the noise sigma value param (float)." << endl;
	cout << "\t-chromatic \t- enable chromatic image adapation." << endl;

	

	cout << "\nExample: ImageGen -ipath ./images -rpath ./renderings -img_w 512 -img_h 512 -o batch \n" << endl;
}


/*
Display all arguments
*/
//static 
void Parser::Display(void)
{
	std::cout << "\nParameters:" << endl;
	std::cout << "Image input path:\t" << opt.background_images_path << endl;
	std::cout << "Image input type:\t" << opt.background_images_type << endl;
	std::cout << "Renderings input path:\t" << opt.rendered_images_log_file << endl;
	std::cout << "Output path:\t" << opt.output_path << endl;
	std::cout << "Image width:\t" << opt.image_width << endl;
	std::cout << "Image height:\t" << opt.image_height << endl;
}


/*
Extract the current path
*/
//static 
void Parser::Path(char* path)
{
	string str(path);

	int idx = str.find_last_of("\\");
	if(idx == -1){ 
		idx = str.find_last_of("/");
	}
	opt.current_path = str.substr(0, idx+1);
}

/*
Extract the current path
*/
//static 
void Parser::ParamError(string option)
{
	cout << "[ERROR] - Parameter for option " << option << " is missing or invalid." << endl;
	error_count++;
}