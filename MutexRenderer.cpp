#include "MutexRenderer.h"

#include "Camera.h"
#include "Frame.h"
#include "HittableScene.h"
#include "Ray.h"
#include "Material.h"
#include "Sphere.h"
#include "Screen.h"

#include <chrono>
#include <fstream>
#include <iostream>

// Constructor that take a scene, camera and frame as parameters
MutexRenderer::MutexRenderer(std::shared_ptr<HittableScene> _scene, std::shared_ptr<Camera> _camera, std::shared_ptr<Frame> _frame)
{
    // Assign MutexRender values
    scene = _scene;
    camera = _camera;
    frame = _frame;

    // Create controller and assign relevant values
    controller = std::make_shared<JobController>();
    
    controller->scene = scene;
    controller->camera = camera;
    controller->frame = frame;

    //server = std::make_shared<RenderServer>();
    
}

//void MutexRenderer::NetworkRender()
//{
//    Serialize();
//    controller->GenerateJobs();
//    server->StartServer(serialized,controller);
//}

// Serializes the scene into a string for sending over the newtwork
void MutexRenderer::Serialize()
{
    // Serialized header
    serialized = "//// SCENE ////\n";
    // Serialize frame resolution
    serialized += "~res:\n" + 
        std::to_string(frame->getResolution().x) + ", " + std::to_string(frame->getResolution().y) + "\n";

    // serialize camera
    serialized += "~camera:\n" +
        std::to_string(camera->horizontal.x) + ", " + std::to_string(camera->horizontal.y) + ", " + std::to_string(camera->horizontal.z) + "\n" +
        std::to_string(camera->lowerLeftCorner.x) + ", " + std::to_string(camera->lowerLeftCorner.y) + ", " + std::to_string(camera->lowerLeftCorner.z) + "\n" +
        std::to_string(camera->origin.x) + ", " + std::to_string(camera->origin.y) + ", " + std::to_string(camera->origin.z) + "\n" +
        std::to_string(camera->vertical.x) + ", " + std::to_string(camera->vertical.y) + ", " + std::to_string(camera->vertical.z) + "\n";

    // Serialize scene
    serialized += scene->Serialize();
    
}

// Renders prameter _job and request more from _controller until complete
void Job::ProcessJob(std::shared_ptr<Job> _job, std::shared_ptr<JobController> _controller)
{
    bool completed = false;
    // Grab resolution values from the frame to limit accesing the memory
    int width = _job->frame->getResolution().x;
    int height = _job->frame->getResolution().y;
    // Loop unil no more jobs are avalible
    while (!completed)
    {
        // stores vecotor of colours generated to push to the controller
        std::vector<glm::vec3> coloursGenerated;

        // Loop through all coordinates stored in current job
        for (int i = 0; i < _job->jobCoords.size(); i++)
        {
            // Create the vec3 to store the output colour
            glm::vec3 pixelColour(0.0f, 0.0f, 0.0f);
            // Loop an amount of times for antialliasing
            for (int s = 0; s < _controller->samples; s++)
            {
                // Generate the u and v values and add a random offset for affecting AA bluring
                float u = ((float)_job->jobCoords.at(i).x + glm::linearRand(0.0f, 1.0f)) / (float)(width - 1);
                float v = ((float)_job->jobCoords.at(i).y + glm::linearRand(0.0f, 1.0f)) / (float)(height - 1);

                // Create ray from camear to generate u v coordinates
                std::shared_ptr<Ray> ray = std::make_shared<Ray>(_job->camera->getRay(u, v));

                // Calculate the colour at the ray's intersection
                glm::vec3 colour = _job->GetRayColour(ray, _controller->depth);

                // Add the generated colour to pixelColour
                pixelColour += colour;
            }
            // Convert the rendered colour to something the sceen can use
            glm::vec3 newColour = _job->WriteColour(pixelColour, _controller->samples);
            // Push converted colour to the vector of generated colours
            coloursGenerated.push_back(newColour);
        }

        // Loop until the controller is locked
        while (true)
        {
            // Attempt to lock the controller
            if (_controller->Lock(_job->id))
            {
				// Check if locked
				if (_job->id == _controller->lockedID)
				{
                    // Push colour data to the screen
					_controller->PushPixelData(coloursGenerated, _job->jobCoords);

                    // Check if locked
					if (_job->id == _controller->lockedID)
					{
                        // Request new coordinates to render
						_job->jobCoords = _controller->RequestJob().jobCoords;

                        // Check if the recived vector has a size of 0, if it does, there are no new jobs
						if (_job->jobCoords.size() == 0)
						{
                            // Exit render loop
							completed = true;
						}

                        // Unlock the controller for other threads
						_controller->Unlock();
						break;
					}
					else
					{
						// Re-loop and wait for opening
					}
				}
				else
				{
					// Re-loop and wait for opening
				}
            }
            else
            {
                // Re-loop and wait for opening
            }
        }

       
    }
}

