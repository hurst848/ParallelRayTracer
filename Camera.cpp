#include "Camera.h"

#include "Ray.h"

// Empty constructor
Camera::Camera()
{
	float aspectRatio = 16.0f / 9.0f;
	float viewportHeight = 2.0f;
	float viewportWidth = aspectRatio * viewportHeight;
	float focalLength = 1.0f;

	origin = glm::vec3(0.0f, 0.0f, 0.0f);
	horizontal = glm::vec3(viewportWidth, 0.0f, 0.0f);
	vertical = glm::vec3(0.0f, viewportHeight, 0.0f);
	lowerLeftCorner = origin - (horizontal / 2.0f) - (vertical / 2.0f) - glm::vec3(0.0f, 0.0f, focalLength);
}

// Constructor with screen resoluiton as input
Camera::Camera(glm::ivec2 _res)
{
	float aspectRatio = (float)_res.x / (float)_res.y;
	float viewportHeight = 2.0f;
	float viewportWidth = aspectRatio * viewportHeight;
	float focalLength = 1.0f;

	origin = glm::vec3(0.0f, 0.0f, 0.0f);
	horizontal = glm::vec3(viewportWidth, 0.0f, 0.0f);
	vertical = glm::vec3(0.0f, viewportHeight, 0.0f);
	lowerLeftCorner = origin - (horizontal / 2.0f) - (vertical / 2.0f) - glm::vec3(0.0f, 0.0f, focalLength);
}

// Casts ray from camera
Ray Camera::getRay(float _u, float _v)
{
	// Cast ray from origin of camera to specified spot in viewport
	return Ray(origin, lowerLeftCorner + (_u * horizontal) + (_v * vertical) - origin);
}