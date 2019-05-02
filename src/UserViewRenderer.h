#ifndef __USERVIEW_RENDERER__
#define __USERVIEW_RENDERER__
/*
class UserViewRenderer

This class renders images interactively. A user can set the camera location and press the 'w' key
to store an image set into the selected folder. 
The renderer draws the rgb image, normal map, and depth map (linear) into a fbo and writes them into a file.


Features:
- Renders the 3d model as RGB image and writes it into a file CV_8UC3)
- Renders the normal map and writes it into a file (CV_32FC1, float).
- Renders the depth map (linear) and writes the values into a file  (CV_32FC1, float).
- Allows a user to manually save the rendered set into a file. 

Usage:

model_renderer = new UserViewRenderer(opt.windows_width, opt.window_height, opt.image_width, opt.image_height);
model_renderer->setVerbose(opt.verbose); 
model_renderer->setOutputPath(opt.output_path);
model_renderer->create(opt.model_path_and_file);


Rafael Radkowski
Iowa State University
rafael@iastate.edu
+1 (515) 294 7044
Jan 2019
All copyrights reserved

------------------------------------------------------------------
last edited:

*/


// stl
#include <iostream>
#include <vector>
#include <string>

// opencv
#include <opencv2/opencv.hpp>


// GLM include files
#define GLM_FORCE_INLINE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>  // transformation
#include <glm/gtx/quaternion.hpp> // quaternions

#include "PolyhedronGeometry.h" // for the Polyhedron geometry
#include "ModelRenderer.h"

class UserViewRenderer : public ModelRenderer
{
public:

	/*
	Constructor
	@param - window_width, window_height - width and height of the opengl, glfw output image in pixel
	@param - image_width, image_height - width and height of the image to be rendered and saved to a file.
	*/
	UserViewRenderer(int window_width = 1280, int window_height = 1024, int image_width = 2560, int image_height = 2048);
	~UserViewRenderer();

	/*
	@param path_and_file - string containg the relative or absolute
							path to the model.
	@return - true, if model was successfully loaded. 
	*/
	bool create( string model_path_and_file );

	/*
	Draw the model 
	@param vm -  a glm 4x4 view matrix
	*/
	bool draw_view(glm::mat4& vm);

	/*
	Keyboard callback. 
	Used to write an image set when the user presses the 'w' key
	*/
	void keyboardCallback(int key, int action);


private:




};




#endif