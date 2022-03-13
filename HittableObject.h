#pragma once
#ifndef _HITTABLE_H_
#define _HITTABLE_H_

#include "include/glm/ext.hpp"

#include <memory>
#include <string>

struct Ray;
struct Material;

struct hitRecord
{
public:

	// Position along the ray 
	glm::vec3 p;
	// The calculated normal of the hit
	glm::vec3 normal;
	float t;
	// Depicts whether the hit is facing towards the camera or not
	bool frontFace;
	// Pointer to the type of materail the ray has hit
	std::shared_ptr<Material> materialPtr;
	// Sets normal based on incoming ray
	void setFaceNormal(std::shared_ptr<Ray> _ray, glm::vec3 _outwardNormal);
	
	// DEPRICATED
	void copy(std::shared_ptr<hitRecord> _rec);
	
	// Utility functions for maths //
	// Returns a randon position within a unit sphere
	glm::vec3 random_in_unit_sphere();
	// Returns the length squared of a vec 3
	float lengthSquared(glm::vec3 _in);
	// Checks if a vec3's magnitude is near zero, returns true if it is
	bool nearZero(glm::vec3 _in);

	glm::vec3 reflect(glm::vec3 _v, glm::vec3 _n);
	

private:
};

struct hittableObject
{

public:
	// Method for checking intersection with ray, retrun true if a hit occurs
	virtual bool hit(std::shared_ptr<Ray> _ray, float t_min, float t_max, std::shared_ptr<hitRecord> _rec) { return false; }
	// Virtual method for serializing to a string
	virtual std::string Serialize() { return "HITTABLE_OBJECT"; }
};


#endif // !_HITTABLE_H_
