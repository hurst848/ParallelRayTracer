#include "Material.h"

#include "Ray.h"
#include "HittableObject.h"

// Constructor that take a vec3 as a colour/albedo value
Lambertian::Lambertian(glm::vec3 _colour)
{
	albedo = _colour;
}

// Method for scattering ray r_in and outputing it as ray _scattered
bool Lambertian::scatter(std::shared_ptr<Ray> r_in, std::shared_ptr<hitRecord> _rec, std::shared_ptr<glm::vec3> _attenuation, std::shared_ptr<Ray> _scattered)
{
	// Creates a random direction of the scatter 
	glm::vec3 scatterDirection = _rec->normal + glm::normalize(_rec->random_in_unit_sphere());

	// Checks to see if the scatterd ray is valid (not zero)
	if (_rec->nearZero(scatterDirection))
	{
		scatterDirection = _rec->normal;
	}

	// Set return values
	*_scattered = Ray(_rec->p, scatterDirection);
	*_attenuation = albedo;

	// Always returns true
	return true;
	
}

// Method for serializing to a string
std::string Lambertian::Serialize()
{
	std::string rtrn;
	// Convert the albedo in to string and return
	rtrn += "~Lambertian: \n" +
		std::to_string(albedo.x) + ", " + std::to_string(albedo.y) + ", " + std::to_string(albedo.z);
	return rtrn;
}

// Constructor that take a vec3 as a colour/albedo value
Metal::Metal(glm::vec3 _colour)
{
	albedo = _colour;
}

// Method for scattering ray r_in and outputing it as ray _scattered
bool Metal::scatter(std::shared_ptr<Ray> r_in, std::shared_ptr<hitRecord> _rec, std::shared_ptr<glm::vec3> _attenuation, std::shared_ptr<Ray> _scattered)
{
	// Create vec3 that would be a reflection of the incoming ray
	glm::vec3 reflected = _rec->reflect(glm::normalize(r_in->direction), _rec->normal);
	// Set return values 
	*_scattered = Ray(_rec->p, reflected);
	*_attenuation = albedo;
	
	// Return whether or not the scattered ray was valid (actually visible)
	return (glm::dot(_scattered->direction, _rec->normal) > 0);
}

// Method for serializing to a string
std::string Metal::Serialize()
{
	std::string rtrn;
	// Convert the albedo in to string and return
	rtrn += "~Metal: \n" +
		std::to_string(albedo.x) + ", " + std::to_string(albedo.y) + ", " + std::to_string(albedo.z);
	return rtrn;
}