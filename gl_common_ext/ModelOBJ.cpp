#include "ModelOBJ.h"



namespace cs557_OBJModel
{

	static const string vs_string_410 =
		"#version 410 core                                                 \n"
		"                                                                   \n"
		"uniform mat4 projectionMatrix;                                    \n"
		"uniform mat4 viewMatrix;                                           \n"
		"uniform mat4 modelMatrix;                                          \n"
		"in vec3 in_Position;                                               \n"
		"in vec2 in_Texture;                                                \n"
		"in vec3 in_Normal;                                                  \n"
		"out vec3 pass_Color;                                               \n"
		"out vec2 pass_Texture;												\n"
		"                                                                  \n"
		"void main(void)                                                   \n"
		"{                                                                 \n"
		"    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(in_Position, 1.0);  \n"
		"    pass_Color = in_Normal;                                         \n"
		"    pass_Texture = in_Texture;                                         \n"
		"}                                                                 \n";

	// Fragment shader source code. This determines the colors in the fragment generated in the shader pipeline. In this case, it colors the inside of our triangle specified by our vertex shader.
	static const string fs_string_410 =
		"#version 410 core                                                 \n"
		"                                                                  \n"
		"in vec3 pass_Color;                                                 \n"
		"in vec2 pass_Texture;												\n"
		"out vec4 color;                                                    \n"
		"void main(void)                                                   \n"
		"{                                                                 \n"
		"    color = vec4(pass_Color, 1.0);                               \n"
		"}                                                                 \n";



};



using namespace cs557_OBJModel;



	
/*
Load an OBJ model from file
@param path_and_filename - number of rows
@param shader_program - overwrite the default shader program by passing a hander to the constructor
*/
void cs557::OBJModel::create(string path_and_filename, int shader_program)
{
	program = -1;
	_N = 0;
	_I = 0;

	// This overwrite the default shader program
	if(shader_program != -1){
		program = shader_program;
	}


	float center_x = 0.0;
	float center_y = 0.0;
	float center_z = 0.0;

	modelMatrix ={  1, 0, 0, 0,
					0, 1, 0 ,0,
					0, 0, 1, 0,
					0, 0, 0, 1};

	std::vector<std::pair<glm::vec3, glm::vec2> > points; // points and texture coordinates
	std::vector<glm::vec3> normals;
	std::vector<int> indices;
	

	// create a shader program only if the progrm was not overwritten. 
	if(program == -1)
		program = cs557::CreateShaderProgram(vs_string_410, fs_string_410);


	// Find the id's of the related variable name in your shader code. 
	projMatrixLocation = glGetUniformLocation(program, "projectionMatrix"); // Get the location of our projection matrix in the shader
	viewMatrixLocation = glGetUniformLocation(program, "viewMatrix"); // Get the location of our view matrix in the shader
	modelMatrixLocation = glGetUniformLocation(program, "modelMatrix"); // Get the location of our model matrix in the shader
	int pos_location = glGetAttribLocation(program, "in_Position");
	int norm_location = glGetAttribLocation(program, "in_Normal");
	int tex_location = glGetAttribLocation(program, "in_Texture");


	// Load the geometry from file. 
	objl::Loader loader;
	loader.LoadFile(path_and_filename);


	int current_start_index = 0;
	start_index.clear();
	length.clear();


	int size = loader.LoadedMeshes.size();

	for(int i=0; i<size; i++)
	{

		objl::Mesh curMesh = loader.LoadedMeshes[i];

		// process all materials
		cs557::Material mat;
		mat.ambient_mat = glm::vec3(curMesh.MeshMaterial.Ka.X, curMesh.MeshMaterial.Ka.Y, curMesh.MeshMaterial.Ka.Z) ;
		mat.diffuse_mat = glm::vec3(curMesh.MeshMaterial.Kd.X, curMesh.MeshMaterial.Kd.Y, curMesh.MeshMaterial.Kd.Z) ;
		mat.specular_mat = glm::vec3(curMesh.MeshMaterial.Ks.X, curMesh.MeshMaterial.Ks.Y, curMesh.MeshMaterial.Ks.Z) ;
		mat.specular_s = curMesh.MeshMaterial.Ns;
		mat.specular_int = 0.2;
		mat.ambient_int = 0.2;
		mat.diffuse_int = 0.8;
		mat.with_error_check = false;

		materials.push_back(mat);

		// process all textures
		processTextures(program, curMesh, path_and_filename);
	
		

		for (int j = 0; j < curMesh.Vertices.size(); j++)
		{
			points.push_back(make_pair( glm::vec3(curMesh.Vertices[j].Position.X, curMesh.Vertices[j].Position.Y, curMesh.Vertices[j].Position.Z),
			 							glm::vec2(curMesh.Vertices[j].TextureCoordinate.X, curMesh.Vertices[j].TextureCoordinate.Y )));
			normals.push_back(glm::vec3(curMesh.Vertices[j].Normal.X, curMesh.Vertices[j].Normal.Y, curMesh.Vertices[j].Normal.Z) );
		}
		
		start_index.push_back(current_start_index);
		length.push_back(curMesh.Indices.size());

		for (int j = 0; j < curMesh.Indices.size(); j++)
		{
			indices.push_back(curMesh.Indices[j] + current_start_index); 
			//  + current_start_index is requires since the obj loader objl::Loader loader
			// starts a new index count for each mesh instead of keeping the original indices. 
			// This code puts all vertices and indices into one vertex buffer and index buffer. 
			// So the + current_start_index implements the offset jump. 
		}

		current_start_index += curMesh.Indices.size();
		//glBindAttribLocation(program, pos_location, "in_Position");
		//glBindAttribLocation(program, norm_location, "in_Normal");
		//glBindAttribLocation(program, tex_location, "in_Texture");
	}
	_I = indices.size();
	_N = points.size();

	// Set the object to the center of mass. 
	GLGeometryUtils::SetToBBCenter(points);

	// compute the bounding box of the object. 
	GLGeometryUtils::CalcAll(points, boundingbox, centroid );

	// create a vertex buffer object
	cs557::CreateVertexObjectsIndexed53(vaoID, vboID, iboID, &points[0].first.x, &normals[0].x, _N, &indices[0], _I, pos_location, tex_location, norm_location );

}