// Returns a complete Job that has not yet been rendered
void JobController::GenerateJobs()
{
    // Get the resolution from the frame to limit memory accesess
    int width = frame->getResolution().x;
    int height = frame->getResolution().y;

    // Specifit the dimetions of the job (how mant coordinates in the job)
    glm::ivec2 jobDimentions = jobSize;

    // Calculate how many jobs wide and tall the frame is
    int numJobs_x = width / jobDimentions.x; 
    int numJobs_y = height / jobDimentions.y;
    
    // Loop through all possible jobs and create them
    for (int i = 0; i < numJobs_x; i++)
    {
        for (int j = 0; j < numJobs_y; j++)
        {
            // Create the job and assing the corresponding varibles
            Job tmp;
            tmp.camera = camera;
            tmp.scene = scene;
            tmp.frame = frame;

            // Loop throught the job dimentions offset by the numJob coordinate to populate the jobCorrds vector
            for (int x = 0; x < jobDimentions.x; x++)
            {
                for (int y = 0; y < jobDimentions.y; y++)
                {
                    tmp.jobCoords.push_back(glm::ivec2(i*jobDimentions.x + x, j * jobDimentions.y + y));
                }
            }
            // Add the job to the jobs vector
            jobs.push_back(tmp);
        }
    }

}

// Converts the rendered colour value to something the screen can display
glm::vec3 Job::WriteColour(glm::vec3 _colour, int _samples)
{
    // Break the _colour value into component floats
    float r = _colour.x;
    float g = _colour.y;
    float b = _colour.z;

    // Divide by number of AA samples to get back to a valid displayable colour
    float scale = 1.0f / _samples;
    r = glm::sqrt(r * scale);
    g = glm::sqrt(g * scale);
    b = glm::sqrt(b * scale);

    // convert from 0-255 to 0-1 values
    glm::ivec3 out(static_cast<int>(256 * glm::clamp(r, 0.0f, 0.999f)),
        static_cast<int>(256 * glm::clamp(g, 0.0f, 0.999f)),
        static_cast<int>(256 * glm::clamp(b, 0.0f, 0.999f))
    );

    // return converted colour vector
    return glm::vec3(out.x, out.y, out.z);
}

// Gets the colour of the pixel the ray is hitting
glm::vec3 Job::GetRayColour(std::shared_ptr<Ray> _ray, int _depth)
{
    // Create a hitRecord to store the intersction data
    std::shared_ptr<hitRecord> hits = std::make_shared<hitRecord>();

    // If the depth is 0, then return black as no more scatters can occurs
    // (simulates the gradual diffusion/loss of light with each scattering)
    if (_depth <= 0){ return glm::vec3(0.0f, 0.0f, 0.0f); }

    // Check if _ray intersects with an object
    if (scene->hit(_ray, 0.001f, std::numeric_limits<float>::max(), hits))
    {
        // Create a ray to store the scattered ray
        std::shared_ptr<Ray> scattered = std::make_shared<Ray>();
        // Create a vec3 to store the attenuation (colour) of the scatter
        std::shared_ptr<glm::vec3> attenuation = std::make_shared<glm::vec3>(0.0f, 0.0f, 0.0f);

        // If it scatters, recusively GetRayColour unitl depth is 0
        if (hits->materialPtr->scatter(_ray, hits, attenuation, scattered))
        {
            return glm::vec3(*attenuation * GetRayColour(scattered, _depth - 1));
        }
        else
        {
            return glm::vec3(0.0f, 0.0f, 0.0f);
        }

    }
    // Normalize the direction of the ray
    glm::vec3 unitDirection = glm::normalize(_ray->direction);
    // Calculate and return the colour
    float t = 0.5f * (unitDirection.y + 1.0f);
    return (1.0f - t) * glm::vec3(1.0f, 1.0f, 1.0f) + (t * glm::vec3(0.5f, 0.7f, 1.0f));
}

