#include "PoseEval3DMain.h"




/*
Constructor
*/
PoseEval3DMain::PoseEval3DMain(int wHeight, int wWidth)
{

	_wHeight = wHeight;
	_wWidth = wWidth;
	_ground_truth_ready = false;
	_predictive_ready = false;

	_renderer = NULL;
	init_renderer();
}

/*
Destructor
*/
PoseEval3DMain::~PoseEval3DMain()
{
	if (_renderer) delete _renderer;
}


/*
Load a ground truth model from an obj file. 
@path_and_file - relative or absolute path to an obj file.
Note that the model must be of type obj.
@return true - if the model could be loaded
*/
bool PoseEval3DMain::loadGroundTruthModel(string path_and_file)
{
	_ground_truth_model.create(path_and_file);
	_ground_truth_ready = true;

	return true;
}



/*
Load a ground truth model from an obj file. 
@path_and_file - relative or absolute path to an obj file.
Note that the model must be of type obj.
@return true - if the model could be loaded
*/
bool PoseEval3DMain::loadPredictiviveModel(string path_and_file)
{
	_predictive_model.create(path_and_file);
	_predictive_model.setTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.2f)) );
	_predictive_ready = true;

	return true;
}


/*
Set the pose prediction data. 
*/
bool PoseEval3DMain::setPosePredictionData(vector<PosePrediction> data)
{
	_data = data;
	return true;
}


/*
Evaluate all the poses in the pose prediction dataset. 
*/
bool PoseEval3DMain::evalAllPoses(void)
{
	for (auto a : _data) {

	}

	return true;
}


/*
Set the pose of the ground truth object.
@param t - the translation
@param q - the orientation
*/
bool PoseEval3DMain::setGroundTruthPose(Eigen::Vector3f t, Eigen::Quaternionf q)
{
	Eigen::Matrix3f R = q.normalized().toRotationMatrix();
	glm::mat4 Rt;

	for (int i = 0; i< 3; i++) {
		for (int j = 0; j < 3; j++) {
			Rt[j][i] = R(i,j);
		}
	}
	Rt[3][0] = t(0);
	Rt[3][1] = t(1);
	Rt[3][2] = t(2);
	Rt[3][3] = 1.0;


	_ground_truth_model.setTransformation(Rt);

	return true;
}

/*
Set the pose of the predictive object
@param t - the translation
@param q - the orientation
*/
bool PoseEval3DMain::setPredictivePose(Eigen::Vector3f t, Eigen::Quaternionf q)
{

	Eigen::Matrix3f R = q.normalized().toRotationMatrix();
	glm::mat4 Rt;

	for (int i = 0; i< 3; i++) {
		for (int j = 0; j < 3; j++) {
			Rt[j][i] = R(i,j);
		}
	}
	Rt[3][0] = t(0);
	Rt[3][1] = t(1);
	Rt[3][2] = t(2);
	Rt[3][3] = 1.0;

	_predictive_model.setTransformation(Rt);

	return true;
}



void PoseEval3DMain::calcPoseDelta(Eigen::Vector3f t0, Eigen::Quaternionf q0, Eigen::Vector3f t1, Eigen::Quaternionf q1)
{
	Eigen::Matrix3f R0 = q0.normalized().toRotationMatrix();
	Eigen::Matrix3f Rt0;

	for (int i = 0; i< 3; i++) {
		for (int j = 0; j < 3; j++) {
			Rt0(i,j) = R0(i,j);
		}
	}



	Eigen::Matrix3f R1 = q1.normalized().toRotationMatrix();
	Eigen::Matrix3f Rt1;

	for (int i = 0; i< 3; i++) {
		for (int j = 0; j < 3; j++) {
			Rt1(i,j) = R1(i,j);
		}
	}


	Eigen::Matrix3f dR = Rt1 * Rt0.inverse();
	Eigen::AngleAxisf newAngleAxis(dR);



	float dt = (t0 - t1).norm();

	std::cout << "delta t: " << dt << "\tdelta R: " << newAngleAxis.angle() << endl;

	
}




/*
This starts the main loop.
The function is  blocking and will not return until
the renderer closes
@return true - if the window closes. 
*/
bool PoseEval3DMain::run(void)
{
	_renderer->start();

	return true;
}


// init the GL renderer
bool PoseEval3DMain::init_renderer(void)
{

	_renderer = new GLRenderer();
	_renderer->create( _wWidth, _wHeight,  "3D Pose Evaluation");
	_renderer->addRenderFcn(std::bind(&PoseEval3DMain::render_fcn, this, _1, _2));

	return true;

}


// init the GL renderer
void  PoseEval3DMain::render_fcn(glm::mat4 pm, glm::mat4 vm) 
{

	if (_ground_truth_ready) {
		_ground_truth_model.draw( vm);
	}
	if (_predictive_ready) {
		_predictive_model.draw( vm);
	}



}