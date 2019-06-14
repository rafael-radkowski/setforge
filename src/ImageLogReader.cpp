#include "ImageLogReader.h"



bool ImageLogReader::Read(string path_and_file, vector<ImageLog>* log )
{
	std::ifstream in(path_and_file, std::ifstream::in);

	if (!in.is_open()) {
		return false;
	}

	int line = 0;
	while (in)
	{
		string str;
		std::getline(in, str);

		if (line == 0)
		{
			line++;
			continue;
		}

		// index,rgb_file,normals_file,depth_file,mask_file,mat_file,tx,ty,tz,qx,qy,qz,qw,roix,roiy,roiw,roih
		vector<string> items = ImageLogReader::split( str, ',');

		if (items.size() > 0) {

			glm::vec3 pos(atof(items[6].c_str()), atof(items[7].c_str()), atof(items[8].c_str()));
			glm::quat q(atof(items[12].c_str()), atof(items[9].c_str()), atof(items[10].c_str()), atof(items[11].c_str())); // (w, x, y, z)
			cv::Rect2f roi(atof(items[13].c_str()), atof(items[14].c_str()), atof(items[15].c_str()), atof(items[16].c_str()));

			log->push_back(ImageLog(atoi(items[0].c_str()), items[1], items[2], items[3], items[4], items[5], pos, q, roi));
		}
	}


	return true;

}


vector<string>  ImageLogReader::split(string str, char delimiter) {
	vector<string> strvec;
	stringstream ss(str); // Turn the string into a stream.
	string tok;
 
	while(getline(ss, tok, delimiter)) {
		strvec.push_back(tok);
	}
 
	return strvec;
}