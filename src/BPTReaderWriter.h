#pragma once

// stl
#include <iostream>
#include <string>
#include <strstream>
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

// local
#include "BPTTypes.h"
#include "TimeUtils.h"
#include "FileUtils.h"

using namespace std;

class BPTReaderWriter
{
private:

	typedef struct _innode
	{
		int node_id;
		int level;
		int num_children;
		int img_index;
	
		_innode(int n, int l, int num, int idx)
		{
			node_id = n;
			level = l;
			num_children = num;
			img_index = idx;
		}

	}InNode;

	typedef struct _inlink
	{
		int parent_id;
		int child_id;

		_inlink(int p, int c) {
			parent_id = p;
			child_id = c;
		}
	}InLink;

public:
	BPTReaderWriter();
	~BPTReaderWriter();

	/*
	Write the BPT to a file
	@param path_and_file - string with the relative or absolute file. 
	@param root - the root node of the tree. 
	@return - true if file was successfully generated. 
	*/
	bool write(string path_and_file, BPTNode* root);


	/*
	Read and create the nodes for the Balanced Pose Tree
	@param path_and_file - string with the relative or absolute file. 
	@param root - the root node of the tree. 
	@param node_repository - reference to the node repository. 
	*/
	bool read(string path_and_file, BPTNode** root, vector<BPTNode*>* node_repository);

private:

	vector<string>  BPTReaderWriter::split(string str, char delimiter);

	/*
	Write the header file
	*/
	bool writeHeader(string path_and_file, BPTNode* root);

	/*
	Write the nodes into a file
	*/
	bool writeNode(string path_and_file, BPTNode* node, int& counter);

	/*
	Create a tree using the loaded data. 
	*/
	bool createTreeFromFile(vector<InNode>* n, vector<InLink>* l, BPTNode** root, vector<BPTNode*>* node_repository);

	// members

};