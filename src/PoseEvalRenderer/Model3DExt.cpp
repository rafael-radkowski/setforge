#include "Model3DExt.h"

/*
Constructor
*/
Model3DExt::Model3DExt()
{
	_projectionMatrix = glm::perspective(1.2f, (float)800 / (float)600, 0.1f, 100.f);
	_viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0, 1.3f), glm::vec3(0.0f, 0.0f, 00.f), glm::vec3(0.0f, 1.0f, 0.0f));
	_modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	_light0.pos = glm::vec3(0.0f, -3.0f, 3.0f);
	_light0.dir = glm::vec3(0.0f, 0.0f, 0.0f);
	_light0.k1 = 0.1;
	_light0.intensity = 1.7;
	_light0.index = 0;
	
	_light1.pos = glm::vec3(0.0f, 3.0f, 3.0f);
	_light1.dir = glm::vec3(0.0f, 0.0f, 0.0f);
	_light1.k1 = 0.1;
	_light1.index = 1;



	_mat0.diffuse_mat = glm::vec3(1.0, 0.0, 0.0);
	_mat0.diffuse_int = 0.8;
	_mat0.ambient_mat = glm::vec3(1.0, 0.0, 0.0);
	_mat0.ambient_int = 0.2;
	_mat0.specular_mat = glm::vec3(1.0, 1.0, 1.0);
	_mat0.specular_int = 0.2;
	_mat0.specular_s = 6.0;

}

Model3DExt::~Model3DExt()
{

}


/*
Create the model instance.
@param path_and_file - string containg the relative or absolute path to the model.
@return - true, if model was successfully loaded.
*/
bool Model3DExt::create(string path_and_file)
{
	if (path_and_file.empty()) return false;


	// load shader
	int program = cs557::LoadAndCreateShaderProgram("./shaders/lit_scene_ext.vs", "./shaders/lit_scene_ext.fs");

	// create model
	_obj_model = new cs557::OBJModel();
	_obj_model->create(path_and_file, program);


	_light0.apply(program);
	//_light1.apply(program);
	_mat0.apply(program);


	// create a coordinate system
	_modelMatrixCoordSystem = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)); 
    _coordinateSystem.create(10);
}


/*
Set the transformation for this object
*/
bool  Model3DExt::setTransformation(glm::mat4 transform)
{
	_modelMatrix = transform;
	_modelMatrixCoordSystem = transform;
	return true;
}

/*
Draw the model 
*/
bool Model3DExt::draw(glm::mat4& vm)
{
	if (_obj_model == NULL) return false;
	_viewMatrix = vm;

	// update the light position so that it stays a camera front light. 
	glm::mat4 inv  = glm::inverse(_viewMatrix);
	_light0.pos = glm::vec3(inv[3][0], inv[3][1], inv[3][2]);
	_light0.apply(_obj_model->getProgram());

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
	_obj_model->draw(_projectionMatrix, _viewMatrix, _modelMatrix);
    _coordinateSystem.draw(_projectionMatrix, _viewMatrix, _modelMatrixCoordSystem);
}