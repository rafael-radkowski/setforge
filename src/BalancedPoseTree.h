/*
class BalancedPoseTree

This class generates a B-tree data structure for a balanced pose tree. 
It creates all nodes, renders image data sets per node, and writes the tree content into a file.

Rafael Radkowski
Iowa State University
rafael@iastate.edu
Jan 2019
MIT license
----------------------------------------------------
last edited:

*/
#pragma once

// stl
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <numeric>

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
#include "BPTReaderWriter.h"
#include "BPTTypes.h"

using namespace std;

class BalancedPoseTree : public ModelRenderer
{

public:

	/*
	Constructor
	@param - window_width, window_height - width and height of the opengl, glfw output image in pixel
	@param - image_width, image_height - width and height of the image to be rendered and saved to a file.
	*/
	BalancedPoseTree(int window_width = 1280, int window_height = 1024, int image_width = 2560, int image_height = 2048);
	~BalancedPoseTree();


	/*
	Set the number of subdivisions for the polyheder
	The polyheder points define the camera view points. 
	@param tree_depth - number of polyhedrons that should be applied. 0 will just generate a icosahedron
	*/
	void create( float camera_distance,  int tree_depth);


	/*
	Set the output path for your tree. 
	@param path - string containging a relative or absolute tree path.
	*/
	void setOutputPath(string path, string filename = "model");

	/*
	Draw the image sequence and save all images to a file
	@return - false, if images still need to be rendered. 
			  true, id all images were saved to a file. 
	*/
	bool draw_sequence(void);


private:

	/*
	Find and add the nearest neighbors for each node.
	*/
	vector<int> find_nearest_neighbors(BPTNode& node);


	//--------------------------------------------------------------
	// members

	glm::vec3			_center;
	glm::vec3			_up;

	float				_camera_distance; 

	int					_N; // max number of points.
	int					_N_current; // current point to render

	int					_node_id;
	int					_curr_node_id; // helper to maintain a node id. 
	int					_cuurent_level;
	int					_subdivisions; // max levels

	string				_tree_output_path; 
	bool					_writing_file;

	BPTNode*				_root;
	vector<PolyhedronPoints>		_tree_points;
	std::vector<BPTNode*>		_tree_nodes;

	std::list<BPTNode*>		_process_queue; // nodes that still need childs

};

