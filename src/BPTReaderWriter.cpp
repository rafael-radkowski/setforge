#include "BPTReaderWriter.h"


BPTReaderWriter::BPTReaderWriter()
{

}

BPTReaderWriter::~BPTReaderWriter()
{

}

/*
Write the BPT to a file
@param path_and_file - string with the relative or absolute file. 
@return - true if file was successfully generated. 
*/
bool BPTReaderWriter::write(string path_and_file, BPTNode* root)
{
	if (root == NULL) {
		cout << "[ERROR] - BPTReaderWriter: no root set." << endl;
		return false;
	}

	if (path_and_file.length() == 0) {
		cout << "[ERROR] - BPTReaderWriter: no path and file set." << endl;
		return false;
	}

	int index = path_and_file.find_last_of("/");
	if (index != -1)
	{
		string path = path_and_file.substr(0, index);
		if(!FileUtils::Exists(path)){
			FileUtils::CreateDirectories(path);
		}
	}

	writeHeader(path_and_file, root);

	int counter = 0;
	// the function writes all nodes recursively into a file. 
	writeNode(path_and_file, root, counter);

	cout << "[INFO] - wrote " << counter << " nodes into " << path_and_file << endl;

	return true;
}


/*
Write the header file
*/
bool BPTReaderWriter::writeHeader(string path_and_file, BPTNode* root)
{
	if (root == NULL) {
		cout << "[ERROR] - BPTReaderWriter: no root set." << endl;
		return false;
	}

	if (path_and_file.length() == 0) {
		cout << "[ERROR] - BPTReaderWriter: no path and file set." << endl;
		return false;
	}

	std::ofstream of(path_and_file, std::ofstream::out);

	if(!of.is_open()){
		cout << "[ERROR] - BPTReaderWriter: could not open file " << path_and_file << endl;
		return false;
	}

	of << "BPTDATA\n";
	of << "Automatically generated file\n";
	of << TimeUtils::GetCurrentDateTime() << "\n";
	of << "Header\n";
	of << "NODE\tnode_id\tnode_level\tnum_childs\timage_index\n";
	of << "LINK\tnode_id_parent\tnode_id_child\n";
	of << "\nDATA\n";

	of.close();
}


/*
Write the nodes into a file
*/
bool BPTReaderWriter::writeNode(string path_and_file, BPTNode* node, int& counter)
{
	if (node == NULL) return false;

	std::ofstream of(path_and_file, std::ofstream::out | std::ofstream::app);

	if(!of.is_open()){
		cout << "[ERROR] - BPTReaderWriter: could not open file " << path_and_file << endl;
		return false;
	}

	of << "NODE\t" << node->node_id << "\t" << node->level << "\t" << node->childs.size() << "\t" << node->image_index << "\t"  <<"\n";

	counter++;

	for (auto c : node->childs) {
		of << "LINK\t" << node->node_id << "\t" << c->node_id << "\n";
	}

	of.close();

	// depth first 
	for (auto c : node->childs) {
		writeNode(path_and_file, c, counter);
	}

	return true;
}



/*
Read and create the nodes for the Balanced Pose Tree
@param path_and_file - string with the relative or absolute file. 
@param root - the root node of the tree. 
@param node_repository - reference to the node repository. 
*/
bool  BPTReaderWriter::read(string path_and_file, BPTNode** root, vector<BPTNode*>* node_repository)
{
	if (!FileUtils::Exists(path_and_file)) {
		cout << "[ERROR] - BPTReaderWriter: could not find file " << path_and_file << endl;
		return false;
	}

	std::ifstream in(path_and_file, std::ifstream::in);

	if (!in.is_open()) {
		cout << "[ERROR] - BPTReaderWriter: could not open file " << path_and_file << endl;
		return false;
	}

	bool found_data = false;

	vector<InNode>	nodes;
	vector<InLink>	links;

	while (in)
	{
		string str;
		std::getline(in, str);

		if (found_data) {
			vector<string> result = split(str, '\t');
			if (result.size() == 0) continue;
			if (result[0].compare("NODE") == 0)
			{
				InNode n(atoi(result[1].c_str()), atoi(result[2].c_str()), atoi(result[3].c_str()), atoi(result[4].c_str()));
				nodes.push_back(n);
			}
			else if (result[0].compare("LINK") == 0)
			{
				InLink l(atoi(result[1].c_str()), atoi(result[2].c_str()));
				links.push_back(l);
			}
			//cout << str << endl;
		}
		else
		{
			//cout << str << endl;
		}

		if (str.compare("DATA") == 0) found_data = true;

	}

	in.close();

	cout << "[INFO] - Found " << nodes.size() << " nodes and " << links.size() << " links." << endl;



	createTreeFromFile(&nodes, &links, root, node_repository);


	

	return true;
}



vector<string>  BPTReaderWriter::split(string str, char delimiter) {
	vector<string> strvec;
	stringstream ss(str); // Turn the string into a stream.
	string tok;
 
	while(getline(ss, tok, delimiter)) {
		strvec.push_back(tok);
	}
 
	return strvec;
}

/*
Create a tree using the loaded data. 
*/
bool BPTReaderWriter::createTreeFromFile(vector<InNode>* n, vector<InLink>* l, BPTNode** root, vector<BPTNode*>* node_repository)
{
	node_repository->resize(n->size());

	for (int i = 0; i < n->size(); i++) {
		InNode tnode = (*n)[i];
		BPTNode* node = new BPTNode(tnode.node_id, tnode.level, -1, glm::vec3(0, 0, 0), tnode.img_index);
		node->image_index = tnode.img_index;
		(*node_repository)[((*n)[i].node_id)] = node;

		if (node->node_id == 0) {
			(*root) = node;
		}
	}


	for (int i = 0; i < l->size(); i++) {
		InLink tlink = (*l)[i];

		(*node_repository)[(int)tlink.parent_id]->childs.push_back((*node_repository)[(int)tlink.child_id]);

	}


	return true;

}