#include "ModelRenderer.h"



ModelRenderer::ModelRenderer(int window_width, int window_height, int image_width, int image_height):
	_width(window_width), _height(window_height), _image_width(image_width),  _image_height(image_height)
{
	_obj_model = NULL;
	_fboHidden = -1;
	_color_texture_idx = -1;
	_depth_texture_idx = -1;

	_fboHiddenNormals = -1;
	_normal_texture_idx = -1;
	_normal_depth_texture_idx = -1;

	_output_file_id = 0;
	_save = false;
	_output_file_path = "out";
	_output_file_name = "model";
	_writer_enabled = true;

	_with_roi = true; 
	_with_mask = true;
	_with_rand_col = true;
	_with_bbox = false;
	_with_bbox_projection = true;
	_verbose = false;

	_projectionMatrix = glm::perspective(1.2f, (float)800 / (float)600, 0.1f, 100.f);
	_projectionMatrix = glm::perspective( glm::radians(40.0f), (float)480 / (float)480, 0.1f, 100.f);
	_viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0, 13.3f), glm::vec3(0.0f, 0.0f, 00.f), glm::vec3(0.0f, 1.0f, 0.0f));
	_modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));  //glm::rotate(3.1415f, glm::vec3(0.0f, 1.0f, 0.0f)) *  glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	_light0.pos = glm::vec3(0.0f, -3.0f, 3.0f);
	_light0.dir = glm::vec3(0.0f, 0.0f, 0.0f);
	_light0.k1 = 0.1;
	_light0.intensity = 1.5;
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
	_mat0.specular_s = 4.0;
	
#ifndef _DEVELOP
	_light0.with_error_check = false;
	_light1.with_error_check = false;
	_mat0.with_error_check = false;
#endif

	_data_rgb = (unsigned char*)malloc(_image_width * _image_height * 3 * sizeof(int));
	_data_depth = (unsigned char*)malloc(_image_width * _image_height * 1 * sizeof(float));
	_data_normals = (unsigned char*)malloc(_image_width * _image_height * 3 * sizeof(float));


	_writer = new ImageWriter();

	// init the point projectoin. 
	_projection = new PointProjection(_image_width, _image_height);
}


ModelRenderer::~ModelRenderer()
{
	free(_data_rgb);
	free(_data_depth);
	free(_data_normals);

	delete _writer;
	delete _projection;
}


/*
Set the model to render
@param path_and_file - string containg the relative or absolute
						path to the model.
@return - true, if model was successfully loaded.
*/
bool ModelRenderer::setModel(string path_and_file)
{
	if (path_and_file.empty()) return false;

//#define _DEVELOP
#ifdef _DEVELOP
	// load shader
	int program = cs557::LoadAndCreateShaderProgram("./shaders/image_renderer.vs", "./shaders/image_renderer.fs");
#else
	int program = cs557::CreateShaderProgram(glslshader::image_renderer_vs, glslshader::image_renderer_fs);
#endif
	// create model
	_obj_model = new cs557::OBJModel();
	_obj_model->create(path_and_file, program);
	_obj_model->setTextureParam(cs557::TextureMode::REPLACE);

	_light0.apply(program);
	//_light1.apply(program);
	_mat0.apply(program);

	//-----------------------
	// create a secodn object to render only normal vectors
	// load shader
#ifdef _DEVELOP
	int program_normals = cs557::LoadAndCreateShaderProgram("./shaders/normal_renderer.vs", "./shaders/normal_renderer.fs");
#else
	int program_normals = cs557::CreateShaderProgram(glslshader::normal_renderer_vs, glslshader::normal_renderer_fs);
#endif
	_obj_model_normals = new cs557::OBJModel();
	_obj_model_normals->create(path_and_file, program_normals);

	_light0.apply(program_normals);
	//_light1.apply(program_normals);
	

	CreatePrerendererScene();
	CreateHelperContent();

}


