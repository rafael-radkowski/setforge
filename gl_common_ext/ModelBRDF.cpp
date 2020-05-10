#include "ModelBRDF.h"


namespace ns_ModelBRDF
{


	static const string vs_string_410 =
		"#version 410 core                  \n"                               
		" \n"
		"uniform mat4 projectionMatrix;     \n"                               
		"uniform mat4 viewMatrix;     \n"                                      
		"uniform mat4 modelMatrix;  \n"
		"in vec3 in_Position;     \n"                                                                                                      
		"in vec3 in_Normal;       \n"                                
		"out vec3 pass_Normal; \n"
		"out vec3 pass_Position;\n"
		"out vec4 pass_Coordinates;						\n"
		"out vec4 pass_Color;     \n"                                        
		" \n"
		"void main(void)         \n"                                         
		"{            \n"
		"	// for lighting\n"
		"	pass_Position = vec3(  viewMatrix *  modelMatrix  *  vec4(in_Position, 1.0));   \n" 
		"\n"
		"	// to calculate the real (linear) depth		\n"
		"	pass_Coordinates = projectionMatrix * viewMatrix * modelMatrix * vec4(in_Position, 1.0);	\n"
		"\n"
		"	// make it red, a backup color\n"
		"	pass_Color = vec4(1.0,0.0,0.0,1.0);   \n"
		"\n"
		"   // pass the normal vector along\n"
		"	pass_Normal =   vec3( transpose(inverse(viewMatrix * modelMatrix)) *  vec4(in_Normal, 0.0));      \n"                   
		"\n"
		"	// pass the position				                       \n"
		"	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(in_Position, 1.0);       \n"
		"	\n"
		"}\n";                                                      


