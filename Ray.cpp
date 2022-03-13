#include "Ray.h"

// Empty constructor
Ray::Ray()
{
	direction = glm::vec3(0, 0, 0);
	origin = glm::vec3(0, 0, 0);
}

// Constructor that takes origin and direction as parameters
Ray::Ray(glm::vec3 _origin, glm::vec3 _direction)
{
	direction = _direction;
	origin = _origin;
}

// Returns the position on the ray at a specified distance from the origin
glm::vec3 Ray::at(float _distance)
{
	return origin + (_distance * direction);
}

// Returns the length squared of a vec3
float Ray::lengthSquared(glm::vec3 _in)
{
	return _in.x * _in.x + _in.y * _in.y + _in.z * _in.z;
}