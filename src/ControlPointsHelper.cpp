#include "ControlPointsHelper.h"



 
bool ControlPointsHelper::Read(const std::string path_and_filename, CPType& type, std::vector<glm::vec2>& control_points)
{

	if (!FileUtils::Exists(path_and_filename))
	{
		std::cout << "[ERROR] ControlPointsHelper -  file " << path_and_filename << "does not exist.\n" << std::endl;;
		return false;
	}

	int N = 0;
	control_points.clear();


	std::ifstream in_file(path_and_filename, std::ofstream::in);

	if(!in_file.is_open()){
		std::cout << "[ERROR] ControlPointsHelper - Cannot open file " << path_and_filename << " for reading." << std::endl;
		return false;
	}

	std::string label;
	in_file >> label;

	if (label.compare("BBox") == 0) {
		type = ControlPointsHelper::BBox;
		N = 9; // expect 8 corner pionts and the center
	}

	int i = 0;
	while (in_file)
	{
		if(i > N-1) break;

		std::string line;
		in_file >> line;
		std::vector<std::string> values = split(line, ',');

		if (values.size() >= 2) {
			//std::cout << i << ": " << std::stof(values[0]) << " : " <<std::stof(values[1]) << std::endl;
			control_points.push_back(glm::vec2(std::stof(values[0]),std::stof(values[1])));
		}
		else {
			std::cout << "[ERROR] ControlPointsHelper - Error while reading line from " << path_and_filename << "." << std::endl;
		}
		i++;
	}
	in_file.close();


	// -------------------------------------------------------------
	// fill up the vector with points if the number does not match the expected number

	if (control_points.size() < N) {
		for (int i = 0; i < N - control_points.size(); i++) {
			control_points.push_back(glm::vec2(-1.0, -1.0));
		}
	}
	

	return true;
}



bool ControlPointsHelper::Write(const std::string path_and_filename, const CPType type, std::vector<glm::vec2>& control_points)
{
	std::ofstream out_file(path_and_filename, std::ofstream::out);

	if(!out_file.is_open()){
		std::cout << "[ERROR] ControlPointsHelper - Cannot open file " << path_and_filename << " for writing (1)." << std::endl;
		return false;
	}

	switch (type){
	case CPType::BBox:
		out_file << "BBox\n";
		break;
	default:
		out_file << "Unknown\n";
		break;
	}


	for each (glm::vec2 p in control_points) {
		out_file << p.x << "," << p.y << "\n";
	}

	out_file.close();


	return true;
}

 
bool ControlPointsHelper::Read3D(const std::string path_and_filename, CPType& type, std::vector<glm::vec3>& control_points)
{

	if (!FileUtils::Exists(path_and_filename))
	{
		std::cout << "[ERROR] ControlPointsHelper -  file " << path_and_filename << "does not exist.\n" << std::endl;;
		return false;
	}

	int N = 0;
	control_points.clear();


	std::ifstream in_file(path_and_filename, std::ofstream::in);

	if(!in_file.is_open()){
		std::cout << "[ERROR] ControlPointsHelper - Cannot open file " << path_and_filename << " for reading." << std::endl;
		return false;
	}

	std::string label;
	in_file >> label;

	if (label.compare("BBoxLocal") == 0) {
		type = ControlPointsHelper::BBoxLocal;
		N = 8; // expect 8 corner pionts and the center
	}

	int i = 0;
	while (in_file)
	{
		if(i > N-1) break;

		std::string line;
		in_file >> line;
		std::vector<std::string> values = split(line, ',');

		if (values.size() >= 3) {
			//std::cout << i << ": " << std::stof(values[0]) << " : " <<std::stof(values[1]) << std::endl;
			control_points.push_back(glm::vec3( std::stof(values[0]), std::stof(values[1]), std::stof(values[2])));
		}
		else {
			std::cout << "[ERROR] ControlPointsHelper - Error while reading line from " << path_and_filename << "." << std::endl;
		}
		i++;
	}
	in_file.close();


	// -------------------------------------------------------------
	// fill up the vector with points if the number does not match the expected number

	if (control_points.size() < N) {
		for (int i = 0; i < N - control_points.size(); i++) {
			control_points.push_back(glm::vec3(-1.0, -1.0, -1.0));
		}
	}
	


	return true;
}


bool ControlPointsHelper::Write3D(const std::string path_and_filename, const CPType type,  std::vector<glm::vec3>& control_points)
{
	std::ofstream out_file(path_and_filename, std::ofstream::out);

	if(!out_file.is_open()){
		std::cout << "[ERROR] ControlPointsHelper - Cannot open file " << path_and_filename << " for writing (2)." << std::endl;
		return false;
	}

	switch (type){
	case CPType::BBox:
		out_file << "BBox\n";
		break;
	case CPType::BBoxLocal:
		out_file << "BBoxLocal\n";
		break;
	default:
		out_file << "Unknown\n";
		break;
	}


	for each (glm::vec3 p in control_points) {
		out_file << p.x << "," << p.y <<  "," << p.z << "\n";
	}

	out_file.close();
	return true;
}


std::vector<string>  ControlPointsHelper::split(std::string str, char delimiter) 
{
	std::vector<std::string> strvec;
	std::stringstream ss(str); // Turn the string into a stream.
	std::string tok;
 
	while(getline(ss, tok, delimiter)) {
		strvec.push_back(tok);
	}
 
	return strvec;
}