#include "SphereCoordRenderer.h"


/*
Constructor
@param - window_width, window_height - width and height of the opengl, glfw output image in pixel
@param - image_width, image_height - width and height of the image to be rendered and saved to a file.
*/
SphereCoordRenderer::SphereCoordRenderer(int window_width, int window_height, int image_width, int image_height) :
	ModelRenderer(window_width, window_height, image_width, image_height)
{
	_segments = 30;
	_rows = 45;
	_radius = 1.0;
	_center = glm::vec3(0.f,0.f,0.f);
	_up = glm::vec3(0.0f, 1.0f, 0.0f);
	_N = 0;
	_N_current = 0;
	_camera_distance = 1.3;
}

SphereCoordRenderer::~SphereCoordRenderer()
{

}



/*
Create  the number of segments and rows of the sphere.
This defines the number of camera points.
@param segments - the number of vertical segments
@param rows - the number of horizontal rows
*/
void SphereCoordRenderer::createSphereGeometry(float radius, int segments, int rows)
{
	if(_verbose)
		cout << "[INFO] - Created Sphere Coordinates."  << endl;

	if (segments >= 5) {
		_segments = segments;
	}
	else {
		cout << "[ERROR] - The minimum number for sphere segments is 5.\nSet the value to 5." << endl;
		_segments = 5;
	}
	if (rows >= 5) { _rows = rows; }
	else{ 
		cout << "[ERROR] - The minimum number for sphere rows is 5.\nSet the value to 5." << endl;
		_rows = 5;
	}
	if (radius > 0.1){
		_camera_distance = radius;
	}
	else {
		cout << "[ERROR] - Sphere radius cannot be smaller than 0.1.\nSet to 1.0" << endl;
		_camera_distance = 1.0;
	}


	_points.clear();
	_normals.clear();
	make_geometry(_center, _radius, _segments, _rows, _points, _normals);

	_N = _points.size();
	
	if(_verbose)
		cout << "[INFO] - Created " << _N << " coordinates around a sphere." << endl;

	return;
}


/*
Draw the image sequence and save all images to a file
@return - false, if images still need to be rendered.
			true, id all images were saved to a file.
*/
bool SphereCoordRenderer::draw_sequence(void)
{
	if (_N_current < _N) {
		glm::vec3 p = _points[_N_current];
		glm::vec3 n = _normals[_N_current];
		glm::vec3 eye = n * glm::vec3(_camera_distance);
		glm::mat4 vm = glm::lookAt(eye, _center, _up );
		setCameraMatrix(vm);

		enable_writer(true);
		if(_verbose)
			cout << "[INFO] - Render image " << _N_current << " from pos: " << eye[0] << " : " << eye[1] << " : " << eye[2]<< endl;
		draw_and_save();
		_N_current++;
		
		if (_N_current == _N){
			if(_verbose)
				cout << "[INFO] - DONE - rendered " << _N_current << " sets." <<  endl;
		}
		return false;
	}
	else {
		glm::vec3 p = _points[0];
		glm::vec3 n = _normals[0];
		glm::vec3 eye = n * glm::vec3(_camera_distance);
		glm::mat4 vm = glm::lookAt(eye, _center, _up);

		setCameraMatrix(vm);

		enable_writer(false);
		draw();
		return true;
	}
	
}




void SphereCoordRenderer::make_geometry(glm::vec3 center, double radius, double segments, double rows,
	std::vector<glm::vec3> &spherePoints, std::vector<glm::vec3> &normals)
{
	const double PI = 3.141592653589793238462643383279502884197;
	spherePoints.clear();

	int current_size = 0;

	double theta = 0;
	double phi = 0;
	glm::vec3 point;
	point.x = radius * cos(phi) * sin(theta) + center.x;
	point.y = radius * sin(phi) * sin(theta) + center.y;
	point.z = radius * cos(theta) + center.z;
	spherePoints.push_back(point); 

	glm::vec3  normal;
	normal.x = cos(phi) * sin(theta);
	normal.y = sin(phi) * sin(theta);
	normal.z = cos(theta);
	normals.push_back(normal);



	for (double theta = PI / float(rows); theta < PI ; theta += PI / float(rows)) // Elevation [0, PI]
	{
		//double theta = 1.57;
		double theta2 = theta + PI / float(rows);
		int count = 0;
		int count_row = 0;
		current_size = spherePoints.size();

		// Iterate through phi, theta then convert r,theta,phi to  XYZ
		for (double phi = 0.; phi < 2 * PI; phi += PI / float(segments)) // Azimuth [0, 2PI]
		{


			glm::vec3 point;
			point.x = radius * cos(phi) * sin(theta) + center.x;
			point.y = radius * sin(phi) * sin(theta) + center.y;
			point.z = radius * cos(theta) + center.z;
			spherePoints.push_back(point); count++;


		//	glm::vec3  point2;
		//	point2.x = radius * cos(phi) * sin(theta2) + center.x;
		//	point2.y = radius * sin(phi) * sin(theta2) + center.y;
		//	point2.z = radius * cos(theta2) + center.z;
		//	spherePoints.push_back(point2); count++;

			glm::vec3  normal;
			normal.x = cos(phi) * sin(theta);
			normal.y = sin(phi) * sin(theta);
			normal.z = cos(theta);
			normals.push_back(normal);

		//	glm::vec3  normal2;
		//	normal2.x = cos(phi) * sin(theta2);
		//	normal2.y = sin(phi) * sin(theta2);
		//	normal2.z = cos(theta2);
		//	normals.push_back(normal2);


		}
		if (count_row == 0) count_row = count;
	}

	{

		theta = PI;
		phi = 2 * PI;

		glm::vec3 point;
		point.x = radius * cos(phi) * sin(theta) + center.x;
		point.y = radius * sin(phi) * sin(theta) + center.y;
		point.z = radius * cos(theta) + center.z;
		spherePoints.push_back(point);

		glm::vec3  normal;
		normal.x = cos(phi) * sin(theta);
		normal.y = sin(phi) * sin(theta);
		normal.z = cos(theta);
		normals.push_back(normal);
	}


	return;
}


