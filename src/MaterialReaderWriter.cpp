#include "MaterialReaderWriter.h"




/*
Read material parameters from a JSON file.
Follow the documentation for the file format.
@param path_and_file - string containing the path and filename to the file.
@return true if file was succesfully read. 
*/
bool MaterialReaderWriter::readHSV(std::string path_and_file, MateriaHSVParams& hsv_params, bool verbose)
{
	//CameraParameters::Write(path_and_file);

	if (!FileUtils::Exists(path_and_file)) {
		cout << "[ERROR] - (MaterialReaderWriter) Cannot find file " << path_and_file << "." << endl;
		return false;
	}


	cv::FileStorage fs(path_and_file, cv::FileStorage::READ);

	if(!fs.isOpened()) {
		cout << "[ERROR] - (MaterialReaderWriter) Cannot open file " << path_and_file << "." << endl;
		return false;
	}

	cv::Vec2f h_range;
	cv::Vec2f s_range;
	cv::Vec2f v_range;
	bool	   with_v;

	fs["h_range"] >> h_range;
	fs["s_range"] >> s_range;
	fs["v_range"] >> v_range;
	fs["with_v"] >> with_v;
	

	fs.release();

	hsv_params.hue_min = h_range[0];
	hsv_params.hue_max = h_range[1];

	hsv_params.saturation_min = s_range[0];
	hsv_params.saturation_max = s_range[1];

	hsv_params.brightness_value_min = v_range[0];
	hsv_params.brightness_value_max = v_range[1];

	hsv_params.with_v = with_v;

	if (verbose) {
		cout << "h_range\n" << h_range << endl;
		cout << "s_range\n" << s_range << endl;
		cout << "v_range\n" << v_range << " : " << with_v << endl;
	}


	return true;
}