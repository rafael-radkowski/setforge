#include "GLGeometryUtils.h"



namespace nsGLGeometryUtils {
	
	bool verbose = false;
	float maxX = 0.0;
	float maxY = 0.0;
	float maxZ = 0.0;
	float minX = 0.0;
	float minY = 0.0;
	float minZ = 0.0;
	
}

using namespace nsGLGeometryUtils;
using namespace std;


bool GLGeometryUtils::CalcBoundingBox(std::vector<glm::vec3>& points, glm::vec3& boundingbox_size)
{
	

	if (points.size() == 0) {
		if(verbose){
			std::cout << "[ERROR] - GeometryUtils: no points given for bounding box calculation." << std::endl;
		}
		return false;
	}

	int size = points.size();
    //--------------
    // find min and max values 
	//float maxX, maxY, maxZ, minX, minY, minZ;
    maxX =  std::numeric_limits<float>::min();
    maxY =  std::numeric_limits<float>::min();
    maxZ =  std::numeric_limits<float>::min();
    minX =  std::numeric_limits<float>::max();
    minY =  std::numeric_limits<float>::max();
    minZ =  std::numeric_limits<float>::max();

    for( auto p : points){
       if(p.x > maxX) maxX = p.x;
       if(p.x < minX) minX = p.x;
       if(p.y > maxY) maxY = p.y;
       if(p.y < minY) minY = p.y;
       if(p.z > maxZ) maxZ = p.z;
       if(p.z < minZ) minZ = p.z;
    }

    if(verbose){
        cout << "[INFO] - Min x: " << minX << ", max x: " << maxX << endl;
        cout << "[INFO] - Min y: " << minY << ", max y: " << maxY << endl;
        cout << "[INFO] - Min z: " << minZ << ", max z: " << maxZ << endl;
    }

	boundingbox_size.x  = maxX - minX;
	boundingbox_size.y  = maxY - minY;
	boundingbox_size.z  = maxZ - minZ;

	glm::vec3 bounding_box;
	bounding_box[0] = boundingbox_size.x;
	bounding_box[1] = boundingbox_size.y;
	bounding_box[2] = boundingbox_size.z; 

	if(verbose){
		cout << "[INFO] - Object dimensions" << endl;
		cout << "[INFO] - Dim x: " << bounding_box[0] << "."  << endl;
		cout << "[INFO] - Dim y: " << bounding_box[1] << "."  << endl;
		cout << "[INFO] - Dim z: " << bounding_box[2] << "."  << endl;
	}

	return true;
}



 bool GLGeometryUtils::CalcBoundingBox(std::vector<std::pair<glm::vec3, glm::vec2> > & points, glm::vec3& boundingbox_size)
 {

	std::vector<glm::vec3> data;
	std::transform(begin(points), end(points),
               std::back_inserter(data),
               [](auto const& pair){ return pair.first; });


	return CalcBoundingBox(data,  boundingbox_size);
 }



 bool GLGeometryUtils::CalcAll(std::vector<std::pair<glm::vec3, glm::vec2> > & points, glm::vec3& boundingbox_size, glm::vec3& centroid)
 {
	std::vector<glm::vec3> data;
	std::transform(begin(points), end(points),
               std::back_inserter(data),
               [](auto const& pair){ return pair.first; });

	bool ret1 = CalcBoundingBox(data,  boundingbox_size);

	bool ret2 = CalcGeometricCentroid(data, centroid);

	return ret1 & ret2;
 }

/*
Identify the centroid of the object. 
@param cx - location to store the x-coordinate of the centroid
@param cy - location to store the y-coordinate of the centroid
@param cz - location to store the z-coordinate of the centroid
*/
bool GLGeometryUtils::CalcGeometricCentroid(std::vector<glm::vec3>& points, glm::vec3& centroid)
{

	if (points.size() == 0) {
		 if(verbose){
			cout << "[ERROR] - GeometryUtils: no points given for centroid calculation." << endl;
		 }
		return false;
	}

	int N = points.size();
	float x = 0;
	float y = 0;
	float z = 0;

	for (auto p : points) {
		x += p.x;
		y += p.y;
		z += p.z;
	}

	centroid.x = x / N;
	centroid.y = y / N;
	centroid.z = z / N;


	 if(verbose){
        cout << "[INFO] GeometryUtils - geometric centroid x: " << centroid.x << ". " << endl;
        cout << "[INFO] GeometryUtils - geometric centroid y: " << centroid.y << ". " << endl;
        cout << "[INFO] GeometryUtils - geometric centroid z: " << centroid.z << ". " << endl;
    }

	return true;
}



bool GLGeometryUtils::SetToBBCenter(std::vector<std::pair<glm::vec3, glm::vec2> > & points)
{
	std::vector<glm::vec3> data;
	std::transform(begin(points), end(points),
               std::back_inserter(data),
               [](auto const& pair){ return pair.first; });


	SetToBBCenter(data);


	for (int i = 0; i < points.size(); i++) {
		points[i].first = data[i];
	}
}

/*
Move the entire dataset to its centroid
@param cx - x-coordinate of the centroid
@param cy - y-coordinate of the centroid
@param cz - z-coordinate of the centroid
*/
bool GLGeometryUtils::SetToBBCenter(std::vector<glm::vec3>& points)
{
	if (points.size() == 0 ) {
		if(verbose){
			cout << "[ERROR] - no points given to modify." << endl;
		 }
		return false;
	}


	glm::vec3 boundingbox_size;
	CalcBoundingBox(points, boundingbox_size);
	

	float dist_x = (maxX - minX)/2.0 + minX;
	float dist_y = (maxY - minY)/2.0 + minY;
	float dist_z = (maxZ - minZ)/2.0 + minZ;


	size_t N = points.size();
	for (int i = 0; i < N; i++) {
		points[i].x = points[i].x - dist_x;
		points[i].y = points[i].y - dist_y;
		points[i].z = points[i].z - dist_z;
	}

	return true;
}





bool  GLGeometryUtils::SetVerbose(bool verbose)
{
	nsGLGeometryUtils::verbose = verbose;
	return nsGLGeometryUtils::verbose;
}