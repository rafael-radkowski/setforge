#include "LogFileReader.h"



LogFileReader::LogFileReader()
{

}


LogFileReader::~LogFileReader()
{

}


bool LogFileReader::read(string path_and_file)
{

	if (!std::experimental::filesystem::exists(path_and_file)) {
		cout << "[ERROR] - File " << path_and_file << " does not exist." << endl;
		return false;
	}

	std::ifstream in(path_and_file, std::iostream::in);
	if (!in.is_open())
	{
		cout << "[ERROR] - Could not open file " << path_and_file << "." << endl;
		return false;
	}


	while (in) {
		string in_str;
		in >> in_str;
		vector<string> values = split(in_str, ',');

		if (values.size() == 0) continue;

		PosePrediction pose;
		pose.t0.x() = atof(values[0].c_str());
		pose.t0.y() = atof(values[1].c_str());
		pose.t0.z() = atof(values[2].c_str());
		pose.q0.x() = atof(values[3].c_str());
		pose.q0.y() = atof(values[4].c_str());
		pose.q0.z() = atof(values[5].c_str());
		pose.q0.w() = atof(values[6].c_str());

		pose.t1.x() = atof(values[7].c_str());
		pose.t1.y() = atof(values[8].c_str());
		pose.t1.z() = atof(values[9].c_str());
		pose.q1.x() = atof(values[10].c_str());
		pose.q1.y() = atof(values[11].c_str());
		pose.q1.z() = atof(values[12].c_str());
		pose.q1.w() = atof(values[13].c_str());

		_data.push_back(pose);
	}
	in.close();
	
	return true;
}

vector<PosePrediction> LogFileReader::getData(void)
{
	return _data;
}


vector<string>  LogFileReader::split(string str, char delimiter) {
	vector<string> strvec;
	stringstream ss(str); // Turn the string into a stream.
	string tok;
 
	while(getline(ss, tok, delimiter)) {
		strvec.push_back(tok);
	}
 
	return strvec;
}