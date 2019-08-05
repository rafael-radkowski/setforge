#pragma once
/*
Class TextureLoader

This class loades textures from image files using opencv, changes their size to a power of 2 and stores them 
in global namespace.  The class usess OpenCV to load the textures from files. Thus, it can load all images that OpenCV
can load. 
It replaces the previously used bmp file loader. 

Rafael Radkowski
Iowa State University
rafael@iastate.edu
+1 (515) 294 7044
July 4th, 2019
MIT License
---------------------------------------------------------------
Last edits:

July 13, 2019, RR 
- Flipped the texture to align the blender/opencv coordinate system with the GL coordinate system. 


*/

// stl include
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <experimental/filesystem>

// GLEW include
#include <GL/glew.h>

// opencv
#include <opencv2/opencv.hpp>


using namespace std;


class TextureLoader
{
public:

	/*!
	Load a texture from a file. 
	@param path_and_name - string containing the path and the filename of the texture. 
	@param texture - pointer to store the memory address of the textures. The texture is kept in global namespace. 
					The function returns its address. 
	@param width - location to return the width of the image in pixels (integer). 
	@param height - location to return the height of the image in pixels (interger). 
	@param channels - location to return the number of channels of the image as integer.
	@return - true if the image was successfully loaded, false otherwise. 
	*/
	static bool Load(string path_and_name, unsigned char** texture, int* width, int* height, int* channels);

private:

	/*!
	Return the next power of two of for the value number.
	The function is used internally to resize the texture to a 2^n size. 
	@param number - an integer number.
	@return - the closest 2^n number for the given integer. 
	*/
	static int FindNextPower2(int number);


};