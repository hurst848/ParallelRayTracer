#include "HittableObject.h"

#include "Ray.h"
#include "Material.h"


// Sets normal and the front face based on incoming ray
void hitRecord::setFaceNormal(std::shared_ptr<Ray> _ray, glm::vec3 _outwardNormal)
{
	frontFace = (glm::dot(_ray->direction, _outwardNormal) < 0);
	normal = frontFace ? _outwardNormal : -_outwardNormal;
}

// DEPRICATED
void hitRecord::copy(std::shared_ptr<hitRecord> _rec)
{
	p = _rec->p;
	normal = _rec->normal;
	frontFace = _rec->frontFace;
	t = _rec->t;
	materialPtr = _rec->materialPtr;
}

// Returns a randon position within a unit sphere
glm::vec3 hitRecord::random_in_unit_sphere() {
	// Loop through unitl a valid vec3 is generated
	while (true) {
		// Create a vec3 with each component randomized
		glm::vec3 p = glm::vec3(glm::linearRand(-1.0f, 1.0f), glm::linearRand(-1.0f, 1.0f), glm::linearRand(-1.0f, 1.0f));
		// Check if the length squared is greated or equal to 1
		if (lengthSquared(p) >= 1) continue;
		return p;
	}
}

// Returns the length squared of a vec 3
float hitRecord::lengthSquared(glm::vec3 _in)
{
	return _in.x * _in.x + _in.y * _in.y + _in.z * _in.z;
}

// Checks if a vec3's magnitude is near zero, returns true if it is
bool hitRecord::nearZero(glm::vec3 _in)
{
	// Define the 'near zero value'
	float s = 1e-8;
	// Check each component of the vector, and return true or false
	return (fabs(_in.x) < s) && (fabs(_in.y) < s) && (fabs(_in.z) < s);
}

glm::vec3 hitRecord::reflect(glm::vec3 _v, glm::vec3 _n)
{
	return _v - 2 * glm::dot(_v, _n) * _n;
}
