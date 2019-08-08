#include "RandomPoseViewRenderer.h"


/*
Constructor
@param - window_width, window_height - width and height of the opengl, glfw output image in pixel
@param - image_width, image_height - width and height of the image to be rendered and saved to a file.
*/
RandomPoseViewRenderer::RandomPoseViewRenderer(int window_width, int window_height, int image_width, int image_height) :
	ModelRenderer(window_width, window_height, image_width, image_height)
{
	_segments = 30;
	_rows = 45;
	_radius = 1.0;
	_center = glm::vec3(0.f,0.f,0.f);
	_up = glm::vec3(0.0f, 1.0f, 0.0f);
	_N = 0;
	_N_current = 0;
	_camera_distance = 1.2;
	_upper_hemisphere = true;
	_subdivisions = 0;

	_lim_px = 2;
	_lim_nx = -2;
	_lim_py = -2;
	_lim_ny = 2;
	_lim_pz = -3;
	_lim_nz = -1;
}

RandomPoseViewRenderer::~RandomPoseViewRenderer()
{

}



/*
Set the number of subdivisions for the polyheder
The polyheder points define the camera view points. 
@param subdivisions - number of subdivisions. 0 is an Icosahedron
*/
void RandomPoseViewRenderer::create(int num_images ,  int subdivisions)
{

	if(_verbose)
		cout << "[INFO] - Created Polyhedron."  << endl;

	if (subdivisions >= 0) {
		_subdivisions = subdivisions;
	}
	else {
		cout << "[ERROR] - The minimum number for subdivisions is 0." << endl;
		_subdivisions = 0;
	}

	if (num_images > 0) {
		_N = num_images;
		if (_N > 100000) {
			cout << "[WARNING] - Note that rendering "  << _N << " images will take a while."<< endl;
		}
	} else{
		cout << "[ERROR] - The minimum number of images to generate is 1." << endl;
		_subdivisions = 0;
	}
		

	
	_points.clear();
	_normals.clear();

	// generates the orientations
	std::pair< vector<glm::vec3>, vector<glm::ivec3> > final_mesh;
	final_mesh  = arlab::PolyhedronGeometry::Create(_subdivisions);
	_points = final_mesh.first;
	_normals = final_mesh.first;


	// filter all poses to only get the upright hemisphere.
	// up is y.

	if (_upper_hemisphere) {
		for (int i = 0; i < _points.size(); i++) {
			glm::vec3 p = _points[i];
			if (p.y < 0) {
				_points.erase(_points.begin() + i);
				_normals.erase(_normals.begin() + i);
				i--;
			}
		}
	}



	
	if(_verbose)
		cout << "[INFO] - Created " << _N << " coordinates around a sphere." << endl;

	return;
}


/*
Set the limits for the random pose in camera coordinates
Camera coordinates:  view direction: -z,  up: y, side: x (to the right in image coordinates).
@param nx, px - x-axis limits
@param ny, py - y-axis limits 
@param nz, pz - z-axis limits. Note that the camera looks into the negative z-direction.
*/
void RandomPoseViewRenderer::setPoseLimits(float nx, float px, float ny, float py, float nz, float pz)
{
	_lim_px = px;
	_lim_nx = nx;
	_lim_py = py;
	_lim_ny = ny;
	_lim_pz = min(pz, -1.0);
	_lim_nz = min(nz, -0.5f);

	if (px < nx) {
		_lim_px = nx;
		_lim_nx = px;
		cout << "[WARNING] - Positive x limit < negative x limit. Flipped values." << endl;
	}
	if (py < ny) {
		_lim_py = ny;
		_lim_ny = py;
		cout << "[WARNING] - Positive y limit < negative y limit. Flipped values." << endl;
	}
	if (pz < nz) {
		_lim_pz = nz;
		_lim_nz = pz;
		cout << "[WARNING] - Positive z limit < negative z limit. Flipped values." << endl;
	}
}


/*
Limits the orientation to upper hemisphere orientations only.
Note that up means positive y direction
@param upright - true to limit the orientations to upright poses. 
*/
void RandomPoseViewRenderer::setHemisphere(bool upright)
{
	_upper_hemisphere = upright;
}

/*
Draw the image sequence and save all images to a file
@return - false, if images still need to be rendered.
			true, id all images were saved to a file.
*/
bool RandomPoseViewRenderer::draw_sequence(void)
{
	if (_N_current < _N) {

		glm::mat4 pose = getRandomPosition();
		setCameraMatrix(pose);

		enable_writer(true);
		
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


/*
Returns a random pose 
*/
glm::mat4  RandomPoseViewRenderer::getRandomPosition(void)
{

	// orientation
	static int num_points = _points.size();
	std::random_device int_generator;
    std::uniform_int_distribution<int> distribution(0,num_points-1);
    int dice_index = distribution(int_generator); 

	glm::vec3 p = _points[dice_index];
	glm::vec3 n = _normals[dice_index];
	glm::vec3 eye = n * glm::vec3(_camera_distance);


	// This prevents that the view matrix becomes NaN when eye and up vector align. 
	float ang = glm::dot(glm::normalize(eye), _up);
	float s = 1.0;
	if (ang < 0) s = -1.0;
	if (abs(ang) > 0.999)
		_up = glm::vec3(0.0, 0.0, -s);
	else
		_up = glm::vec3(0.0, 1.0, 0.0);


	glm::mat4 vm = glm::lookAt(eye, _center, _up );


	// position
	std::mt19937 generator(_rd());
    std::uniform_real_distribution<float> distribution_x(_lim_nx,_lim_px);
	std::uniform_real_distribution<float> distribution_y(_lim_ny,_lim_py);
	std::uniform_real_distribution<float> distribution_z(_lim_nz,_lim_pz);
    float x = distribution_x(generator); 
	float y = distribution_y(generator); 
	float z = distribution_z(generator); 

	// subtract the camera distance to account for the camera at location _camera_distance
	glm::mat4 pose = glm::translate(glm::vec3(x, y, z+_camera_distance));

	if (_verbose) {
		std::cout << std::fixed;
		std::cout << std::setprecision(2);
		cout << "[INFO] - Render image " << _N_current << " from pos: (" << eye[0] << ", " << eye[1] << ", " << eye[2] << "), with translation (" << x << ", " << y << ", " << z << ")." << endl;
	}
	else
	{
		if (_N_current % 100 == 0 && _N_current > 0) {
			std::cout << ". ";
		}
		if (_N_current % 1000 == 0 && _N_current > 0) {
			std::cout << _N_current << "/" << _N << std::endl;
		}
	}
	return  pose * vm;
	
}


