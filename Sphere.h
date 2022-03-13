#include "HittableObject.h"

#include "include/glm/ext.hpp"

#include <memory>
#include <string>
struct Ray;
struct Material;

struct Sphere : public hittableObject
{
	friend struct MutexRenderer;
public:
	// Empty Constructor
	Sphere();

	// Constructor that take center, radius and a material as parameters
	template <typename T>
	Sphere(glm::vec3 _centre, float _radius, std::shared_ptr<T> _mat)
	{
		radius = _radius;
		centre = _centre;
		materialPtr = _mat;
	}
	// Method for checking intersection with ray, retrun true if a hit occurs
	bool hit(std::shared_ptr<Ray> _ray, float t_min, float t_max, std::shared_ptr<hitRecord> _rec);


private:
	// Stores the radius of the sphere
	float radius;
	// Stores the centre / location of the sphere
	glm::vec3 centre;
	// Stores the material of the sphere
	std::shared_ptr<Material> materialPtr;
	// Method for serializing to a string
	std::string Serialize();
};