/*
Load the model that needs to be rendered. This model uses brdf material. 
@param path_and_file - string containg the relative or absolute
						path to the model.
@param brdf_material - brdf material object. 
@return - true, if model was successfully loaded. 
*/
bool ModelRenderer::setModel(string path_and_file, cs557::BRDFMaterial& brdf_material)
{
	if (path_and_file.empty()) return false;

	// create model
	_obj_model = new cs557::ModelBRDF();
	static_cast<cs557::ModelBRDF*>(_obj_model)->create(path_and_file, brdf_material);
	_obj_model->setTextureParam(cs557::TextureMode::REPLACE);

	_light0.apply(_obj_model->getProgram());
	//_light1.apply(program);


	//-----------------------
	// create a secodn object to render only normal vectors
	// load shader
#ifdef _DEVELOP
	int program_normals = cs557::LoadAndCreateShaderProgram("./shaders/normal_renderer.vs", "./shaders/normal_renderer.fs");
#else
	int program_normals = cs557::CreateShaderProgram(glslshader::normal_renderer_vs, glslshader::normal_renderer_fs);
#endif
	_obj_model_normals = new cs557::OBJModel();
	_obj_model_normals->create(path_and_file, program_normals);

	_light0.apply(program_normals);
	//_light1.apply(program_normals);
	

	CreatePrerendererScene();
	CreateHelperContent();
}


