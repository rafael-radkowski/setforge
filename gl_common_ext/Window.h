#pragma once
/*


Rafael Radkowski
Iowa State University
rafael@iastate.edu
MIT License

------------------------------------------------------
Edits:

06/14/19, RR:
-	Added a new function api for initWindow, with parameters for the window width and height.
	It overwrites the previous version.

*/

// stl include
#include <iostream>
#include <string>
#include <vector>
#include <functional>

// GLEW include
#include <GL/glew.h>

// GLM include files
#define GLM_FORCE_INLINE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

// glfw includes
#include <GLFW/glfw3.h>


// locals
#include "TrackballControls.h"		// a trackball controller
#include "CameraControls.h"

using namespace std;





namespace cs557
{

	/*!
	This function initializes the GLFW window
	@param window_name - string containing the window name
	@return - a pointer storing a handle to the window
	*/
	GLFWwindow* initWindow(string window_name = "OpenGL Window for 557");


	/*!
	This function initializes the GLFW window
	@param wnd_height - height of the window in pixel
	@param wnd_width - width of the window in pixel
	@param window_name - string containing the window name
	@return - a pointer storing a handle to the window
	*/
	GLFWwindow* initWindow(int wnd_height, int wnd_width, string window_name = "OpenGL Window for 557");


	/*
	Deprecated: Do not use this. Use GetCamera() instead
	Return a reference of the trackball.
    LEGACY FUNCTION - TO BE BACKWARD COMPATIBLE UNTIL I CHANGE THIS
	*/
	ControlsBase& GetTrackball(void);

	/*
	Returns the camera controller. 
	*/
	ControlsBase& GetCamera(void);
	

	/*
	Set the initial view matrix for the camera controler. 
	@param vm - 4x4 view matrix. 
	*/
	void InitControlsViewMatrix(glm::mat4 vm);


	/*
	Add a keyboard callback function of type
	void name(int key, int action).
	The keyboard function will call it. 
	*/
	void AddKeyboardCallbackPtr(std::function<void(int, int)> fc);

}