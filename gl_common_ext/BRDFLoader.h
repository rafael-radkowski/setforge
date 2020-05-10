/**
@class BRDFLoader

@brief The class provides methods to load 2D textures and to assign those to brdf materials. 

Rafael Radkowski
Iowa State University
November 2018
rafael@iastate.edu

MIT License
-----------------------------------------------------
Last edits:

*/

#ifndef _BRDFLOADER_
#define _BRDFLOADER_


#include <iostream>
#include <string>


#include "BRDFTypes.h" // The brdf material
#include "BMPLoader.h"
#include "FileUtils.h"
#include "Texture2D.h"

using namespace std;

namespace cs557
{

class BRDFLoader
{
public:

    /*
    Read the bitmap maps containing BRDF material values and prepare the BRDF materials. 
	The function initializes all textures. 
	@param mat - the brdf material type
	@param albedo_map - a string containing a path to a 2D albedo map.
	@param metalic_map - a string containing a path to a 2D metalic map.
	@param roughness_map - a string containing a path to a 2D roughness map; the map should cotains roughness values between 0 and 1. 
	@param ambient_occlusion_map - a string containing a path to a 2D ambient occlusion map; the map should cotains occlusion values between 0 and 1. 
    */
    static int ReadBMP(BRDFTexMaterial& mat, const string albedo_map, const string metalic_map, const string roughness_map, const string ambient_occlusion_map );


	/*
    Read the bitmap maps containing BRDF material values, an IBL reflectance map, and prepare the BRDF materials. 
	The function initializes all textures. 
	@param mat - the brdf material type
	@param albedo_map - a string containing a path to a 2D albedo map.
	@param metalic_map - a string containing a path to a 2D metalic map.
	@param roughness_map - a string containing a path to a 2D roughness map; the map should cotains roughness values between 0 and 1. 
	@param ambient_occlusion_map - a string containing a path to a 2D ambient occlusion map; the map should cotains occlusion values between 0 and 1. 
	@param ibl_map - the IBL map. 
    */
    static int ReadIBL(BRDFTexMaterial& mat, const string albedo_map, const string metalic_map, const string roughness_map, const string ambient_occlusion_map, const string ibl_map );

};


}
#endif