// Locks the controller to be only used with a job with value ID
bool JobController::Lock(int _ID)
{
    // If the controller is already locked return false
    if (locked)
    {
        return false;
    }
    else
    {
        // Lock the controller and set lockedID to _ID
        locked = true;
		lockedID = _ID;
        return true;
    }
}

// Unlocks the controller so other jobs can acces it
bool JobController::Unlock()
{
    // Reset lockedID to -1 and unlock the controller
	lockedID = -1;
    locked = false;
    return true;
}

// Pushes a vector of rendered pixels to frame given a vector of colours 
// generated and the coordinates of the pixels
void JobController::PushPixelData(std::vector<glm::vec3> _colours, std::vector<glm::ivec2> _coords)
{
    // Loop through the vectors and push according data to the frame
    for (int i = 0; i < _colours.size(); i++)
    {
        frame->writeData(_coords.at(i), _colours.at(i));
    }
}

// Pushes rendered pixel to frame given the colour generated and the coordinates of the pixel
void JobController::PushPixelData(glm::vec3 _colour, glm::ivec2 _coord)
{
    frame->writeData(_coord, _colour);
}

// Returns a complete Job that has not yet been rendered
Job JobController::RequestJob()
{
    // If itr = the amount of jobs, return an empty job
    if (itr >= jobs.size())
    {
        return Job();
    }
    else
    {
        // Get next job in queue and increment itr
        Job rtrn = jobs.at(itr);
        itr++;

        return rtrn;
    }
}

// Renders the scene using a single thread
void MutexRenderer::RenderScene()
{
    controller->GenerateJobs();
    Job::ProcessJob(std::make_shared<Job>(controller->RequestJob()), controller);
}

// Renders the secne using _threads number of threads
void MutexRenderer::RenderScene(int _threads)
{
    controller->GenerateJobs();
    std::vector<std::thread> renderingThreads;

    // Create and start all the rendering threads and assign their first jobs
    for (int i = 0; i < _threads; i++)
    {
		std::shared_ptr<Job> tmp = std::make_shared<Job>(controller->RequestJob());
		tmp->id = i;
        renderingThreads.push_back(std::thread(Job::ProcessJob, tmp , controller));
    }
    // Wait to join all the rendering threads
    for (int i = 0; i < renderingThreads.size(); i++)
    {
        renderingThreads.at(i).join();
    }
}

// Clears the frame currently being displayed
void MutexRenderer::ClearFrame()
{
    frame->purge();
}

// Sets the samples per pixel value
void MutexRenderer::setSamples(int _samples) { controller->samples = _samples; }

// Sets the Scatter depth
void MutexRenderer::setDepth(int _depth) { controller->depth = _depth; }

// Sets the size of the job
void MutexRenderer::setJobSize(glm::ivec2 _size) { controller->jobSize = _size; }

