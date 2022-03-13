#ifndef _HITTABLE_SCENE_H_
#define _HITTABLE_SCENE_H_

#include "HittableObject.h"

#include <vector>
#include <memory>
#include <string>


struct HittableScene : public hittableObject
{
	friend struct MutexRenderer;
public:
	// Clears the objects vector of all hittableObjects
	void clear();

	// Add a child of the hittableObject struct to the objects vector
	template <typename T>
	void add(std::shared_ptr<T> _hittableObject)
	{
		// Push _hittableObject to back of objects
		objects.push_back(_hittableObject);
	}

	// Method for checking intersection with ray, retrun true if a hit occurs
	bool hit(std::shared_ptr<Ray> _ray, float t_min, float t_max, std::shared_ptr<hitRecord> _rec);
	// Method for serializing to a string
	std::string Serialize();

private:
	// Vector for storing all hittableObject structs
	std::vector<std::shared_ptr<hittableObject>> objects;
};

#endif // !_HITTABLE_SCENE_H_


