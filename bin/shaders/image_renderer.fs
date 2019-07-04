#version 410 core     
#define MAX_LIGHTS 4

uniform mat4 projectionMatrix;                                    
uniform mat4 viewMatrix;                                           
uniform mat4 modelMatrix;  
                                                                   
in vec3 pass_Normal;     
in vec3 pass_Position;   
in vec4 pass_Color;   
in vec4 pass_Coordinates;
in vec2 pass_Texture;	

// The material parameters
uniform struct LightSource {
    vec3 position;
	vec3 direction;
	vec3 color;
	float intensity;
	float cutoff_in; // inner
	float cutoff_out; // outer
	float k1;    //attenuation
	float k2;    //attenuation
	bool used;
	int  type;  //0:point, 1:spot, 2:directional
} light[MAX_LIGHTS];


// The material parameters
uniform struct Material {
    vec3  diffColor;
 	float diffInt;
 	vec3  ambColor;
 	float ambInt;
 	vec3  specColor;
 	float specInt;
 	float shininess;
} mat[1];


uniform struct Textures {
	sampler2D tex_kd; // diffuse texture
	bool with_tex_kd; // default is false
	sampler2D tex_ka; // ambient texture
	bool with_tex_ka; // 
	sampler2D tex_ks; // specular texture
	bool with_tex_ks;
	sampler2D tex_bump; // specular texture
	bool with_tex_bump; // bumpmap

} tex[1];





out vec4 color;       

/*
Per-fragment light. 
Note that all vectors need to be in camera/eye-space. 
@param L - vector between light and fragment position.
@param E - eye point
@param s - the light source
@param m - the material 
*/
vec4 useLight(vec3 L, vec3 E, vec3 N, LightSource s, Material m)
{
	vec4 color = vec4(0.0,0.0,0.0,0.0);

	// diffuse light
	vec3 Idiff =  m.diffInt * m.diffColor *  max(dot(L, N), 0.0); 
	Idiff = clamp(Idiff, 0.0, 1.0); 

	// ambient light
	vec4 Iamb = vec4( m.ambColor, 1.0) * m.ambInt;

	// specular light
	vec3 R = reflect(L, N);
	vec3 Ispec =  m.specInt * m.specColor *  pow(max(dot(R, E), 0.0), m.shininess); 

	// calculate color                                     
	color = max(vec4( ( Idiff + Ispec) * s.color, 1.0), Iamb)  * s.intensity;  

	// attenuation 
	float dist = length(L);
	float Iatt = 1 / (1 + s.k1 * dist + s.k2*s.k2 * dist);

	if(s.type == 0)//pointlight
	{
		color = color * Iatt;
	}
	else if(s.type == 1)// spotlight
	{
		vec4 K = viewMatrix * vec4(normalize(s.direction - s.position), 0.0);
		float a = dot(-L,K.xyz); // angle between light dir and fragment position. 

		float c = smoothstep(1.0-s.cutoff_out, 1.0-s.cutoff_in, a); 
		Iatt = Iatt * c; // multiple with attenuation to maintain the distance effect. 

		color = color * Iatt; // attenutation
	}


	return color;
}



/*
Per-fragment light. 
Note that all vectors need to be in camera/eye-space. 
@param L - vector between light and fragment position.
@param E - eye point
@param s - the light source
@param m - the material 
@param tex_diff - diffuse texture color 
@param tex_ambi - ambient texture color 
@param tex_spec - specular texture color 
*/
vec4 useLightExt(vec3 L, vec3 E, vec3 N, LightSource s, Material m, vec4 tex_diff, vec4 tex_ambi, vec4 tex_spec)
{
	vec4 color = vec4(0.0,0.0,0.0,0.0);

	// diffuse light
	vec3 Idiff =  m.diffInt * m.diffColor *  max(dot(L, N), 0.0); 
	Idiff = clamp(Idiff, 0.0, 1.0) * tex_diff.rgb; 

	// ambient light
	vec4 Iamb = vec4( m.ambColor, 1.0) * m.ambInt;
	Iamb = Iamb * tex_ambi;

	// specular light
	vec3 R = reflect(L, N);
	vec3 Ispec =  m.specInt * m.specColor *  pow(max(dot(R, E), 0.0), m.shininess); 
	Ispec = Ispec * tex_spec.rgb;

	// calculate color                                     
	color = max(vec4( ( Idiff + Ispec) * s.color, 1.0), Iamb)  * s.intensity;  

	// attenuation 
	float dist = length(L);
	float Iatt = 1 / (1 + s.k1 * dist + s.k2*s.k2 * dist);

	if(s.type == 0)//pointlight
	{
		color = color * Iatt;
	}
	else if(s.type == 1)// spotlight
	{
		vec4 K = viewMatrix * vec4(normalize(s.direction - s.position), 0.0);
		float a = dot(-L,K.xyz); // angle between light dir and fragment position. 

		float c = smoothstep(1.0-s.cutoff_out, 1.0-s.cutoff_in, a); 
		Iatt = Iatt * c; // multiple with attenuation to maintain the distance effect. 

		color = color * Iatt; // attenutation
	}


	return color;
}



                       
void main(void)                                                  
{    
	// texture colors
	vec4 tex_kd_color = vec4(1,1,1,1);
	if(tex[0].with_tex_kd) tex_kd_color = texture(tex[0].tex_kd, pass_Texture);
	
	// texture colors
	vec4 tex_ka_color = vec4(1,1,1,1);
	if(tex[0].with_tex_ka) tex_ka_color = texture(tex[0].tex_ka, pass_Texture);
	
	// texture colors
	vec4 tex_ks_color = vec4(1,1,1,1);
	if(tex[0].with_tex_ks) tex_ks_color = texture(tex[0].tex_ks, pass_Texture);

	// eye position 
	vec3 E = normalize( vec3(viewMatrix[3][0], viewMatrix[3][1], viewMatrix[3][2]) );
     

	// calc light. 
	vec4 mixed = vec4(0.0,0.0,0.0,1.0);
	for (int i=0; i<MAX_LIGHTS; i++){

		if(light[i].used == false) continue;

		// light position if camera frame
		vec4 L_c = viewMatrix * vec4(light[i].position, 1.0);
	
		// light to fragment 
		vec3 L = normalize( L_c.xyz - pass_Position );
		if(light[i].type == 2) L = light[i].direction;// direct light

		// checks whether the light was set.
		// Multiple lights blend adative
		mixed += useLightExt( L,  E,  pass_Normal, light[i], mat[0], tex_kd_color, tex_ka_color, tex_ks_color );
		
	}

	color = mixed;      


	//------------------------------------------------
	// Get linear depth back

	// must match  the projection
	const float n = 0.01; // camera z near
    const float f = 10.0; // camera z far

	//   range without z/w -> [0, high value, linear but not scaled], with z/w -> range [-1,1]
	float current_depth = pass_Coordinates.z/ pass_Coordinates.w;
	          
	// Adjust the range to [0,1]
	//current_depth = 0.5 * current_depth + 0.5;  // range [0,1]

	// linear depth [0, depth]. requires range [-1, 1] as input.
	// Calculate the real depth
	current_depth  = (2.0 * f  * n) / (f + n - current_depth * (f - n)) ;  

	gl_FragDepth =  current_depth;

	                   
}                                                      