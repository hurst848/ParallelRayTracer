#include "Sphere.h" 

#include "Ray.h"
#include "Material.h"

// Empty Constructor
Sphere::Sphere()
{
	radius = 0;
	centre = glm::vec3(0, 0, 0);
}

// Method for serializing to a string
std::string Sphere::Serialize()
{
	std::string rtrn;
	// Convert centre and radius to string and serialize the material
	rtrn += "~Sphere: \n" +
		std::to_string(radius) + "\n" +
		std::to_string(centre.x) + ", " + std::to_string(centre.y) + ", " + std::to_string(centre.z) + "\n" +
		materialPtr->Serialize() + "\n";
	return rtrn;
}

// Method for checking intersection with ray, retrun true if a hit occurs
bool Sphere::hit(std::shared_ptr<Ray> _ray, float t_min, float t_max, std::shared_ptr<hitRecord> _rec)
{
	// Complete ray sphere interesection and update values of _rec
	glm::vec3 oc = _ray->origin - centre;
	float a = _ray->lengthSquared(_ray->direction);
	float b = glm::dot(oc, _ray->direction);
	float c = _ray->lengthSquared(oc) - (radius * radius);

	float discriminant = (b * b) - (a * c);

	if (discriminant < 0) { return false; }
	float sqrtDiscriminant = glm::sqrt(discriminant);

	float root = (-b - sqrtDiscriminant) / a;
	if (root < t_min || root > t_max)
	{
		root = (-b + sqrtDiscriminant) / a;
		if (root < t_min || root > t_max)
		{
			return false;
		}
	}

	// Set calculated values to the hitRecord _rec
	_rec->t = root;
	_rec->p = _ray->at(_rec->t);
	glm::vec3 outwardNormal = (_rec->p - centre) / radius;
	_rec->setFaceNormal(_ray, outwardNormal);
	_rec->materialPtr = materialPtr;

	return true;
}