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




	Eigen::Matrix3f mat_offset, mat_scanned;


	mat_offset <<	1, 0 ,0 ,
					0 , 0 , -1,
					0, 1, 0;


	Eigen::Quaternionf q2m(mat_offset);

	mat_scanned << 1, 0, 0,
					0, -1, 0,
					0, 0, -1;


	Eigen::Quaternionf q_scan(mat_scanned);

	

	// step 2034
	//pose t: -0.0862471, -0.173864,-3.11568, pr:  -0.08009093999862671, -0.15807102620601654,-2.8466033935546875
	//pose q: 0.131851, -0.40845,-0.0597571, 0.901228,  pr:  0.13670428097248077, -0.32154902815818787,-0.022113297134637833 ,0.8510690927505493

	//Saved at step 2024
	//pose t: -0.684793, -0.252719,-2.85771, pr:  -0.6578999161720276, -0.23359517753124237,-2.7290570735931396
	//pose q: -0.325752, 0.317365,-0.117098, 0.882866,  pr:  -0.2969219982624054, 0.2794656455516815,-0.10111062973737717 ,0.8881614804267883

	//Saved at step 2014
	//pose t: -0.0329885, -0.465623,-3.59478, pr:  -0.020379403606057167, -0.3578830063343048,-3.2595200538635254
	//pose q: 0.0378501, 0.934673,-0.104792, -0.337597,  pr:  -0.0024761203676462173, 0.7256592512130737,-0.09607072174549103 ,-0.10026872158050537


	//0.0111249	-0.261019	1.16087	-0.00872697	-0.510253	1.16087	0.0051613


	//Saved at step 2004
	//pose t: 1.08915, -0.141013,-3.2991, pr:  1.029910922050476, -0.16753888130187988,-3.035698413848877
	//pose q: -0.276319, 0.723635,-0.451988, 0.442388,  pr:  -0.11399850249290466, 0.6900088787078857,-0.2423483282327652 ,0.4264727532863617
	

	//0.0111249	0.261019	-1.16087	-0.00252131	-0.247279	-1.16087	0.00982049


	Eigen::Vector3f t_gt = Eigen::Vector3f(1.08915, -0.141013,-3.2991);
	Eigen::Vector3f t_pred = Eigen::Vector3f( 1.029910922050476, -0.16753888130187988,-3.035698413848877);
	//Eigen::Vector3f t_pred = Eigen::Vector3f(0.0111249,	0.261019,	-1.16087);

	Eigen::Quaternionf q_gt(-0.276319, 0.723635,-0.451988, 0.442388);
	Eigen::Quaternionf q_pred( -0.11399850249290466, 0.6900088787078857,-0.2423483282327652 ,0.4264727532863617);
	//Eigen::Quaternionf q_pred(0.00982049, -0.00252131,	-0.247279	,-1.16087);
	
	pose_eval->setPredictivePose(t_gt-t_pred, q_pred * q2m);
	pose_eval->setGroundTruthPose(  Eigen::Vector3f(0.0, 0, 0),   q_gt * q2m );

	//pose_eval->setPredictivePose( t_pred,   q_pred * q2m );
	//pose_eval->setGroundTruthPose( t_gt,   q_gt * q2m );



	pose_eval->calcPoseDelta(t, q, t + Eigen::Vector3f(10.1, 0, 0), q2);


	pose_eval->run();


	return 1;
}