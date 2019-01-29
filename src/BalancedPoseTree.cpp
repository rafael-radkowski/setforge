#include "BalancedPoseTree.h"


/*
Constructor
@param - window_width, window_height - width and height of the opengl, glfw output image in pixel
@param - image_width, image_height - width and height of the image to be rendered and saved to a file.
*/
BalancedPoseTree::BalancedPoseTree(int window_width, int window_height, int image_width, int image_height ):
	ModelRenderer(window_width, window_height, image_width, image_height)
{
	_center = glm::vec3(0.f,0.f,0.f);
	_up = glm::vec3(0.0f, 1.0f, 0.0f);
	_N = 0;
	_N_current = 0;
	_camera_distance = 1.3;

	_node_id = 0;
	_curr_node_id = 0;
	_cuurent_level = 0;
	_tree_output_path = "tree";

	_subdivisions = 0;
}

BalancedPoseTree::~BalancedPoseTree()
{

}


/*
Set the number of subdivisions for the polyheder
The polyheder points define the camera view points. 
@param tree_depth - number of polyhedrons that should be applied. 0 will just generate a icosahedron
*/
void BalancedPoseTree::create(float camera_distance, int tree_depth)
{
	if (camera_distance >= 0.1) _camera_distance = camera_distance;
	if (tree_depth >= 0) _subdivisions = tree_depth;

	_tree_nodes.clear();
	_tree_points.clear();

	// create the polyhedron nodes
	for (int i = 0; i < _subdivisions + 1; i++)
	{
		// < points, vertices>
		std::pair< vector<glm::vec3>, vector<glm::ivec3> > poly;
		poly = arlab::PolyhedronGeometry::Create(i);

		_tree_points.push_back(poly.first);

	}

	// create the tree root node
	_root = new BPTNode(_curr_node_id, -1, -1, glm::vec3(0,0,0), -1);
	_curr_node_id++;

	_process_queue.push_back(_root); // add into the processing queue.
	_tree_nodes.push_back(_root);

	while (_process_queue.size() > 0) // all the nodes in the processing queue need children
	{
		BPTNode* node = _process_queue.front();

		int next_level = node->level + 1;
		if (next_level >= _tree_points.size()) {
			_process_queue.pop_front();
			continue; // end of levels
		}

		// points for the node's next level
		// create nodes for the first level
		PolyhedronPoints points = _tree_points[node->level+1];

		// get nearest neighbors
		vector<int> nn_idx =  find_nearest_neighbors(*node);

		// select 6 and create new nodes

		int max = 6;
		if (next_level == 0) {
			max = points.size(); // the root node is an empty node but it gets all 12 vertices of the icosahedron as childs.
		}
	
		for (int i = 0; i < max; i++)
		{
			glm::vec3 new_point = points[nn_idx[i]];

			BPTNode* new_node = new BPTNode(_curr_node_id, next_level, node->node_id, new_point, nn_idx[i]);
			_curr_node_id++;

			node->childs.push_back(new_node);
			_tree_nodes.push_back(new_node);
			_process_queue.push_back(new_node);
		}
		_process_queue.pop_front();
	}

	_N = _tree_nodes.size();

	cout << "Created N=" << _N << "nodes " << endl;

	
}


/*
Set the output path for your tree. 
@param path - string containging a relative or absolute tree path.
*/
void BalancedPoseTree::setOutputPath(string path, string filename)
{
	// note that this function checks and creates the path.
	ModelRenderer::setOutputPath(path);

	_tree_output_path = path;
}

/*
Draw the image sequence and save all images to a file
@return - false, if images still need to be rendered. 
			true, id all images were saved to a file. 
*/
bool BalancedPoseTree::draw_sequence(void)
{
	if (_N_current < _N) {

		if (_tree_nodes[_N_current]->node_id == 0) {
			_N_current++;
			return false;
		}// root node;

		glm::vec3 p = _tree_nodes[_N_current]->point;
		glm::vec3 n = _tree_nodes[_N_current]->point;
		glm::vec3 eye = n * glm::vec3(_camera_distance);

		// store the image index
		// _N_current - 1; -> the root node does not get an image. 
		_tree_nodes[_N_current]->image_index = _N_current - 1;

		// This prevents that the view matrix becomes NaN when eye and up vector align. 
		float ang = glm::dot(glm::normalize(eye), _up);
		float s = 1.0;
		if (ang < 0) s = -1.0;
		if (abs(ang) > 0.999)
			_up = glm::vec3(0.0, 0.0, -s);
		else
			_up = glm::vec3(0.0, 1.0, 0.0);


		glm::mat4 vm = glm::lookAt(eye, _center, _up );
		setCameraMatrix(vm);

		enable_writer(true);
		if(_verbose)
			cout << "[INFO] - Render image " << _N_current-1 << " for node " << _tree_nodes[_N_current]->node_id << " (level: " << _tree_nodes[_N_current]->level  << ") from pos: " << eye[0] << " : " << eye[1] << " : " << eye[2]<< endl;
		draw_and_save();
		_N_current++;
		
		if (_N_current == _N){
			if(_verbose)
				cout << "[INFO] - DONE - rendered " << _N_current << " sets." <<  endl;
			_writing_file = true;
		}
		return false;
	}
	else{
		if (_writing_file)
		{
			string tree_file = _tree_output_path;
			tree_file.append("/BPTData.csv");

			cout << "[INFO] - Start writing tree to file" << endl;
			BPTReaderWriter writer;
			writer.write(tree_file, _root);
			cout << "[INFO] - Done writing!" << endl;
			_writing_file = false;
		}
		glm::vec3 p = _tree_nodes[0]->point;
		glm::vec3 n = _tree_nodes[0]->point;
		glm::vec3 eye = n * glm::vec3(_camera_distance);
		glm::mat4 vm = glm::lookAt(eye, _center, _up);
		setCameraMatrix(vm);

		enable_writer(false);
		draw();
		return true;
	}
	


	return true;
}


/*
Find and add the nearest neighbors for each node.
*/
vector<int>  BalancedPoseTree::find_nearest_neighbors(BPTNode& node)
{
	// check for level
	if ( node.level + 1 >= _tree_points.size() ) return vector<int>(0);

	int next_level = node.level + 1;

	// initialize index vector 
	vector<int> idx(_tree_points[next_level].size());
	iota(idx.begin(), idx.end(), 0);

	// calculate distances
	vector<float> distances;
	for (auto v : _tree_points[next_level]){
		distances.push_back(glm::distance(v, node.point));
	}

	 // sort indexes based on comparing values in v
	std::sort(idx.begin(), idx.end(),
       [&distances](int i1, int i2) {return distances[i1] < distances[i2];});


	return idx;
}