/*
Draw the coordinate system
@param viewMatrix - a view matrix object
@param modelMatrix - a model matrix object.
*/
void cs557::OBJModel::draw(glm::mat4 projectionMatrix, glm::mat4 viewMatrix, glm::mat4 modelMatrix )
{

	// Enable the shader program
	glUseProgram(program);



	// this changes the camera location
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]); // send the view matrix to our shader
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]); // send the model matrix to our shader
	glUniformMatrix4fv(projMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]); // send the projection matrix to our shader


	 // Bind the buffer and switch it to an active buffer
	glBindVertexArray(vaoID[0]);

	for (int i = 0; i < start_index.size(); i++) {
		glUseProgram(program);

		if(!with_override_material)
			materials[i].apply(program);
		else{
			override_material.apply(program);
		}
	
		
		glUseProgram(program);

		// apply texturs
		if (textures[i].num_textures > 0) {
			 // Activate the texture unit and bind the texture. 
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[i].diff.tex_id);
		}

		// Draw the triangles
		glDrawElements(GL_TRIANGLES, length[i], GL_UNSIGNED_INT, (GLint*)(sizeof(int)*start_index[i]));
	}
	//glDrawElements(GL_TRIANGLES, _I, GL_UNSIGNED_INT, 0);

	// Unbind our Vertex Array Object
	glBindVertexArray(0);

	// Unbind the shader program
	glUseProgram(0);


}


void cs557::OBJModel::draw(glm::mat4 projectionMatrix, glm::mat4 viewMatrix)
{
	draw(projectionMatrix, projectionMatrix, viewMatrix );
}



