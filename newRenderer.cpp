#include "newRenderer.h"

#include "Frame.h"
#include "Ray.h"
#include "Material.h"

#include <thread>

newRenderer::newRenderer(HittableScene _scene, Camera _camera, std::shared_ptr<Frame> _frame)
{
	scene = HittableScene(_scene);
	camera = Camera(_camera);
	frame = _frame;
}

void newRenderer::RenderScene(int _threads)
{
	glm::ivec2 jobDimentions = frame->getResolution() / 10;

    std::vector<std::shared_ptr<Job>> jobs;

    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            std::shared_ptr <Job> job = std::make_shared<Job>();
            for (int x = 0; x < jobDimentions.x; x++)
            {
                for (int y = 0; y < jobDimentions.y; y++)
                {
                    job->jobCoords.push_back(glm::ivec2(i * jobDimentions.x + x, j * jobDimentions.y + y));
                }
            }
            job->_camera = camera;
            job->_scene = scene;
            job->_frame = frame;
            job->done = false;
            job->threadId = -1;
            jobs.push_back(job);
        }
    }

    std::vector<std::thread> renderThreads;

    for (int i = 0; i < _threads; i++)
    {
        jobs.at(i)->threadId = i;
        renderThreads.push_back(std::thread(RenderJob, jobs.at(i)));
    }

    int incomplete = 100;
    int itr = _threads;
    while (incomplete > 0)
    {
        incomplete = 0;
        for (int i = 0; i < jobs.size(); i++)
        {
            if (jobs.at(i)->done)
            {
                int id = jobs.at(i)->threadId;
                renderThreads.at(id).join();
                pushRenderData(jobs.at(i));

                jobs.at(itr)->threadId = id;
                renderThreads.at(id) = std::thread(RenderJob, jobs.at(itr));
                itr++;
            }
            else
            {
                incomplete++;
            }
        }
    }



}

void newRenderer::RenderScene(int _threads, bool _true)
{
    std::vector<std::shared_ptr<Job>> jobs;
    for (int x = 0; x < frame->getResolution().x; x++)
    {
        for (int y = 0; y < frame->getResolution().y; y++)
        {
            std::shared_ptr<Job> tmp = std::make_shared<Job>();

            tmp->jobCoords.push_back(glm::ivec2(x, y));

            tmp->_camera = camera;
            tmp->_scene = scene;
            tmp->_frame = frame;
            tmp->done = false;
            tmp->threadId = -1;
            jobs.push_back(tmp);
        }
    }

    std::vector<std::thread> renderThreads;
    std::vector<std::shared_ptr<Job>> currentJobs;

    for (int i = 0; i < _threads; i++)
    {
        jobs.at(0)->threadId = i;
        currentJobs.push_back(jobs.at(i));
        jobs.erase(jobs.begin());
        renderThreads.push_back(std::thread(RenderJob, currentJobs.at(i)));
    }

    while (jobs.size() > 0)
    {
        for (int i = 0; i < currentJobs.size(); i++)
        {
            if (currentJobs.at(i)->done)
            {
                int id = currentJobs.at(i)->threadId;
                renderThreads.at(id).join();
                pushRenderData(currentJobs.at(i));

                jobs.at(0)->threadId = id;
                currentJobs.at(i) = jobs.at(0);
                jobs.erase(jobs.begin());

                renderThreads.at(id) = std::thread(RenderJob, currentJobs.at(i));
            }
        }
    }
}

void newRenderer::RenderJob(std::shared_ptr<Job> _job)
{
    int width = _job->_frame->getResolution().x;
    int height = _job->_frame->getResolution().y;
    
    for (int i = 0; i < _job->jobCoords.size(); i++)
    {
        glm::vec3 pixelColour(0.0f, 0.0f, 0.0f);
        for (int s = 0; s < 100; s++)
        {
            float u = ((float)_job->jobCoords.at(i).x + glm::linearRand(0.0f, 1.0f)) / (float)(width - 1);
            float v = ((float)_job->jobCoords.at(i).y + glm::linearRand(0.0f, 1.0f)) / (float)(height - 1);

            std::shared_ptr<Ray> ray = std::make_shared<Ray>(_job->_camera.getRay(u, v));

            glm::vec3 colour = _job->getRayColour(ray, 50);

            pixelColour += colour;
        }
        glm::vec3 newColour = _job->writeColour(pixelColour);
        Job::GeneratedData pixel;
        pixel.colour = newColour;
        pixel.location = glm::ivec2
        (
            width - _job->jobCoords.at(i).x - 1,
            height - _job->jobCoords.at(i).y - 1
        );
        _job->data.push_back(pixel);
    }

    _job->done = true;
}

void newRenderer::pushRenderData(std::shared_ptr<Job> _job)
{
    for (int i = 0; i < _job->data.size(); i++)
    {
        frame->writeData(_job->data.at(i).location.x, _job->data.at(i).location.y, _job->data.at(i).colour);
    }
}


glm::vec3 Job::writeColour(glm::vec3 _colour)
{
    float r = _colour.x;
    float g = _colour.y;
    float b = _colour.z;

    float scale = 1.0f / 100;
    r = glm::sqrt(r * scale);
    g = glm::sqrt(g * scale);
    b = glm::sqrt(b * scale);

    glm::ivec3 out(static_cast<int>(256 * glm::clamp(r, 0.0f, 0.999f)),
        static_cast<int>(256 * glm::clamp(g, 0.0f, 0.999f)),
        static_cast<int>(256 * glm::clamp(b, 0.0f, 0.999f))
    );

    return glm::vec3(out.x, out.y, out.z);
}

glm::vec3 Job::getRayColour(std::shared_ptr<Ray> _ray, int _depth)
{
    std::shared_ptr<hitRecord> hits = std::make_shared<hitRecord>();

    if (_depth <= 0)
    {
        return glm::vec3(0.0f, 0.0f, 0.0f);
    }

    if (_scene.hit(_ray, 0.001f, std::numeric_limits<float>::max(), hits))
    {
        std::shared_ptr<Ray> scattered = std::make_shared<Ray>();
        std::shared_ptr<glm::vec3> attenuation = std::make_shared<glm::vec3>(0.0f, 0.0f, 0.0f);

        if (hits->materialPtr->scatter(_ray, hits, attenuation, scattered))
        {
            return glm::vec3(*attenuation * getRayColour(scattered, _depth - 1));
        }
        else
        {
            return glm::vec3(0.0f, 0.0f, 0.0f);
        }

    }
    glm::vec3 unitDirection = glm::normalize(_ray->direction);
    float t = 0.5f * (unitDirection.y + 1.0f);
    return (1.0f - t) * glm::vec3(1.0f, 1.0f, 1.0f) + (t * glm::vec3(0.5f, 0.7f, 1.0f));
}