/*
BPTypes.h

This file declares datatypes related to the balanced pose tree

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
#include <bitset> 

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


typedef struct _BPTNode
{
	int					node_id;
	vector<_BPTNode*>	childs;
	int					parent;

	// data
	int					level;
	int					image_index;
	int					point_id;
	glm::vec3			point;

	_BPTNode(int id_, int level_, int parent_, glm::vec3 p, int point_id_ )
	{
		node_id = id_;
		parent = -1;
		level = level_;
		parent = parent_;
		point = p;
		point_id = point_id_;

		image_index = -1;
	}

}BPTNode;


typedef std::vector<glm::vec3> PolyhedronPoints;



typedef struct _BPTDescriptor
{
	std::bitset<8>	orient;
	float			weight;

}BPTDescriptor;