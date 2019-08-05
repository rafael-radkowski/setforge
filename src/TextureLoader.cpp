#include "TextureLoader.h"


namespace TextureLoaderInternal {

	vector<cv::Mat>	textures_storage;
	vector<string>	files;

}

using namespace TextureLoaderInternal;

//static 
bool  TextureLoader::Load(string path_and_name, unsigned char** texture, int* width, int* height, int* channels)
{


	if (!std::experimental::filesystem::exists(path_and_name)) {
		cout << "[ERROR] - Could not find file " << path_and_name << ". Check the file" << endl;
		return false;
	}


	cv::Mat image = cv::imread(path_and_name, cv::IMREAD_ANYCOLOR);

	if (image.rows == 0 || image.cols == 0) {
		cout << "[ERROR] - Could not read image " << path_and_name << "." << endl;
		return false;
	}

	cv::Mat image_rs;
	int new_size = std::max(FindNextPower2(image.rows), FindNextPower2(image.cols));
	cv::resize(image, image_rs, cv::Size2f(new_size, new_size));

	// flip the texture to match the opengl coordinate system
	cv::flip(image_rs, image_rs, 0);

	textures_storage.push_back(image_rs);
	files.push_back(path_and_name);

	int i = textures_storage.size();


	(*texture) = (unsigned char*) textures_storage[i - 1].data;
	(*width) = image_rs.cols;
	(*height) = image_rs.rows;
	(*channels) = image_rs.channels();

	return true;

}


//static 
int TextureLoader::FindNextPower2(int size)
{
	float l = std::ceil(std::log2f(size));
	float next = std::pow(2,l);
	return (int)next;
}