	static const string fs_string_410 =
		"#version 410 core                                                 \n"
		"                                                                  \n"
		"#define MAX_LIGHTS 8								\n"
		"								\n"
		"// the incoming variables								\n"
		"in vec3 pass_Normal;								\n"
		"in vec3 pass_Position;								\n"
		"in vec4 pass_Color;								\n"
		"in vec4 pass_Coordinates;						\n"	
		"								\n"
		"uniform mat4 projectionMatrix;                      								\n"              
		"uniform mat4 viewMatrix;                            								\n"               
		"uniform mat4 modelMatrix;  								\n"
		"								\n"
		"float PI = 3.1415;                                  								\n"                        
		"								\n"
		"// The material parameters								\n"
		"uniform struct LightSource {								\n"
		"	vec3 position;								\n"
		"	vec3 direction;								\n"
		"	vec3 color;								\n"
		"	float intensity;								\n"
		"	float cutoff_in; // inner								\n"
		"	float cutoff_out; // outer								\n"
		"	float k1;    //attenuation								\n"
		"	float k2;    //attenuation								\n"
		"	bool used;								\n"
		"	int  type;  //0:point, 1:spot, 2:directional								\n"
		"} light[MAX_LIGHTS];								\n"
		"								\n"
		"								\n"
		"// The parameters for a BRDF materail								\n"
		"uniform struct BRDFMaterial {								\n"
		"	vec3 albedo; // the general diffuse color								\n"
		"	float metallic; // metalic factor runs in a range from 0 to 1								\n"
 		"	float roughness; // roughness factor runs in a range from 0 to 1								\n"
 		"	float ao; // ambient intensity								\n"
		"	vec3 lightColor; // color of the light. 								\n"
		"	vec3 F0; 								\n"
		"								\n"
		"	float k1; // linear attenuation								\n"
		"	float k2; // quadratic attenuation								\n"
		"} brdf[1];								\n"
		"								\n"
		"								\n"
		"// the final color								\n"
		"out vec4 frag_out;   								\n"
		"								\n"
		"float calculateAttenuation(vec3 light_position, vec3 fragment_position, float k1, float k2)								\n"
		"{								\n"
		"	float distance    = length(light_position - fragment_position);								\n"
		"	float attenuation = 1.0 / (1.0 + k1 * distance + 								\n"
		"					k2 * (distance * distance));  								\n"
		"	return attenuation;								\n"
		"}								\n"
		"								\n"
		"vec3 fresnelSchlick(float cosTheta, vec3 F0)								\n"
		"{								\n"
		"	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);								\n"
		"}								\n"
		"								\n"
		"float DistributionGGX(vec3 N, vec3 H, float roughness)								\n"
		"{								\n"
		"	float a      = roughness*roughness;								\n"
		"	float a2     = a*a;								\n"
		"	float NdotH  = max(dot(N, H), 0.0);								\n"
		"	float NdotH2 = NdotH*NdotH;								\n"
		"								\n"
		"	float num   = a2;								\n"
		"	float denom = (NdotH2 * (a2 - 1.0) + 1.0);								\n"
		"	denom = PI * denom * denom;								\n"
		"								\n"
		"	return num / denom;								\n"
		"}								\n"
		"								\n"
		"float GeometrySchlickGGX(float NdotV, float roughness)								\n"
		"{								\n"
		"	float r = (roughness + 1.0);								\n"
		"	float k = (r*r) / 8.0;								\n"
		"								\n"
		"	float num   = NdotV;								\n"
		"	float denom = NdotV * (1.0 - k) + k;								\n"
		"								\n"
		"	return num / denom;								\n"
		"}								\n"
		"								\n"
		"float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)								\n"
		"{								\n"
		"	float NdotV = max(dot(N, V), 0.0);								\n"
		"	float NdotL = max(dot(N, L), 0.0);								\n"
		"	float ggx2  = GeometrySchlickGGX(NdotV, roughness);								\n"
		"	float ggx1  = GeometrySchlickGGX(NdotL, roughness);								\n"
		"								\n"
		"	return ggx1 * ggx2;								\n"
		"}								\n"
		"								\n"
		"void main(void)                                            								\n"      
		"{   								\n"
		"	// eye position 								\n"
		"	vec3 E = normalize( vec3(viewMatrix[3][0], viewMatrix[3][1], viewMatrix[3][2]) );								\n"
		"								\n"
		"	vec3 F0 = brdf[0].F0;								\n"
		"	F0 = mix(F0, brdf[0].albedo, brdf[0].metallic);								\n"
		"								\n"
		"	// camera view to fragment position								\n"
		"	vec3 V = normalize(E -pass_Position);								\n"
		"								\n"
		"	 // normal vector								\n"
		"	vec3 N = normalize(pass_Normal);								\n"
		"								\n"
		"	vec3 Lo = vec3(0.0);								\n"
		"	for (int i=0; i<MAX_LIGHTS; i++){								\n"
		"								\n"
		"		if(light[i].used == false) continue;								\n"
		"								\n"
		"		// Light in view space								\n"
		"		vec3 L_viewspace = vec3(viewMatrix * vec4(light[i].position, 1.0));								\n"
		"								\n"
		"		// light to fragment position								\n"
		"		vec3 L = normalize(L_viewspace - pass_Position);								\n"
		"								\n"
		"		  // halfway -vector								\n"
		"		vec3 H = normalize(V + L);								\n"
		"								\n"
		"		// calculate the radiance comming from the light source. 								\n"
		"		vec3  lightColor  = brdf[0].lightColor;								\n"
		"		float cosTheta    = max(dot(pass_Normal, L), 0.0);								\n"
		"		float attenuation = calculateAttenuation(pass_Position, L_viewspace, brdf[0].k1, brdf[0].k2);								\n"
		"		vec3  radiance    = lightColor * attenuation * cosTheta;								\n"
        "								\n"
		"		 // cook-torrance reflection brdf								\n"
		"		float NDF = DistributionGGX(N, H, brdf[0].roughness);      								\n"  
		"		float G   = GeometrySmith(N, V, L, brdf[0].roughness);     								\n" 
		"		vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0); 								\n"
		"								\n"
		"		vec3 numerator    = NDF * G * F;								\n"
		"		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);								\n"
		"		vec3 specular     = numerator / max(denominator, 0.001);  								\n"
		"								\n"
		"		// Diffuse and ambient term								\n"
		"		vec3 kS = F;								\n"
		"		vec3 kD = vec3(1.0) - kS;								\n"
		"		kD *= 1.0 - brdf[0].metallic;	    								\n"
		"								\n"
		"		 // add to outgoing radiance Lo								\n"
		"		float NdotL = max(dot(N, L), 0.0);  								\n"
		"		Lo += (kD * brdf[0].albedo / PI + specular) * radiance * NdotL; 								\n"
		"								\n"
		"		//Lo = vec3(G );								\n"
		"	}								\n"
		"								\n"
		"	//ambient add								\n"
		"	vec3 ambient = vec3(0.03) * brdf[0].albedo * brdf[0].ao;								\n"
		"	vec3 color = ambient + Lo;								\n"
		"								\n"
		"	// gamma correction								\n"
		"	color = color / (color + vec3(1.0));								\n"
		"	color = pow(color, vec3(1.0/2.2));  								\n"
		"								\n"
		"	frag_out = vec4(color, 1.0); 								\n"      
		"  // frag_out = vec4(Lo, 1.0);									\n"
		"																							\n"		
		"	//------------------------------------------------										\n"	
		"	// Get linear depth back																\n"	
		"																							\n"	
		"	// must match  the projection															\n"	
		"	const float n = 0.01; // camera z near													\n"	
		"	const float f = 10.0; // camera z far													\n"	
		"																							\n"	
		"	//   range without z/w -> [0, high value, linear but not scaled], with z/w -> range [-1,1]\n"	
		"	float current_depth = pass_Coordinates.z/ pass_Coordinates.w;							\n"	
		"																							\n"	
		"	// Adjust the range to [0,1]															\n"	
		"	//current_depth = 0.5 * current_depth + 0.5;  // range [0,1]							\n"	
		"																							\n"	
		"	// linear depth [0, depth]. requires range [-1, 1] as input.							\n"	
		"	// Calculate the real depth																\n"	
		"	current_depth  = (2.0 * f  * n) / (f + n - current_depth * (f - n)) ;					\n"	
		"																							\n"	
		"	gl_FragDepth =  current_depth;															\n"	
		"}\n";    

}