void cs557::OBJModel::processTextures(int& program, objl::Mesh& curMesh, string path)
{
	// Extract the current path. 
	int idx = path.find_last_of("/");
	string curr_path = "";
	if (idx != 0) {
		curr_path = path.substr(0, idx+1);
	}

	cs557::TexMaterial tex;

	//-------------------------------------------------------------------------------------------------------
	// diffuse texture
	string tex_Kd = curMesh.MeshMaterial.map_Kd;
	
	if (tex_Kd.length() > 0) {
		string tex_path = curr_path;
		tex_path.append(tex_Kd);

		// Load the texture
		TextureLoader::Load(tex_path, &tex.diff.map, &tex.diff.width, &tex.diff.height, &tex.diff.channels);

		// create the gl texture
		glUseProgram(program);
		cs557::CreateTexture2D(tex.diff.width, tex.diff.height, tex.diff.channels, tex.diff.map, &tex.diff.tex_id);

		// Activate the texture unit and bind the texture. 
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex.diff.tex_id);

		// Fetch the texture location and set the parameter to 0.
		// Note that 0 is the number of the texture unit GL_TEXTURE0.
		tex.diff.tex_loc = glGetUniformLocation(program, "tex[0].tex_kd");
		glUniform1i(tex.diff.tex_loc, 0);

		// set the texture to active. 
		int u = glGetUniformLocation(program, "tex[0].with_tex_kd");
		glUniform1i(u, 1);

		glUseProgram(0);

		tex.num_textures++;
	}

	//-------------------------------------------------------------------------------------------------------
	// ambient texture
	string tex_Ka = curMesh.MeshMaterial.map_Ka;

	if (tex_Ka.length() > 0) {
		string tex_path = curr_path;
		tex_path.append(tex_Ka);

		// Load the texture
		TextureLoader::Load(tex_path, &tex.ambi.map, &tex.ambi.width, &tex.ambi.height, &tex.ambi.channels);

		// create the gl texture
		glUseProgram(program);
		cs557::CreateTexture2D(tex.ambi.width, tex.ambi.height, tex.ambi.channels, tex.ambi.map, &tex.ambi.tex_id);

		// Activate the texture unit and bind the texture. 
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex.ambi.tex_id);

		// Fetch the texture location and set the parameter to 0.
		// Note that 0 is the number of the texture unit GL_TEXTURE0.
		tex.ambi.tex_loc = glGetUniformLocation(program, "tex[0].tex_ka");
		glUniform1i(tex.ambi.tex_loc, 0);

		// set the texture to active. 
		int u = glGetUniformLocation(program, "tex[0].with_tex_ka");
		glUniform1i(u, 1);

		glUseProgram(0);

		tex.num_textures++;
	}


	//-------------------------------------------------------------------------------------------------------
	// specular texture
	string tex_Ks = curMesh.MeshMaterial.map_Ks;

	if (tex_Ks.length() > 0) {
		string tex_path = curr_path;
		tex_path.append(tex_Ks);

		// Load the texture
		TextureLoader::Load(tex_path, &tex.spec.map, &tex.spec.width, &tex.spec.height, &tex.spec.channels);

		// create the gl texture
		glUseProgram(program);
		cs557::CreateTexture2D(tex.spec.width, tex.spec.height, tex.spec.channels, tex.spec.map, &tex.spec.tex_id);

		// Activate the texture unit and bind the texture. 
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex.spec.tex_id);

		// Fetch the texture location and set the parameter to 0.
		// Note that 0 is the number of the texture unit GL_TEXTURE0.
		tex.spec.tex_loc = glGetUniformLocation(program, "tex[0].tex_ks");
		glUniform1i(tex.spec.tex_loc, 0);

		// set the texture to active. 
		int u = glGetUniformLocation(program, "tex[0].with_tex_ks");
		glUniform1i(u, 1);

		glUseProgram(0);

		tex.num_textures++;
	}


	//----------------------------------------------------
	// set the texture mode
	glUseProgram(program);
	int loc = glGetUniformLocation(program, "tex[0].tex_mode");
	if (tex.num_textures > 0) 
		glUniform1i(loc, tex.tex_mode);
	else
		glUniform1i(loc, 0);
	glUseProgram(0);

	textures.push_back(tex);


}


/*
Set the texture parameters.
@param blend_mode - set the texture blend mode. MODULATE and REPLACE are currently supported. 
*/
void cs557::OBJModel::setTextureParam(TextureMode blend_mode)
{
	for (auto & i : textures)
	{
		i.tex_mode = blend_mode;
		glUseProgram(program);
		int loc = glGetUniformLocation(program, "tex[0].tex_mode");
		if (i.num_textures > 0) // only activate the texture mode if a texture is available
			glUniform1i(loc, i.tex_mode);
		else
			glUniform1i(loc, 0);
		glUseProgram(0);
	}
}



/*
Set a material for this object
@param material - the material parameters. 
*/
void cs557::OBJModel::setMaterial(cs557::Material material)
{
	override_material = material;
	with_override_material = true;
}

