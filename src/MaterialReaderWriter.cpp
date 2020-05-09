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


/*
Read BRDF parameters from a JSON file. 
Follow the documentation for the file format.
@param path_and_file - string containing the path and filename to the file.
@param brdf_material  - location of a material type to return the material parameters
@return true if file was succesfully read. 
*/
//static 
bool MaterialReaderWriter::readBRFD(std::string path_and_file, cs557::BRDFMaterial& brdf_material, bool verbose)
{

	if (!FileUtils::Exists(path_and_file)) {
		cout << "[ERROR] - (MaterialReaderWriter) Cannot find brdf material file " << path_and_file << "." << endl;
		return false;
	}


	cv::FileStorage fs(path_and_file, cv::FileStorage::READ);

	if(!fs.isOpened()) {
		cout << "[ERROR] - (MaterialReaderWriter) Cannot open  brdf material file " << path_and_file << "." << endl;
		return false;
	}


	cv::Vec3f brdf_albedo = cv::Vec3f(0.0f, 0.0f, 0.0f);
	cv::Vec3f brdf_lightColor = cv::Vec3f(23.47, 21.31, 20.79);
	cv::Vec3f brdf_F0 = cv::Vec3f(0.04, 0.04, 0.04);
	float brdf_intensity = 1.0f;
	float brdf_roughness = 0.5f;
	float brdf_metallic = 0.5f;
	float brdf0_ao = 0.0f;
	float brdf0_k1 = 0.1;
	float brdf0_k2 = 0.0;


	fs["brdf_albedo"] >> brdf_albedo;
	fs["brdf_lightColor"] >> brdf_lightColor;
	fs["brdf_F0"] >> brdf_F0;
	fs["brdf_intensity"] >> brdf_intensity;
	fs["brdf_roughness"] >> brdf_roughness;
	fs["brdf_metallic"] >> brdf_metallic;
	fs["brdf_ao"] >> brdf0_ao;
	fs["brdf_k1"] >> brdf0_k1;
	fs["brdf_k2"] >> brdf0_k2;
	

	fs.release();

	brdf_material.albedo =   glm::vec3(brdf_albedo[0], brdf_albedo[1], brdf_albedo[2]);
	brdf_material.lightColor = glm::vec3(brdf_lightColor[0], brdf_lightColor[1], brdf_lightColor[2])  * glm::vec3(brdf_intensity, brdf_intensity, brdf_intensity);
	brdf_material.F0 = glm::vec3(brdf_F0[0], brdf_F0[1], brdf_F0[2]) ;
	brdf_material.roughness = brdf_roughness;
	brdf_material.metallic = brdf_metallic;
	brdf_material.ao = brdf0_ao;
	brdf_material.k1 = brdf0_k1;
	brdf_material.k2 = brdf0_k2;



	if (verbose) {
	
	}


	return true;
}