// loads scene and associated from a file
void MutexRenderer::loadRenderFromFile(std::shared_ptr<Frame> _frame, std::shared_ptr<Screen> _screen,
    std::shared_ptr<HittableScene> _scene, std::shared_ptr<Camera> _camera, std::string _path)
{
    // Read in and validate scene file
    glm::ivec2 resolution = glm::ivec2(0, 0);

    int operation = 0;
    int itr = 0;
    bool hasScene = false;  bool hasObjects = false;
    bool hasCamera = false; bool hasResolution = false;

    Sphere tmpSphere;
    Lambertian tmpLamb(glm::vec3(0, 0, 0));
    Metal tmpMetal(glm::vec3(0, 0, 0));
    int matType = 0;

    std::ifstream sin(_path);
    std::string line;
    while (std::getline(sin, line))
    {
        if (operation == 0)
        {
            if (line.find("~res:") != std::string::npos)
            {
                hasResolution = true;
                operation = 1;
            }
            else if (line.find("~camera:") != std::string::npos)
            {
                hasCamera = true;
                operation = 2;
            }
            else if (line.find("~Scene:") != std::string::npos)
            {
                hasScene = true;
            }
            else if (line.find("~Sphere:") != std::string::npos)
            {
                hasObjects = true;
                operation = 3;
            }
        }
        else if (operation == 1)
        {
            std::cout << line << std::endl;
            std::string x = line.substr(0, line.find(","));
            resolution.x = std::stoi(x);

            std::string y = line.substr(line.find(",") + 2);
            resolution.y = std::stoi(y);
            operation = 0;
        }
        else if (operation == 2)
        {
            switch (itr)
            {
            case 0:
                _camera->horizontal = readVector(line);
                itr++;
                break;
            case 1:
                _camera->lowerLeftCorner = readVector(line);
                itr++;
                break;
            case 2:
                _camera->origin = readVector(line);
                itr++;
                break;
            case 3:
                _camera->vertical = readVector(line);
                itr = 0;
                operation = 0;
                break;
            default:
                break;
            }
        }
        else if (operation == 3)
        {
            switch (itr)
            {
            case 0:
                tmpSphere = Sphere();
                tmpSphere.radius = std::stof(line);
                itr++;
                break;
            case 1:
                tmpSphere.centre = readVector(line);
                itr++;
                break;
            case 2:
                if (line.find("Lambertian") != std::string::npos) { itr = 3; matType = 0; }
                else { itr = 4; matType = 1; }
                break;
            case 3:
                tmpLamb = Lambertian(readVector(line));
                itr = 5;
                break;
            case 4:
                tmpMetal = Metal(readVector(line));
                itr++;
                break;
            default:
                break;
            }
            if (itr == 5)
            {
                if (matType == 0) { tmpSphere.materialPtr = std::make_shared<Lambertian>(tmpLamb); }
                else { tmpSphere.materialPtr = std::make_shared<Metal>(tmpMetal); }

                _scene->objects.push_back(std::make_shared<Sphere>(tmpSphere));
                itr = 0;
                operation = 0;
            }
        }
    }
    if (!(hasCamera && hasObjects && hasResolution && hasScene))
    {
        std::cout << "MISSING COMPONENTS DETECTED" << std::endl;
        throw std::exception();
    }
    _frame = std::make_shared<Frame>(resolution.x, resolution.y);
    _screen = std::make_shared<Screen>(resolution.x, resolution.y);

}

// Reads a glm::vec3 from a formatted string
glm::vec3 MutexRenderer::readVector(std::string _vectorText)
{
    glm::vec3 rtrn = glm::vec3(0.0f, 0.0f, 0.0f);
    int length = _vectorText.find(",");

    std::string x = _vectorText.substr(0, _vectorText.find(","));
    rtrn.x = std::stof(x);

    std::string y = _vectorText.substr(length + (size_t)2, length);
    rtrn.y = std::stof(y);

    std::string z = _vectorText.substr(_vectorText.find_last_of(",") + 1);
    rtrn.z = std::stof(z);

    return rtrn;
}
// Reads a glm::ivec2 from a formatted string
glm::ivec2 MutexRenderer::readIvec2(std::string _input)
{
    glm::ivec2 rtrn = glm::ivec2(0, 0);
    int length = _input.find(",");

    std::string x = _input.substr(0, length);
    rtrn.x = std::stoi(x);

    std::string y = _input.substr(length + (size_t)2);
    rtrn.y = std::stoi(y);

    return rtrn;
}
