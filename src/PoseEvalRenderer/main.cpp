/*



Rafael Radkowski
Iowa State University
rafael@iastate.edu
MIT License
06/14/2019

----------------------------------------------------------------------
Edits:


*/

// stl
#include <iostream>
#include <string>
#include <fstream>


// local
#include "PoseEval3DMain.h"
#include "LogFileReader.h"


using namespace std;


int main(int argc, char** argv) {


	LogFileReader reader;
	reader.read("PosePredictions.txt");
	vector<PosePrediction> data = reader.getData();


	PoseEval3DMain* pose_eval = new PoseEval3DMain();
	pose_eval->loadGroundTruthModel("../data/stanford_bunny_centered.obj");
	pose_eval->loadPredictiviveModel("../data/stanford_bunny_green.obj");


	auto angle = 3.1415 / 2;
    auto sinA = std::sin(angle / 2);
    auto cosA = std::cos(angle / 2);

	Eigen::Quaternionf q;
    q.x() = 0 * sinA;
    q.y() = 0 * sinA;
    q.z() = 1 * sinA;
    q.w() = cosA;    

	q.x() =-0.104705;
    q.y() = 0.860554;
    q.z() = -0.196725;
    q.w() = -0.45802;    
	Eigen::Vector3f t = Eigen::Vector3f(-200.14042, 1.399, 1.6041);

	

	Eigen::Quaternionf q2;
	q2.x() = 0.0;
    q2.y() = 0.856054;
    q2.z() = -0.516886;
    q2.w() = 0.0;    


	pose_eval->setPredictivePose(t, q);



	Eigen::Matrix3f mat_offset, mat_rotation, mat;
	mat_offset << 1, 0 ,0 ,
       0 , 0 , -1,
	   0, 1, 0;

	mat_rotation << 0.850651, 0, 0.525731,
		0, 1, 0,
		-0.525731, -0, 0.850651;


	mat = mat_rotation * mat_offset;

	Eigen::Quaternionf q2m(mat_offset);


	Eigen::Matrix3f mat_q2(q2);
	cout << "q2:\n" <<  mat_q2 << endl;

	Eigen::Quaternionf q0(mat_rotation);

	cout << "q0:\n" << q0.x() << ", " <<  q0.y() << ", " <<  q0.z() << ", " <<  q0.w()  << endl;

	Eigen::Vector3f t_pose = Eigen::Vector3f(0.405633,-0.223808,-2.69291);

	//pose_eval->setGroundTruthPose(t + Eigen::Vector3f(0.1, 0,0), q2);
	pose_eval->setGroundTruthPose(t_pose,   q2 * q2m );



	pose_eval->calcPoseDelta(t, q, t + Eigen::Vector3f(10.1, 0, 0), q2);


	pose_eval->run();


	return 1;
}