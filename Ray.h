#include "include/glm/ext.hpp"

struct Ray
{
public:
	// The starting location of the Ray
	glm::vec3 origin;
	// The direction which the ray is going
	glm::vec3 direction;

	// Empty constructor 
	Ray();
	// Constructor that takes origin and direction as parameters
	Ray(glm::vec3 _origin, glm::vec3 _direction);
	// Returns the position on the ray at a specified distance from the origin
	glm::vec3 at(float _distance);
	// Returns the length squared of a vec3
	float lengthSquared(glm::vec3 _vec);
};