#pragma once
#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <memory>
#include <string>
#include "include/glm/ext.hpp"

struct Ray;
struct hitRecord;

struct Material
{
public:
	// Virtual Method for scattering ray r_in and outputing it as ray _scattered
	virtual bool scatter(std::shared_ptr<Ray> r_in, std::shared_ptr<hitRecord> _rec, std::shared_ptr<glm::vec3> _attenuation, std::shared_ptr<Ray> _scattered) { return false; }
	// Virtual method for serializing to a string
	virtual std::string Serialize() { return "INVALID MATERIAL"; }
private:
};

struct Lambertian : public Material
{
public:
	// Constructor that take a vec3 as a colour/albedo value
	Lambertian(glm::vec3 _colour);

	// Method for scattering ray r_in and outputing it as ray _scattered
	bool scatter(std::shared_ptr<Ray> r_in, std::shared_ptr<hitRecord> _rec, std::shared_ptr<glm::vec3> _attenuation, std::shared_ptr<Ray> _scattered);
	// Method for serializing to a string
	std::string Serialize();
private:
	// Stores the colour value
	glm::vec3 albedo;

};

struct Metal : public Material
{
public:
	// Constructor that take a vec3 as a colour/albedo value
	Metal(glm::vec3 _colour);

	// Method for scattering ray r_in and outputing it as ray _scattered
	bool scatter(std::shared_ptr<Ray> r_in, std::shared_ptr<hitRecord> _rec, std::shared_ptr<glm::vec3> _attenuation, std::shared_ptr<Ray> _scattered);
	// Method for serializing to a string
	std::string Serialize();
private:
	// Stores the colour value
	glm::vec3 albedo;
};


#endif // !_MATERIAL_H_

