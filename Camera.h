#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "include/glm/ext.hpp"

struct Ray;

struct Camera
{
	friend struct MutexRenderer;
public:
	// Empty constructor
	Camera();
	// Constructor with screen resoluiton as input
	Camera(glm::ivec2 _res);

	// Casts ray from camera
	Ray getRay(float _u, float _v);

private:
	// Location of the camera
	glm::vec3 origin;
	// Lower left corner of the viewport
	glm::vec3 lowerLeftCorner;
	// Works as viewpoint 'x' 
	glm::vec3 horizontal;
	// Works as viewponit 'y'
	glm::vec3 vertical;
};

#endif // !_CAMERA_H_