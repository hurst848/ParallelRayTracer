#include "HittableScene.h"

// Clears the objects vector of all hittableObjects
void HittableScene::clear() { objects.clear(); }

// Method for checking intersection with ray, retrun true if a hit occurs
bool HittableScene::hit(std::shared_ptr<Ray> _ray, float t_min, float t_max, std::shared_ptr<hitRecord> _rec)
{
	// Create a hitRecord
	std::shared_ptr<hitRecord> tmpRecord = std::make_shared<hitRecord>();
	bool hitAnything = false;
	// Stores the value of the distance to the closest object that the ray hits
	float closestHit = t_max;

	// Loop through all objects and check if they 
	for (int i = 0; i < objects.size(); i++)
	{
		// If the ray intersects with an object
		if (objects.at(i)->hit(_ray, t_min, closestHit, tmpRecord))
		{
			hitAnything = true;
			closestHit = tmpRecord->t;
			// Copy the data of the hit so it represents the closest hit
			_rec->copy(tmpRecord);
		}
	}

	return hitAnything;
}

// Method for serializing to a string
std::string HittableScene::Serialize()
{
	std::string rtrn;
	rtrn += "~Scene: \n";
	// Loop through all objects and call their serialize method
	for (int i = 0; i < objects.size(); i++)
	{
		rtrn += objects.at(i)->Serialize();
	}
	return rtrn;
}