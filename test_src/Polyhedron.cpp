#include "Polyhedron.h"



namespace polyhedron_types
{

	static const string vs_string_410 =
		"#version 410 core                                                 \n"
		"                                                                   \n"
		"uniform mat4 projectionMatrix;                                    \n"
		"uniform mat4 viewMatrix;                                           \n"
		"uniform mat4 modelMatrix;                                          \n"
		"in vec3 in_Position;                                               \n"
		"in vec2 in_Texture;                                                 \n"
		"in vec3 in_Normal;                                                  \n"
		"out vec3 pass_Normal;                                              \n"
		"out vec2 pass_Texture;												\n"
		"                                                                  \n"
		"void main(void)                                                   \n"
		"{                                                                 \n"
		"    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(in_Position, 1.0);  \n"
		"    pass_Normal = in_Normal;                                         \n"
		"	 pass_Texture = in_Texture;										\n"
		"}                                                                 \n";

	// Fragment shader source code. This determines the colors in the fragment generated in the shader pipeline. In this case, it colors the inside of our triangle specified by our vertex shader.
	static const string fs_string_410 =
		"#version 410 core                                                 \n"
		"                                                                  \n"
		"in vec3 pass_Normal;                                                 \n"
		"in vec2 pass_Texture;												\n"
		"out vec4 color;                                                    \n"
		"void main(void)                                                   \n"
		"{                                                                 \n"
		"    color = vec4(pass_Texture, 0.0, 1.0);                               \n"
		"}                                                                 \n";



};



using namespace polyhedron_types;
using namespace arlab;


/*
Create an Icosahedron or Polyhedron, depending on the num of subdivisions. 
@param level - the number of subdivisons. 0 creates an Icosahedron;
*/
void Polyhedron::create(int level, int shader_program)
{
	// This overwrite the default shader program
	if(shader_program != -1){
		program = shader_program;
	}
	else
	{
		program = -1;
	}

	
	//createIcosahedron();
	std::pair< vector<glm::vec3>, vector<glm::ivec3> > final_mesh;
	final_mesh  = PolyhedronGeometry::Create(level);


	cout << "Created " << final_mesh.first.size() << " vertices" << endl;


	// create a shader program only if the progrm was not overwritten. 
	if(program == -1)
		program = cs557::CreateShaderProgram(vs_string_410, fs_string_410);

	glUseProgram(program);

	int pos_location = glGetAttribLocation(program, "in_Position");
	int normal_location = glGetAttribLocation(program, "in_Normal");
	int texture_location = glGetAttribLocation(program, "in_Texture");

	_I = 0;


	_I = final_mesh.second.size() * 3;
	cs557::CreateVertexObjectsIndexed33(vaoID, vboID, iboID, &final_mesh.first[0].x, &final_mesh.first[0].x, final_mesh.first.size(), (int*)&final_mesh.second[0],_I ,
										pos_location, normal_location);
	


	// Find the id's of the related variable name in your shader code. 
	projMatrixLocation = glGetUniformLocation(program, "projectionMatrix"); // Get the location of our projection matrix in the shader
	viewMatrixLocation = glGetUniformLocation(program, "viewMatrix"); // Get the location of our view matrix in the shader
	modelMatrixLocation = glGetUniformLocation(program, "modelMatrix"); // Get the location of our model matrix in the shader

	

	glBindAttribLocation(program, pos_location, "in_Position");
	glBindAttribLocation(program, texture_location, "in_Texture");
	glBindAttribLocation(program, normal_location, "in_Normal");

	glUseProgram(0);
}