/*
Draw the scene into an fbo to get textures
*/
bool ModelRenderer::drawFBO(void)
{
	if (_obj_model == NULL) return false;

	glBindFramebuffer(GL_FRAMEBUFFER, _fboHidden);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// Set up our green background color
	static GLfloat clear_color[] = { 0.0f, 0.0f, 0.0f, 0.0f };//{0.6f, 0.7f, 1.0f, 1.0f};
	static GLfloat clear_depth[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	// Clear the entire buffer with our green color.
	glClearBufferfv(GL_COLOR, 0, clear_color);
	glClearBufferfv(GL_DEPTH, 0, clear_depth);

	// set the viewport. It must match the texture size.
	glViewport(0, 0,  _image_width, _image_height);

	_obj_model->draw(_projectionMatrix, _viewMatrix, _modelMatrix);

	//-------------------------------------------------------------------------------------
	// get the data back 

	// read back the pixels
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glReadPixels(0, 0, _image_width, _image_height, GL_BGR, GL_UNSIGNED_BYTE, _data_rgb);

	glReadBuffer(GL_DEPTH_ATTACHMENT);
	glReadPixels(0, 0, _image_width, _image_height, GL_DEPTH_COMPONENT, GL_FLOAT, _data_depth);

	// switch back to the regular output buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// set the viewport to window size
	glViewport(0, 0, _width, _height);

	// rgb image
	cv::Mat image(_image_height, _image_width, CV_8UC3, _data_rgb);
	cv::Mat dst, output_rgb;
	cv::flip(image, dst, 0);
	

	// depth image
	cv::Mat imaged(_image_height, _image_width, CV_32FC1, _data_depth);
	cv::Mat dst_depth, output_depth, normalized;
	cv::flip(imaged, dst_depth, 0);



	//-------------------------------------------------------------------------------------
	// Draw normals
	glBindFramebuffer(GL_FRAMEBUFFER, _fboHiddenNormals);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	static GLfloat clear_normals[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Clear the entire buffer with our green color.
	glClearBufferfv(GL_COLOR, 0, clear_normals);
	glClearBufferfv(GL_DEPTH, 0, clear_depth);

	// set the viewport. It must match the texture size.
	glViewport(0, 0, _image_width, _image_height);

	_obj_model_normals->draw(_projectionMatrix, _viewMatrix, _modelMatrix);


	//-------------------------------------------------------------------------------------
	// get the data back 

	// read back the pixels
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glReadPixels(0, 0, _image_width, _image_height, GL_BGR, GL_FLOAT, _data_normals);

	cv::Mat image_normals(_image_height, _image_width, CV_32FC3, _data_normals);
	cv::Mat dst_norm, output_norm;
	cv::flip(image_normals, dst_norm, 0);


	//-------------------------------------------------------------------------------------
	// region of interest extraction
	cv::Rect2f roi;
	if (_with_roi) {
		RoIDetect::Extract(dst, roi);
	}

	//-------------------------------------------------------------------------------------
	// Extract an image mask
	cv::Mat mask;
	if (_with_mask) {
		ImageMask::Extract(dst, mask);
	}
	

	//-------------------------------------------------------------------------------------
	// Project the bounding box corner points and the center of the bounding box. 
	if (_with_bbox_projection) {
		projectBBoxPoints();
	}


	// switch back to the regular output buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// set the viewport to window size
	glViewport(0, 0, _width, _height);


	if (_verbose) {

		// to normalize the depth image
		//cv::normalize(dst_norm, output_norm, 0, 255, cv::NORM_MINMAX, CV_8UC3);
		cv::resize(dst, output_rgb, cv::Size(512, 512));
		cv::resize(dst_depth, output_depth, cv::Size(512, 512));
		cv::resize(dst_norm, output_norm, cv::Size(512, 512));

		cv::imshow("RGB image (3 x uchar)", output_rgb);
		cv::imshow("Depth image (float)", output_depth);
		cv::imshow("Normal image (float)", output_norm);
		

		if (_verbose && _with_roi) 
			RoIDetect::RenderRoI(dst, roi);

		if (_verbose && _with_bbox_projection) {
			_projection->showProjection();
		}

		cv::waitKey(1);
	}

	

	if (_save && _writer_enabled && _writer){

		ImageWriter::IWData odata;
		odata.index = _output_file_id;
		odata.rgb = &dst;
		odata.normals = &dst_norm;
		odata.depth = &dst_depth;
		if (_with_mask) {
			odata.mask = &mask;
		}
		// this view matrix describes the object's pose in camera coordinates since the object is at 0,0,0
		odata.pose = _viewMatrix;
		odata.roi = roi;
		odata.control_points = _projected_points;

		_writer->write(odata);

		// this writes model information. Currently, the only info is the bounding box corner points. 
		if(_output_file_id == 0){
			std::vector<glm::vec3> corners =  _bbox->getCorners();
			_writer->writeModelFile(corners);
		}

		//_writer->write(_output_file_id, dst, dst_norm, dst_depth, glm::inverse(_viewMatrix));
		_output_file_id++;
	}

	return true;
}

/*
Draw the current object in a window
*/
bool ModelRenderer::draw(void)
{
	if (_obj_model == NULL) return false;

	

	drawFBO();


	_obj_model->draw(_projectionMatrix, _viewMatrix, _modelMatrix);
    _coordinateSystem.draw(_projectionMatrix, _viewMatrix, _modelMatrixCoordSystem);

	if(_with_bbox)
		_bbox->draw(_projectionMatrix, _viewMatrix, _modelMatrix);

	glUseProgram(_display.getProgram());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _depth_texture_idx);
	_display.draw(_projectionMatrix, _viewMatrix, _display_m);
}


/*
	Draw the current object into a window and into the fbo.
	Save the fbo to file
	*/
bool  ModelRenderer::draw_and_save(void)
{
	if (_obj_model == NULL) return false;
	_save = true;


	// apply random color to randomize the color data. 
	if (_with_rand_col) {
		applyRandomColor();
	}


	draw();
}


/*
	Create a scene for the prerenderer
	*/
void  ModelRenderer::CreatePrerendererScene(void)
{

	// This function is part of RenderToTexture.h
	CreateRenderToTexture(_image_width, _image_height, _fboHidden, _color_texture_idx, _depth_texture_idx);

	// Reset to the regular buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	CreateRenderToTexture32Bit(_image_width, _image_height, _fboHiddenNormals, _normal_texture_idx, _normal_depth_texture_idx);
	//cout << _fboHiddenNormals << " : " << _normal_texture_idx << " : " << _normal_depth_texture_idx << endl;
	
	// Reset to the regular buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glUseProgram(0);
}


/*
Creates some helper displays
*/
void ModelRenderer::CreateHelperContent(void)
{
	// Load the shader program
#ifdef _DEVELOP
	int shader = cs557::LoadAndCreateShaderProgram("./shaders/display.vs", "./shaders/display.fs");
#else
	int shader = cs557::CreateShaderProgram(glslshader::display_renderer_vs, glslshader::display_renderer_fs);
#endif
	// create a plane
	_display.create(0.56, 0.45, shader);
	_display_m = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	_display_p = glm::perspective(1.57f, (float)400 / (float)400, 0.01f, 10.f);
	_display_v = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glUseProgram(shader); 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _color_texture_idx);
	int texture_location = glGetUniformLocation(shader, "tex");
	glUniform1i(texture_location, 0);
	glUniform1f(glGetUniformLocation(shader, "display_scale"), 1.0f);

	glUseProgram(0);


	// create a coordinate system
	_modelMatrixCoordSystem = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)); 
    _coordinateSystem.create(25);

	// create the bounding box
	_bbox = new cs557::BBox();
	_bbox->create(glm::vec3(0.0,0.0,0.0), _obj_model->getBoundingBox());
}


/*
Set the intrinsic parameters
@param  fx, fy, Field of view in pixels into the x and y direction.
@parm	px, py, the camera principle point
*/
void ModelRenderer::setIntrinsic(float fx, float fy, float px, float py)
{
	_projectionMatrix = glm::perspective(fx/fy, (float)fx / (float)fy, 0.1f, 10.f);
	
}

