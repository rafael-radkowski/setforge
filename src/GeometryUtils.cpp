#include "GeometryUtils.h"

namespace nsGeometryUtils {
	
	bool verbose = true;

	
}

using namespace nsGeometryUtils;
using namespace std;


bool GeometryUtils::CalcBoundingBox(std::vector<Eigen::Vector3f>& points, Eigen::Vector3f& boundingbox_size)
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
	float maxX, maxY, maxZ, minX, minY, minZ;
    maxX =  std::numeric_limits<float>::min();
    maxY =  std::numeric_limits<float>::min();
    maxZ =  std::numeric_limits<float>::min();
    minX =  std::numeric_limits<float>::max();
    minY =  std::numeric_limits<float>::max();
    minZ =  std::numeric_limits<float>::max();

    for( auto p : points){
       if(p.x() > maxX) maxX = p.x();
       if(p.x() < minX) minX = p.x();
       if(p.y() > maxY) maxY = p.y();
       if(p.y() < minY) minY = p.y();
       if(p.z() > maxZ) maxZ = p.z();
       if(p.z() < minZ) minZ = p.z();
    }

    if(verbose){
        cout << "[INFO] GeometryUtils - Min x: " << minX << ", max x: " << maxX << endl;
        cout << "[INFO] GeometryUtils - Min y: " << minY << ", max y: " << maxY << endl;
        cout << "[INFO] GeometryUtils - Min z: " << minZ << ", max z: " << maxZ << endl;
    }

	boundingbox_size.x()  = maxX - minX;
	boundingbox_size.y()  = maxY - minY;
	boundingbox_size.z()  = maxZ - minZ;

	Eigen::Vector3f bounding_box;
	bounding_box[0] = boundingbox_size.x();
	bounding_box[1] = boundingbox_size.y();
	bounding_box[2] = boundingbox_size.z(); 

	cout << "[INFO] - Object dimensions" << endl;
	cout << "[INFO] - Dim x: " << bounding_box[0] << "."  << endl;
    cout << "[INFO] - Dim y: " << bounding_box[1] << "."  << endl;
    cout << "[INFO] - Dim z: " << bounding_box[2] << "."  << endl;


	return true;
}


/*
Identify the centroid of the object. 
@param cx - location to store the x-coordinate of the centroid
@param cy - location to store the y-coordinate of the centroid
@param cz - location to store the z-coordinate of the centroid
*/
bool GeometryUtils::CalcGeometricCentroid(std::vector<Eigen::Vector3f>& points, Eigen::Vector3f& centroid)
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
		x += p.x();
		y += p.y();
		z += p.z();
	}

	centroid.x() = x / N;
	centroid.y() = y / N;
	centroid.z() = z / N;


	 if(verbose){
        cout << "[INFO] GeometryUtils - geometric centroid x: " << centroid.x() << ". " << endl;
        cout << "[INFO] GeometryUtils - geometric centroid y: " << centroid.y() << ". " << endl;
        cout << "[INFO] GeometryUtils - geometric centroid z: " << centroid.z() << ". " << endl;
    }

	return true;
}
