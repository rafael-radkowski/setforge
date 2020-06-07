#include "UserViewRenderer.h"


/*
Constructor
@param - window_width, window_height - width and height of the opengl, glfw output image in pixel
@param - image_width, image_height - width and height of the image to be rendered and saved to a file.
*/
UserViewRenderer::UserViewRenderer(int window_width, int window_height, int image_width, int image_height) :
	ModelRenderer(window_width, window_height, image_width, image_height)
{
	_render_bbox = false;
}

UserViewRenderer::~UserViewRenderer()
{

}



/*
@param path_and_file - string containg the relative or absolute
						path to the model.
@return - true, if model was successfully loaded. 
*/
bool UserViewRenderer::create( string model_path_and_file )
{

	if(_verbose)
		cout << "[INFO] - Set to user interactive view."  << endl;


	cout << "[INFO] - Press 'w' to save a set of images to the selected folder."  << endl;
	cout << "[INFO] - Press 'b' to enable/disable the bounding box renderer."  << endl;

	bool ret = this->setModel(model_path_and_file);
	
	if (!ret) {
		cout << "[ERROR] - Could not load model " << model_path_and_file << "." << endl;
		return false;
	}

	return true;
}


/*
@param path_and_file - string containg the relative or absolute
						path to the model.
@return - true, if model was successfully loaded. 
*/
bool UserViewRenderer::create( string model_path_and_file, cs557::BRDFMaterial& material )
{
	if(_verbose)
		cout << "[INFO] - Set to user interactive view."  << endl;


	cout << "[INFO] - Press 'w' to save a set of images to the selected folder."  << endl;
	cout << "[INFO] - Press 'b' to enable/disable the bounding box renderer."  << endl;

	bool ret = this->setModel(model_path_and_file, material);
	
	if (!ret) {
		cout << "[ERROR] - Could not load model " << model_path_and_file << "." << endl;
		return false;
	}

	return true;
}


/*
Draw the model 
@param vm -  a glm 4x4 view matrix
*/
bool UserViewRenderer::draw_view(glm::mat4& vm)
{
	setCameraMatrix(vm);
	draw();

	return true;
}



void UserViewRenderer::keyboardCallback(int key, int action)
{
	if ( action == GLFW_PRESS)
	{
		if (key == 87)  { // w
			enable_writer(true);
			draw_and_save();
			enable_writer(false);
		}
		else if(key == 66) // b
		{
			if(_render_bbox) { 
				_render_bbox = false;
			}else
			{ 
				_render_bbox = true;
			}
			setDrawBBox(_render_bbox);
		}else if(key == 80) // p
		{
			projectBBox();
		}
	}

	//cout << key << endl;


}





