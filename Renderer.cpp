#include "Renderer.h"

#include "HittableScene.h"
#include "Frame.h"
#include "Ray.h"
#include "Material.h"
#include "Camera.h"
#include "Sphere.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <string>

Renderer::Renderer()
{
    scene = std::make_shared<HittableScene>();

    std::shared_ptr<Material> m_Ground = std::make_shared<Lambertian>(glm::vec3(0.8f, 0.8f, 0.0f));
    std::shared_ptr<Material> m_Centre = std::make_shared<Lambertian>(glm::vec3(0.7f, 0.3f, 0.3f));
    std::shared_ptr<Material> m_Left = std::make_shared<Metal>(glm::vec3(0.8f, 0.8f, 0.8f));
    std::shared_ptr<Material> m_Right = std::make_shared<Metal>(glm::vec3(0.8f, 0.6f, 0.2f));

    scene->add(std::make_shared<Sphere>(glm::vec3(0.0f, -100.5f, -1.0f), 100.0f, m_Ground));
    scene->add(std::make_shared<Sphere>(glm::vec3(0.0f, 0.0f, -1.0f), 0.5f, m_Centre));
    scene->add(std::make_shared<Sphere>(glm::vec3(-1.0f, 0.0f, -1.0f), 0.5f, m_Left));
    scene->add(std::make_shared<Sphere>(glm::vec3(1.0f, 0.0f, -1.0f), 0.5f, m_Right));

    camera = std::make_shared<Camera>();

    frame = std::make_shared<Frame>(1280, 720);
}

Renderer::Renderer(std::shared_ptr<HittableScene> _scene, std::shared_ptr<Frame> _frame, std::shared_ptr<Camera> _camera, int _depth, int _samples)
{
    scene = _scene;
    frame = _frame;
    camera = _camera;
    maxDepth = _depth;
    samplesPerPixel = _samples;
}



void Renderer::Render()
{
    std::cout << "RENDERER SELECTED:      SINGLE THREADED" << std::endl
        << "THREAD BENCHMARK :      SKIPPED" << std::endl 
        << "JOB ASSIGNMENT   :      SKIPPED" << std::endl << std::endl
        << "BEGINING RENDER" << std::endl << std::endl;

    int height = frame->getResolution().y;
    int width = frame->getResolution().x;

    std::chrono::steady_clock::time_point startTime = std::chrono::high_resolution_clock::now();

    for (int i = height - 1; i >= 0; i--)
    {
        for (int j = 0; j < width; j++)
        {
            glm::vec3 pixelColour(0.0f, 0.0f, 0.0f);
            for (int s = 0; s < samplesPerPixel; s++)
            {
                float u = ((float)j + glm::linearRand(0.0f, 1.0f)) / (float)(width - 1);
                float v = ((float)i + glm::linearRand(0.0f, 1.0f)) / (float)(height - 1);

                std::shared_ptr<Ray> ray = std::make_shared<Ray>(camera->getRay(u, v));

                glm::vec3 colour = getRayColour(ray, maxDepth);

                pixelColour += colour;
            }
            glm::vec3 newColour = writeColour(pixelColour);
            frame->writeData(width - j - 1, height - i - 1, newColour);
        }
    }

    std::chrono::steady_clock::time_point endTime = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    std::cout << "FRAME GENERATED" << std::endl
        << "TIME ELAPSED: " << std::to_string(milliseconds.count()) << std::endl;
}

void Renderer::Render(int _threads)
{
    std::cout << "RENDERER SELECTED:      SINGLE THREADED" << std::endl
        << "THREAD BENCHMARK :      SKIPPED" << std::endl
        << "JOB ASSIGNMENT   :      STARTED" << std::endl << std::endl;

    std::chrono::steady_clock::time_point startTime = std::chrono::high_resolution_clock::now();

    // create render jobs
    glm::ivec2 jobDimentions = frame->getResolution() / 10;
    
    std::vector<RenderJob> jobs;

    // break up renderer into 100 smaller tasks
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            RenderJob job;
            for (int x = 0; x < jobDimentions.x; x++)
            {
                for (int y = 0; y < jobDimentions.y; y++)
                {
                    job.addTask(glm::ivec2(i * jobDimentions.x + x, j * jobDimentions.y + y));
                }
            }
            jobs.push_back(job);
        }
    }
    int totalPixels = frame->getResolution().x * frame->getResolution().y;
    for (int i = 0; i < totalPixels; i++)
    {

    }

    // Create tasks for each thread
    std::vector<CompleteRenderTask> tasks(_threads);

    // init all tasks
    for (int i = 0; i < tasks.size(); i++)
    {
        tasks.at(i).camera = *camera;        tasks.at(i).frame = frame;
        tasks.at(i).maxDepth = maxDepth;    tasks.at(i).samplesPerPixel = samplesPerPixel;
        tasks.at(i).scene = *scene;
    }

    // assign jobs to all tasks
    int itr = 0;
    for (int i = 0; i < jobs.size(); i++)
    {
        if (itr >= _threads) { itr = 0; }
        tasks.at(itr).jobs.push_back(jobs.at(i));
        itr++;
    }
    
    // start all threads
    std::vector<std::thread> threads;
    for (int i = 0; i < tasks.size(); i++)
    {
        std::shared_ptr<CompleteRenderTask> tmpTask = std::make_shared<CompleteRenderTask>(tasks.at(i));
        threads.push_back(std::thread(RenderJobs, tmpTask));
    }

    std::cout << "JOB ASSIGNMENT COMPLETE " << std::endl << std::endl 
        << "BEGINING RENDER" << std::endl << std::endl;

    // wait to join all of the threads
    for (int i = 0; i < threads.size(); i++)
    {
        threads.at(i).join();
    }

    std::chrono::steady_clock::time_point endTime = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    std::cout << "FRAME GENERATED" << std::endl
        << "TIME ELAPSED: " << std::to_string(milliseconds.count()) << std::endl;
}

