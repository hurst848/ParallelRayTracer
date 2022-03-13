#pragma once
#include "include/glm/ext.hpp"

#include <mutex>
#include <memory>
#include <string>
#include <vector>

struct Frame;
struct HittableScene;
struct Ray;
struct Camera;
struct JobController;
struct Screen;
//struct RenderServer;

struct Job
{
public:
	// Renders prameter _job and request more from _controller until complete
	static void ProcessJob(std::shared_ptr<Job> _job, std::shared_ptr<JobController> _controller);

	// Converts the rendered colour value to something the screen can display
	glm::vec3 WriteColour(glm::vec3 _colour, int _samples);
	// Gets the colour of the pixel the ray is hitting
	glm::vec3 GetRayColour(std::shared_ptr<Ray> _ray, int _depth);

	// A vector containg all coordinates to render
	std::vector<glm::ivec2> jobCoords;

	// Store the scene to be rendered
	std::shared_ptr<HittableScene> scene;
	// Stores the camera currently in use to be used for rendering
	std::shared_ptr<Camera> camera;
	// Stores the frame to push the render to
	std::shared_ptr<Frame> frame;

	// Stores the id of the job, used for locking the controller
	int id;
	
};

struct JobController
{
	friend struct MutexRenderer;
public:
	// Locks the controller to be only used with a job with value ID
	bool Lock(int _ID);
	// Unlocks the controller so other jobs can acces it
	bool Unlock();

	// Generates vector of jobs storred in jobs
	void GenerateJobs();

	// Pushes a vector of rendered pixels to frame given a vector of colours 
	// generated and the coordinates of the pixels
	void PushPixelData(std::vector<glm::vec3> _colours, std::vector<glm::ivec2> _coords);
	// Pushes rendered pixel to frame given the colour generated and the coordinates of the pixel
	void PushPixelData(glm::vec3 _colour, glm::ivec2 _coord);
	// Returns a complete Job that has not yet been rendered
	Job RequestJob();
	// Stores the ID of the job that currently has locked the controller (-1 if not locked)
	int lockedID = -1;
	// Stores how far along the jobs vector is complete
	int itr = 0;

	// The number of samples for anti-alliasing
	int samples = 100;
	// The max number of scatters that can be done
	int depth = 50;
	// Stores the pixel dimentions of the job
	glm::ivec2 jobSize = glm::ivec2(10, 10);

private:
	// Stores whether or not the controller is in a locked state or not
	bool locked;
	// Stores all of the jobs to complete
	std::vector<Job> jobs;
	
	// Store the scene to be rendered
	std::shared_ptr<HittableScene> scene;
	// Stores the camera currently in use to be used for rendering
	std::shared_ptr<Camera> camera;
	// Stores the frame to push the render to
	std::shared_ptr<Frame> frame;
};


struct MutexRenderer
{
public:
	// Empty constructor
	MutexRenderer() {};
	// Constructor that take a scene, camera and frame as parameters
	MutexRenderer(std::shared_ptr<HittableScene> _scene, std::shared_ptr<Camera> _camera, std::shared_ptr<Frame> _frame);

	// For local use //
	// Renders the scene using a single thread
	void RenderScene();
	// Renders the secne using _threads number of threads
	void RenderScene(int _threads);
	// Sets the samples per pixel value
	void setSamples(int _samples);
	// Sets the Scatter depth
	void setDepth(int _depth);
	// Sets the size of the job
	void setJobSize(glm::ivec2 _size);
	// loads scene and associated from a file
	void loadRenderFromFile(std::shared_ptr<Frame> _frame, std::shared_ptr<Screen> _screen,
		std::shared_ptr<HittableScene> _scene, std::shared_ptr<Camera> _camera, std::string _path);
	// for networking use //
	// Serializes the scene into a string for sending over the newtwork
	void Serialize();
	//void NetworkRender();

	// for display use //
	// Clears the frame currently being displayed
	void ClearFrame();
	// Stores the serialized representation of the scene
	std::string serialized;
private:
	
	//std::shared_ptr<RenderServer> server;
	// Stores the controller for job alloctaion
	std::shared_ptr<JobController> controller;
	// Store the scene to be rendered
	std::shared_ptr<HittableScene> scene;
	// Stores the camera currently in use to be used for rendering
	std::shared_ptr<Camera> camera;
	// Stores the frame to push the render to
	std::shared_ptr<Frame> frame;
	
	// Reads a glm::vec3 from a formatted string
	glm::vec3 readVector(std::string _vectorText);
	// Reads a glm::ivec2 from a formatted string
	glm::ivec2 readIvec2(std::string _input);

};