/*
Set the view matrix.
@param viewmatrix - a 4x4 view matrix.
*/
void ModelRenderer::setCameraMatrix(glm::mat4 viewmatrix)
{
	_viewMatrix = viewmatrix;

	// update the light position so that it stays a camera front light. 
	glm::mat4 inv  = glm::inverse(viewmatrix);
	_light0.pos = glm::vec3(inv[3][0], inv[3][1], inv[3][2]);
	_light0.apply(_obj_model->getProgram());
	_light0.apply(_obj_model_normals->getProgram());
}

/*
Set an output path for the images
@param path - relative or absolute output path
@param name - name template for the output images.
*/
void ModelRenderer::setOutputPath(string path, string name)
{
	_output_file_path = path;
	_output_file_name = name;

	if(_writer)
		_writer->setPathAndImageName(path, name);
}


/*
Disable and enable the file writer
*/
bool  ModelRenderer::enable_writer(bool enable)
{
	_writer_enabled = enable;

	return true;
}


/*
Enable or disable random color rendering. 
@param enable - true enables random colors. 
*/
void ModelRenderer::setRandomColors(bool enable )
{
	_with_rand_col = enable;
}


/*
Enable or disable the bounding box renderer.
@param draw - true enables bounding box rendering. 
*/
void ModelRenderer::setDrawBBox(bool draw)
{
	_with_bbox = draw;
}


/*
Project the boundinx box corner points and the bounding box centroid. 
*/
bool ModelRenderer::projectBBoxPoints()
{
	assert(_bbox != NULL);
	std::vector<glm::vec3> corners =  _bbox->getCorners();

	// note that this function assumes that the model is centered at the bounding box center. 
	corners.push_back(glm::vec3(0.0,0.0,0.0)); // the center point. 

	_projection->setProjectionMatrix(_projectionMatrix);
	_projection->setViewMatrix(_viewMatrix);
	_projection->projectPoints(corners, _modelMatrix, _projected_points);

	return true;
}


/*
Set the min / max values for hue and saturation
@param hue_min - a min. hue value in the range from 0 to 360 degree.
@param hue_max - a max. hue value in the range from 0 to 360 degree.
@param sat_min - a min. saturation value in the range from 0 to 1.
@param sat_max - a max. saturation value in the range from 0 to 1.
*/
void ModelRenderer::setRandomColorsParams(float hue_min, float hue_max, float sat_min, float sat_max, float v_min, float v_max, float with_v)
{
	_rand_col.setHueRange(hue_min/360.0, hue_max/360.0);
	_rand_col.setSaturationRange(sat_min, sat_max);
	_rand_col.setBrightnessRange(v_min, v_max);
	_rand_col.setRandomizeBrightness(with_v);
}



/*
Applies random color to the model if 
the setting is set to random color. 
*/
void ModelRenderer::applyRandomColor(void)
{
	
	// fetch a random color component.
	cs557::Material mat;
	std::vector<float> rgb = _rand_col.getRGB();


	mat.diffuse_mat.r = rgb[0]/255.0;
	mat.diffuse_mat.g = rgb[1]/255.0;
	mat.diffuse_mat.b = rgb[2]/255.0;

	mat.ambient_mat.r = rgb[0]/255.0;
	mat.ambient_mat.g = rgb[1]/255.0;
	mat.ambient_mat.b = rgb[2]/255.0;

	mat.specular_mat.r = 1.0;
	mat.specular_mat.g = 1.0;
	mat.specular_mat.b = 1.0;

	mat.specular_int = 0.1;
	mat.ambient_int = 0.2;
	mat.diffuse_int = 0.8;
	mat.specular_s = 10.0;

//	std::cout << "[INFO] - Random color r " << rgb[0] << "\tg " << rgb[1] << "\tv " << rgb[2]  << std::endl;
	
	if(_obj_model!= NULL)
		_obj_model->setMaterial(mat);
}


/*
Enable or disable the bounding box projection function. 
It is enabled by default. 
*/
void ModelRenderer::withBBoxProjection(bool enable)
{
	_with_bbox_projection = enable;
}


/*
Project the bounding box manually. 
This is a debug function to verify that the bounding box works. 
*/
bool ModelRenderer::projectBBox(void)
{
	projectBBoxPoints();
	return true;
}