using namespace ns_ModelBRDF;
using namespace cs557;



/*
Load an OBJ model from file
@param path_and_filename - number of rows
@param shader_program - overwrite the default shader program by passing a hander to the constructor
*/
void ModelBRDF::create(string path_and_filename, BRDFMaterial& material, int shader_program) 
{
	// copy the material reference. 
	_brdf_material = material;

	// load the brdf shader program. 
	if(shader_program == -1){
		//shader_program = LoadAndCreateShaderProgram("../data/shaders/brdf_shader.vs", "../data/shaders/brdf_shader.fs");
		shader_program = CreateShaderProgram(vs_string_410, fs_string_410);
	}
	
	// init the model
	cs557::OBJModel::create( path_and_filename, shader_program);


	// apply the brdf material
	_brdf_material.apply(getProgram());

}


/*
Draw the coordinate system
@param viewMatrix - a view matrix object
@param modelMatrix - a model matrix object.
*/
void cs557::ModelBRDF::draw(glm::mat4 projectionMatrix, glm::mat4 viewMatrix, glm::mat4 modelMatrix )
{
	// No indices loaded. 
	if(_I == 0) return; 

	// Enable the shader program
	glUseProgram(program);


	// this changes the camera location
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]); // send the view matrix to our shader
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]); // send the model matrix to our shader
	glUniformMatrix4fv(projMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]); // send the projection matrix to our shader


	// Bind the buffer and switch it to an active buffer
	glBindVertexArray(vaoID[0]);

	for (int i = 0; i < start_index.size(); i++) {
		_brdf_material.apply(program);
		glUseProgram(program);

		// Draw the triangles
		glDrawElements(GL_TRIANGLES, length[i], GL_UNSIGNED_INT, (GLint*)(sizeof(int)*start_index[i]));
	}
	//glDrawElements(GL_TRIANGLES, _I, GL_UNSIGNED_INT, 0);

	// Unbind our Vertex Array Object
	glBindVertexArray(0);

	// Unbind the shader program
	glUseProgram(0);

}


/*
Draw the coordinate system
@param viewMatrix - a view matrix object
@param modelMatrix - a model matrix object.
*/
void cs557::ModelBRDF::draw(glm::mat4 projectionMatrix, glm::mat4 viewMatrix)
{
	draw(projectionMatrix,  viewMatrix,  modelMatrix );
}
