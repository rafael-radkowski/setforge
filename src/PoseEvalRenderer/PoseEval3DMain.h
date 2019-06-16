#pragma once
/*
Class PoseEval3DMain

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
#include <vector>


// GLM include files
#define GLM_FORCE_INLINE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>  // transformation
#include <glm/gtx/quaternion.hpp> // quaternions

// eigen 3
#include <Eigen/Dense>

// GL
#include "Model3DExt.h"

// local
#include "GLRenderer.h"
#include "types.h"

using namespace std;
using namespace std::placeholders;


class PoseEval3DMain {

public:

	/*
	Constructor
	*/
	PoseEval3DMain(int wHeight = 1024, int wWidth = 1024);

	/*
	Destructor
	*/
	~PoseEval3DMain();

	/*
	Load a ground truth model from an obj file. 
	@path_and_file - relative or absolute path to an obj file.
	Note that the model must be of type obj.
	@return true - if the model could be loaded
	*/
	bool loadGroundTruthModel(string path_and_file);


	/*
	Load a ground truth model from an obj file. 
	@path_and_file - relative or absolute path to an obj file.
	Note that the model must be of type obj.
	@return true - if the model could be loaded
	*/
	bool loadPredictiviveModel(string path_and_file);

	/*
	Set the pose prediction data. 
	*/
	bool setPosePredictionData(vector<PosePrediction> data);


	/*
	Evaluate all the poses in the pose prediction dataset. 
	*/
	bool evalAllPoses(void);


	/*
	Set the pose of the ground truth object.
	@param t - the translation
	@param q - the orientation
	*/
	bool setGroundTruthPose(Eigen::Vector3f t, Eigen::Quaternionf q);

	/*
	Set the pose of the predictive object
	@param t - the translation
	@param q - the orientation
	*/
	bool setPredictivePose(Eigen::Vector3f t, Eigen::Quaternionf q);


	void calcPoseDelta(Eigen::Vector3f t0, Eigen::Quaternionf q0, Eigen::Vector3f t1, Eigen::Quaternionf q1);


	/*
	This starts the main loop.
	The function is  blocking and will not return until
	the renderer closes
	@return true - if the window closes. 
	*/
	bool run(void);

private:

	// init the GL renderer
	bool init_renderer(void);


	// init the GL renderer
	void  render_fcn(glm::mat4 pm, glm::mat4 vm);



	//------------------------------------------------------------------
	// Members

	// the main renderer
	GLRenderer*		_renderer;
	int				_wHeight;
	int				_wWidth;

	// Reference model
	Model3DExt		_ground_truth_model;
	bool			_ground_truth_ready;

	Model3DExt		_predictive_model;
	bool			_predictive_ready;

	// Prediction

	vector<PosePrediction>	_data;

};