void Polyhedron::createIcosahedron(void)
{
	_points.clear();
	_triangle_index.clear();
	_colors.clear();

	const float X =.525731112119133606f;
	const float Z =.850650808352039932f;
	const float N = 0.f;

	//	{-X,N,Z}, {X,N,Z}, {-X,N,-Z}, {X,N,-Z},
 // {N,Z,X}, {N,Z,-X}, {N,-Z,X}, {N,-Z,-X},
//  {Z,X,N}, {-Z,X, N}, {Z,-X,N}, {-Z,-X, N}


	_points.push_back(glm::vec3(-X, N, Z));
	_points.push_back(glm::vec3(X,N,Z));
	_points.push_back(glm::vec3(-X,N,-Z));
	_points.push_back(glm::vec3(X,N,-Z));

	_points.push_back(glm::vec3(N,Z,X));
	_points.push_back(glm::vec3(N,Z,-X));
	_points.push_back(glm::vec3(N,-Z,X));
	_points.push_back(glm::vec3(N,-Z,-X));

	_points.push_back(glm::vec3(Z,X,N));
	_points.push_back(glm::vec3(-Z,X, N));
	_points.push_back(glm::vec3(Z,-X,N));
	_points.push_back(glm::vec3(-Z,-X, N));

	_colors.push_back(glm::vec3(1, 0, 0));
	_colors.push_back(glm::vec3(1, 0, 0));
	_colors.push_back(glm::vec3(1, 0, 0));
	_colors.push_back(glm::vec3(1, 0, 0));

	_colors.push_back(glm::vec3(1, 1, 0));
	_colors.push_back(glm::vec3(1, 1, 0));
	_colors.push_back(glm::vec3(1, 1, 0));
	_colors.push_back(glm::vec3(1, 1, 0));

	_colors.push_back(glm::vec3(0, 0, 1));
	_colors.push_back(glm::vec3(0, 0, 1));
	_colors.push_back(glm::vec3(0, 0, 1));
	_colors.push_back(glm::vec3(0, 0, 1));


//	{0,4,1},{0,9,4},{9,5,4},{4,5,8},{4,8,1},
 // {8,10,1},{8,3,10},{5,3,8},{5,2,3},{2,7,3},
 // {7,10,3},{7,6,10},{7,11,6},{11,0,6},{0,1,6},
 // {6,1,10},{9,0,11},{9,11,2},{9,2,5},{7,2,11}

	_triangle_index.push_back(glm::vec3(0,4,1));
	_triangle_index.push_back(glm::vec3(0,9,4));
	_triangle_index.push_back(glm::vec3(9,5,4));
	_triangle_index.push_back(glm::vec3(4,5,8));
	_triangle_index.push_back(glm::vec3(4,8,1));

	_triangle_index.push_back(glm::vec3(8,10,1));
	_triangle_index.push_back(glm::vec3(8,3,10));
	_triangle_index.push_back(glm::vec3(5,3,8));
	_triangle_index.push_back(glm::vec3(5,2,3));
	_triangle_index.push_back(glm::vec3(2,7,3));

	_triangle_index.push_back(glm::vec3(7,10,3));
	_triangle_index.push_back(glm::vec3(7,6,10));
	_triangle_index.push_back(glm::vec3(7,11,6));
	_triangle_index.push_back(glm::vec3(11,0,6));
	_triangle_index.push_back(glm::vec3(0,1,6));

	_triangle_index.push_back(glm::vec3(6,1,10));
	_triangle_index.push_back(glm::vec3(9,0,11));
	_triangle_index.push_back(glm::vec3(9,11,2));
	_triangle_index.push_back(glm::vec3(9,2,5));
	_triangle_index.push_back(glm::vec3(7,2,11));

}


int Polyhedron::vertex_for_edge(Lookup& lookup, vector<glm::vec3>& vertices, int first, int second)
{
	Lookup::key_type key(first, second);
	if (key.first>key.second)
		std::swap(key.first, key.second);
 
	auto inserted=lookup.insert({key, vertices.size()});

	if (inserted.second)
	{
		glm::vec3& edge0 = vertices[first];
		glm::vec3& edge1=vertices[second];
		glm::vec3 point = glm::normalize(edge0+edge1);

		vertices.push_back(point);
	}
 
	return inserted.first->second;
}


/*
Subdivide the Icosahedron triangles
*/
vector<glm::ivec3> Polyhedron::subdivide(vector<glm::vec3>& vertices, vector<glm::ivec3> triangles)
{
	Lookup lookup;
	vector<glm::ivec3> result;
 
	for (auto each:triangles)
	{
		std::array<int, 3> mid;
		for (int edge=0; edge<3; edge++)
		{
			mid[edge]=vertex_for_edge(lookup, vertices, each[edge], each[edge+1]);
		}
 
		result.push_back({each[0], mid[0], mid[2]});
		result.push_back({each[1], mid[1], mid[0]});
		result.push_back({each[2], mid[2], mid[1]});
		result.push_back({mid[0], mid[1], mid[2]});
	}
 
  return result;
}


/*
Create a Polyhedron
@param subdivisions - the number of subdivisions
@param vertices - the Icosahedron vertices
@param triangles - the Icosahedron triangle index list
*/
std::pair< vector<glm::vec3>, vector<glm::ivec3> > Polyhedron::createPolyhedron(int subdivisions, vector<glm::vec3>& vertices, vector<glm::ivec3> triangles)
{
 
	for (int i=0; i<subdivisions; ++i)
	{
		triangles = subdivide(vertices, triangles);
	}
 
	return make_pair(vertices, triangles);
}


/*
Draw the coordinate system
@param viewMatrix - a view matrix object
@param modelMatrix - a model matrix object.
*/
void Polyhedron::draw(glm::mat4 projectionMatrix, glm::mat4 viewMatrix, glm::mat4 modelMatrix)
{
	// Enable the shader program
	glUseProgram(program);



	// this changes the camera location
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]); // send the view matrix to our shader
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]); // send the model matrix to our shader
	glUniformMatrix4fv(projMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]); // send the projection matrix to our shader


	glBindVertexArray(vaoID[0]);

	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	// Draw the triangles
 	glDrawElements(GL_TRIANGLES, _I, GL_UNSIGNED_INT, 0);
	

	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	// Unbind our Vertex Array Object
	glBindVertexArray(0);

	// Unbind the shader program
	glUseProgram(0);

}