void Renderer::RenderJobs(std::shared_ptr<CompleteRenderTask> _task)
{

    int width = _task->frame->getResolution().x;
    int height = _task->frame->getResolution().y;

    for (int i = 0; i < _task->jobs.size(); i++)
    {
        std::vector<RendererData> data;

        for (int j = 0; j < _task->jobs.at(i).pixelsToRender.size(); j++)
        {
            glm::vec3 pixelColour(0.0f, 0.0f, 0.0f);
            for (int s = 0; s < _task->samplesPerPixel; s++)
            {
                float u = ((float)_task->jobs.at(i).pixelsToRender.at(j).x + glm::linearRand(0.0f, 1.0f)) / (float)(width - 1);
                float v = ((float)_task->jobs.at(i).pixelsToRender.at(j).y + glm::linearRand(0.0f, 1.0f)) / (float)(height - 1);

                std::shared_ptr<Ray> ray = std::make_shared<Ray>(_task->camera.getRay(u, v));

                glm::vec3 colour = _task->getRayColour(ray, _task->maxDepth);

                pixelColour += colour;
            }
            glm::vec3 newColour = _task->writeColour(pixelColour);
            RendererData pixel; 
            pixel.colour = newColour; 
            pixel.location = glm::ivec2
            (
                width - _task->jobs.at(i).pixelsToRender.at(j).x - 1, 
                height - _task->jobs.at(i).pixelsToRender.at(j).y - 1
            );
            data.push_back(pixel);
        }

        _task->pushRenderData(data);

    }
}

void CompleteRenderTask::pushRenderData(std::vector<RendererData> _data)
{
    for (int i = 0; i < _data.size(); i++)
    {
        frame->writeData(_data.at(i).location.x, _data.at(i).location.y, _data.at(i).colour);
    }
}


glm::vec3 Renderer::writeColour(glm::vec3 _colour)
{
    float r = _colour.x;
    float g = _colour.y;
    float b = _colour.z;

    float scale = 1.0f / samplesPerPixel;
    r = glm::sqrt(r * scale);
    g = glm::sqrt(g * scale);
    b = glm::sqrt(b * scale);

    glm::ivec3 out(static_cast<int>(256 * glm::clamp(r, 0.0f, 0.999f)),
        static_cast<int>(256 * glm::clamp(g, 0.0f, 0.999f)),
        static_cast<int>(256 * glm::clamp(b, 0.0f, 0.999f))
    );

    return glm::vec3(out.x, out.y, out.z);
}

glm::vec3 Renderer::getRayColour(std::shared_ptr<Ray> _ray, int _depth)
{
    std::shared_ptr<hitRecord> hits = std::make_shared<hitRecord>();

    if (_depth <= 0)
    {
        return glm::vec3(0.0f, 0.0f, 0.0f);
    }

    if (scene->hit(_ray, 0.001f, std::numeric_limits<float>::max(), hits))
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



glm::vec3 CompleteRenderTask::writeColour(glm::vec3 _colour)
{
    float r = _colour.x;
    float g = _colour.y;
    float b = _colour.z;

    float scale = 1.0f / samplesPerPixel;
    r = glm::sqrt(r * scale);
    g = glm::sqrt(g * scale);
    b = glm::sqrt(b * scale);

    glm::ivec3 out(static_cast<int>(256 * glm::clamp(r, 0.0f, 0.999f)),
        static_cast<int>(256 * glm::clamp(g, 0.0f, 0.999f)),
        static_cast<int>(256 * glm::clamp(b, 0.0f, 0.999f))
    );

    return glm::vec3(out.x, out.y, out.z);
}
glm::vec3 CompleteRenderTask::getRayColour(std::shared_ptr<Ray> _ray, int _depth)
{
    std::shared_ptr<hitRecord> hits = std::make_shared<hitRecord>();

    if (_depth <= 0)
    {
        return glm::vec3(0.0f, 0.0f, 0.0f);
    }

    if (scene.hit(_ray, 0.001f, std::numeric_limits<float>::max(), hits))
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