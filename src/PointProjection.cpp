#include "PointProjection.h"


/*
Constructor
@param image_width, image_height - the width and height of the output image/rendered image in pixel. 
*/
PointProjection::PointProjection(int image_width, int image_height):
	_img_width(image_width), _img_height(image_height)
{
	_view_matrix = glm::mat4(1.0);
	_proj_matrix = glm::mat4(1.0);

	_verbose = false;
}


PointProjection::~PointProjection()
{

}


/*!
Set the opengl view matrix
@param view_matrix - an opengl 4x4 view matrix.
*/
void PointProjection::setViewMatrix(glm::mat4 view_matrix)
{
	_view_matrix = view_matrix;
}
	
/*!
Set the opengl projection matrix. 
@param proj_matrix - a 4x4 opengl projection matrix. 
*/
void PointProjection::setProjectionMatrix(glm::mat4 proj_matrix)
{
	_proj_matrix = proj_matrix;
}


/*
Project a set of 3D points to a 2D plane with the given view matrix and projecton matrix. 
@param points - a vector with N points in x, y, z, coordinates. 
@param matrix - a 4x4 transformation matrix to move the points.
@param projected_points - the projected output points in u, v coordinate in image coordinate space. 
*/
void PointProjection::projectPoints(const std::vector<glm::vec3> points, const glm::mat4 matrix, std::vector<glm::vec2>& projected_points)
{

	int N = points.size();
	projected_points.clear();
	projected_points.reserve(N);
	_projected_points.clear();
	_projected_points.reserve(N);

	for each (glm::vec3 p in points)
	{
		glm::vec4 pGlobal(p.x, p.y, p.z, 1.0);
		
		// projection
		glm::vec4 pImg = _proj_matrix * _view_matrix * matrix * pGlobal;

		// normalize
		pImg.x = pImg.x / pImg.w;
		pImg.y = pImg.y / pImg.w;
		pImg.z = pImg.z / pImg.w;

		// convert to image coordinates
		float u = ((pImg.x * _img_width ) + _img_width)/2.0;
		float v = ((- pImg.y * _img_height ) + _img_height)/2.0;

		// check limits
		u = (std::min)( float(_img_width+1), (std::max)(-1.0f, u) );
		v = (std::min)( float(_img_height+1), (std::max)(-1.0f, v) );

		// mark as -1 if point is out of image. 
		if (u == -1.0 || u == float(_img_width + 1) || v == -1.0 || v == float(_img_height + 1)) {
			u = -1.0;
			v = -1.0;
		}

		//projected_points.push_back(glm::vec2(u,v));
		_projected_points.push_back(glm::vec2(u,v));

		if(_verbose)
			std::cout << "Point " << p.x << ", " << p.y << ", " << p.z << " -> " << pImg.x << ", " << pImg.y << ", " << pImg.z << " -> " << u << ", " << v << std::endl;
	}

	projected_points = _projected_points;

	if(_verbose)
		std::cout << "------\n";
}


/*
Enable the debug window showing the points. 
*/
void PointProjection::setVerbose(bool verbose)
{
	_verbose = verbose;
}


/*!
Render an opencv image that shows the LAST projected points. 
*/
void PointProjection::showProjection(void)
{
#ifdef _WITH_OPENCV
	cv::Mat img = cv::Mat::zeros(_img_height, _img_width, CV_8UC3);
	std::vector<cv::Point2f> points;

	for each (glm::vec2 p in _projected_points) {
		cv::Point2f cvp(p.x, p.y);
		cv::circle(img, cvp, 3, cv::Scalar(0.0, 255.0, 255.0), 4);
	}
	cv::Mat img_out;
	cv::resize(img, img_out, cv::Size(512, 512));
	cv::imshow("Projected Keypoints", img_out);


#endif
}