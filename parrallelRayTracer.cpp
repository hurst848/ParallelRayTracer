#define SDL_MAIN_HANDLED
// TODO:
//      - Find a way to make directories (boost should work)
//      - Finish and run AA test suite
//      - Make and run depth test suite
//      - Make and run Job size test suite (3 res setup required)
#include "Screen.h"
#include "Frame.h"
#include "Ray.h"
#include "Sphere.h"
#include "HittableScene.h"
#include "Camera.h"
#include "hurstLogger.h"
#include "Material.h"
//#include "Renderer.h"
//#include "newRenderer.h"
#include "MutexRenderer.h"

#include "contrib/SDL/include/SDL.h"
#include "include/glm/ext.hpp"

#include <iostream>
#include <fstream>
#include <memory>
#include <thread>
#include <string>
#include <Windows.h>

// Function Declrations for Testing and user interface
void renderLoop(std::shared_ptr<Screen> _screen);
void renderframe(std::shared_ptr<MutexRenderer> _renderer);
void networkRenderframe(std::shared_ptr<MutexRenderer> _renderer);

void runThreadTestingSuite();
void runThreadTest(std::shared_ptr<hurst::logger> _l, std::shared_ptr<MutexRenderer> _renderer, std::shared_ptr<int> _threads);

void runAASampleTestingSuite();
void runAASampleTest(std::shared_ptr<hurst::logger> _l, std::shared_ptr<MutexRenderer> _renderer, std::shared_ptr<int> _samples, std::shared_ptr<Screen> _screen);

void runScatterDepthTestingSuite();
void runScatterDepthTest(std::shared_ptr<hurst::logger> _l, std::shared_ptr<MutexRenderer> _renderer, std::shared_ptr<int> _depth);

void runJobSizeTestingSuite();
void runJobSizeTest(std::shared_ptr<hurst::logger> _l, std::shared_ptr<MutexRenderer> _renderer, std::shared_ptr<glm::ivec2> _jobSize);

void customRenderSetup();
void runCustomRender(std::shared_ptr<MutexRenderer> _renderer, std::shared_ptr<int> _threads);


bool runLoop = true; // Used for enabling/disabling display in testing suites

int main()
{
    // Get user input to select what the program does
    std::cout << "Scene and screen initalized " << std::endl
        << "Please Enter Type of Renderer to Start: " << std::endl
        << "1. Default Settings" << std::endl
        << "2. Network MultiThreading (NON-FUCNTIONAL)" << std::endl 
        << "3. Thread Number Testing Suite" << std::endl  
        << "4. AA Sample Testing Suite" << std::endl 
        << "5. Scatter Depth Testing Suite" << std::endl
        << "6. Job Size Testing Suite" << std::endl 
        << "7. Custom Render Setup" << std::endl << std::endl << "//< ";

    int choice = -1;
    std::cin >> choice;

    
    if (choice < 3)    // If not a full testing suite is chosen
    {
        // Setup some rendering / display parameters
        int width = 1280;
        int height = 720;
        float aspectRatio = (float)width / (float)height;
        float samplesPerPixle = 100.0f;
        int maxDepth = 50;

        // Create Basic Scene
        std::shared_ptr<HittableScene> world = std::make_shared<HittableScene>();

        std::shared_ptr<Material> m_Ground = std::make_shared<Lambertian>(glm::vec3(0.8f, 0.8f, 0.0f));
        std::shared_ptr<Material> m_Centre = std::make_shared<Lambertian>(glm::vec3(0.7f, 0.3f, 0.3f));
        std::shared_ptr<Material> m_Left = std::make_shared<Metal>(glm::vec3(0.8f, 0.8f, 0.8f));
        std::shared_ptr<Material> m_Right = std::make_shared<Metal>(glm::vec3(0.8f, 0.6f, 0.2f));

        world->add(std::make_shared<Sphere>(glm::vec3(0.0f, -100.5f, -1.0f), 100.0f, m_Ground));
        world->add(std::make_shared<Sphere>(glm::vec3(0.0f, 0.0f, -1.0f), 0.5f, m_Centre));
        world->add(std::make_shared<Sphere>(glm::vec3(-1.0f, 0.0f, -1.0f), 0.5f, m_Left));
        world->add(std::make_shared<Sphere>(glm::vec3(1.0f, 0.0f, -1.0f), 0.5f, m_Right));

        std::shared_ptr<Camera> cam = std::make_shared<Camera>(glm::ivec2(width, height));

        // Setup the frame and screen to be rendered to
        std::shared_ptr<Frame> frame = std::make_shared<Frame>(width, height);
        std::shared_ptr<Screen> screen = std::make_shared<Screen>(width, height);
        screen->updateFrame(frame);
        screen->render();



        std::cout << "Width: " << width << "\t Height: " << height << std::endl;;

        // Create the renderer
        std::shared_ptr<MutexRenderer> renderer = std::make_shared<MutexRenderer>(world, cam, frame);
        renderer->Serialize();

        std::cout << renderer->serialized << std::endl << std::endl;

        if (choice == 1)    // If a standard renderer is chosen
        {
            // Create a thread for the renderer to operate within
            std::thread renderThread(renderframe, renderer);
            // Start the display loop
            renderLoop(screen);
            renderThread.join();
        }
        else if (choice == 2)   // If network renderer is chosen
        {
            // Create a thread for the renderer and server to operate within
            std::thread renderThread(networkRenderframe, renderer);
            // Start the display loop
            renderLoop(screen);
            renderThread.join();
        }
        else
        {
            std::cout << std::endl << "INVALID CHOISE, EXITING PROGRAM" << std::endl;
        }
    }
    else if (choice == 3)// run full testing suite
    {
        runThreadTestingSuite();
    }
    else if (choice == 4)
    {
        runAASampleTestingSuite();
    }
    else if (choice == 5)
    {
        runScatterDepthTestingSuite();
    }
    else if (choice == 6)
    {
        runJobSizeTestingSuite();
    }
    else if (choice == 7)
    {
        customRenderSetup();
    }
    else
    {
        std::cout << std::endl << "INVALID CHOISE, EXITING PROGRAM" << std::endl;
    }
    return 0;
}

// Funciton for the diplay/game loop
void renderLoop(std::shared_ptr<Screen> _screen)
{
    bool display = true;
    while (display)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                display = false;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                // Quit the renderer if ESC is pressed
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    display = false;
                }
            }
        }
        // Rendere current frame to the screen
        _screen->render();
    }
}

// Used to create at thread to hold the renderer
void renderframe(std::shared_ptr<MutexRenderer> _renderer)
{
    int numThreads = 1;

    // Create a logger to output timing data
    hurst::logger l("raytracingdata.csv");
    std::cout << "Threads: " << numThreads << std::endl << std::endl;
    // Start timer
    std::chrono::steady_clock::time_point startTime = std::chrono::high_resolution_clock::now();
    // Start renderer
    _renderer->RenderScene(numThreads);
    // End timer
    std::chrono::steady_clock::time_point endTime = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    // Output time take to file through the logger
    l.logLn(std::to_string(numThreads) + ", " + std::to_string(milliseconds.count()));
    std::cout << "Time Elapsed: " << milliseconds.count() << std::endl;
    
    // Destroy and close the logger
    l.stop();
    
}

// Used to create at thread to hold the renderer
void networkRenderframe(std::shared_ptr<MutexRenderer> _renderer)
{
    // Starts the server and the renderer
    //_renderer->NetworkRender();
}

// Executes a testing suite that times how long it takes to render
// with resolution and number of threads as varibles
void runThreadTestingSuite()
{
    // Create vector to hold testing resolutions
    std::vector<glm::ivec2> resolutions
    {
        glm::ivec2(320,240),
        glm::ivec2(640,480),
        glm::ivec2(1280,720),
    };

    // Create Basic Scene
    std::shared_ptr<HittableScene> world = std::make_shared<HittableScene>();

    std::shared_ptr<Material> m_Ground = std::make_shared<Lambertian>(glm::vec3(0.8f, 0.8f, 0.0f));
    std::shared_ptr<Material> m_Centre = std::make_shared<Lambertian>(glm::vec3(0.7f, 0.3f, 0.3f));
    std::shared_ptr<Material> m_Left = std::make_shared<Metal>(glm::vec3(0.8f, 0.8f, 0.8f));
    std::shared_ptr<Material> m_Right = std::make_shared<Metal>(glm::vec3(0.8f, 0.6f, 0.2f));

    world->add(std::make_shared<Sphere>(glm::vec3(0.0f, -100.5f, -1.0f), 100.0f, m_Ground));
    world->add(std::make_shared<Sphere>(glm::vec3(0.0f, 0.0f, -1.0f), 0.5f, m_Centre));
    world->add(std::make_shared<Sphere>(glm::vec3(-1.0f, 0.0f, -1.0f), 0.5f, m_Left));
    world->add(std::make_shared<Sphere>(glm::vec3(1.0f, 0.0f, -1.0f), 0.5f, m_Right));

    // Start looping through all testing resolutions 
    for (int i = 0; i < resolutions.size(); i++)
    {
        // Create filename for the results
        std::string fileName = "log_" + std::to_string(resolutions.at(i).x) + "X" + std::to_string(resolutions.at(i).y) + "_results.csv";
        // Create the logger for data output
        std::shared_ptr<hurst::logger> l = std::make_shared<hurst::logger>(fileName);

        // Setup some rendering / display parameters
        float samplesPerPixle = 100.0f;
        int maxDepth = 50;

        // Create the camera and the screen for this resolution
        std::shared_ptr<Camera> cam = std::make_shared<Camera>(resolutions.at(i));
        std::shared_ptr<Screen> screen = std::make_shared<Screen>(resolutions.at(i).x, resolutions.at(i).y);

        // Loop through a number of threads
        for  (int threads = 1; threads <= 50 ; threads++)
        {
            // Loop through each test 5 times for an average
            for (int j = 0; j < 5; j++)
            {
                // Enable the displaying to screen
                runLoop = true;
                // Create a Frame and assign it to the screen to be rendered
                std::shared_ptr<Frame> frame = std::make_shared<Frame>(resolutions.at(i).x, resolutions.at(i).y);
                screen->updateFrame(frame);
                // Create the renderer
                std::shared_ptr<MutexRenderer> renderer = std::make_shared<MutexRenderer>(world, cam, frame);
                // Create a thread for the renderer to operate within
                std::thread renderThread(runThreadTest, l, renderer, std::make_shared<int>(threads));

                // Loop to allow display and rendering of output
                while (runLoop)
                {
                    SDL_Event event;
                    while (SDL_PollEvent(&event))
                    {
                        if (event.type == SDL_QUIT)
                        {
                            runLoop = false;
                        }
                        else if (event.type == SDL_KEYDOWN)
                        {
                            // Quit the renderer if ESC is pressed
                            if (event.key.keysym.sym == SDLK_ESCAPE)
                            {
                                runLoop = false;
                            }
                        }
                    }
                    screen->render();
                }
                renderThread.join();
            }
        }
        // Destroy and close the logger
        l->stop();
    }
}
// Times and executes the render of a frame and outputs the results to the logger
void runThreadTest(std::shared_ptr<hurst::logger> _l, std::shared_ptr<MutexRenderer> _renderer, std::shared_ptr<int> _threads)
{
    // Start the timer
    std::chrono::steady_clock::time_point startTime = std::chrono::high_resolution_clock::now();
    // Start the renderer with the amount of threads specified
    _renderer->RenderScene(*_threads);
    // Stop the timer
    std::chrono::steady_clock::time_point endTime = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    _l->logLn(std::to_string(*_threads) + ", " + std::to_string(milliseconds.count()));
    // Stop the display from rendering and conclude this test
    runLoop = false;
}

// Executes a testing suite that times how long it takes to render
// with differnet values in the samples per pixel varible
void runAASampleTestingSuite()
{
    // Check if directory exists, if it doesnt create it //
    // https://www.tenouk.com/cpluscodesnippet/createremovedirectorydeletefile.html
    // https://stackoverflow.com/questions/875249/how-to-get-current-directory
    // Get current directory
    TCHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, buffer, MAX_PATH);
    std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
    std::wstring loc = std::wstring(buffer).substr(0, pos);
    // Check / create new directory
    loc += L"\\AASampleTest";
    LPCWSTR fp = &loc[0];
    if (!CreateDirectory(fp, NULL)){ printf("\nCouldn't create %S directory.\n", fp); }
    else{ printf("\n%S directory successfully created.\n", fp); }

       
    // Create filename for the results
    std::string fileName = "AASampleTest\\log_AA-SampleTest_results.csv";
    // Create the logger for data output
    std::shared_ptr<hurst::logger> l = std::make_shared<hurst::logger>(fileName);

    int width = 1280;
    int height = 720;

    std::shared_ptr<HittableScene> world = std::make_shared<HittableScene>();

    std::shared_ptr<Material> m_Ground = std::make_shared<Lambertian>(glm::vec3(0.8f, 0.8f, 0.0f));
    std::shared_ptr<Material> m_Centre = std::make_shared<Lambertian>(glm::vec3(0.7f, 0.3f, 0.3f));
    std::shared_ptr<Material> m_Left = std::make_shared<Metal>(glm::vec3(0.8f, 0.8f, 0.8f));
    std::shared_ptr<Material> m_Right = std::make_shared<Metal>(glm::vec3(0.8f, 0.6f, 0.2f));

    world->add(std::make_shared<Sphere>(glm::vec3(0.0f, -100.5f, -1.0f), 100.0f, m_Ground));
    world->add(std::make_shared<Sphere>(glm::vec3(0.0f, 0.0f, -1.0f), 0.5f, m_Centre));
    world->add(std::make_shared<Sphere>(glm::vec3(-1.0f, 0.0f, -1.0f), 0.5f, m_Left));
    world->add(std::make_shared<Sphere>(glm::vec3(1.0f, 0.0f, -1.0f), 0.5f, m_Right));

    std::shared_ptr<Camera> cam = std::make_shared<Camera>(glm::ivec2(width, height));

    // Setup the frame and screen to be rendered to
    std::shared_ptr<Frame> frame = std::make_shared<Frame>(width, height);
    std::shared_ptr<Screen> screen = std::make_shared<Screen>(width, height);
    screen->updateFrame(frame);
    screen->render();

    // Start looping through all AA sample values
    for (int i = 1; i <= 225; i++)
    {
        // purge the screen
        frame->purge();
        // Enable the displaying to screen
        runLoop = true;

        // Create the renderer and set the sample value
        std::shared_ptr<MutexRenderer> renderer = std::make_shared<MutexRenderer>(world, cam, frame);
        renderer->setSamples(i);

        // Create a thread for the renderer to operate within
        std::thread renderThread(runAASampleTest, l, renderer, std::make_shared<int>(i), screen);

        // Loop to allow display and rendering of output
        while (true)
        {
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                {
                    runLoop = false;
                }
                else if (event.type == SDL_KEYDOWN)
                {
                    // Quit the renderer if ESC is pressed
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                    {
                        runLoop = false;
                    }
                }
            }
            screen->render();
            if (!runLoop)
            {
                // Sleep Momentarly
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                // Save current Screen as image
                screen->render();
                std::string picName = "AASampleTest\\img_" + std::to_string(i) + ".bmp";
                screen->takeScreenShot(picName);
                break;
            }
        }
        renderThread.join();
        
    }
    l->stop();
}
// Times and executes the render of a frame and outputs the results to the logger and a saved image of the render
void runAASampleTest(std::shared_ptr<hurst::logger> _l, std::shared_ptr<MutexRenderer> _renderer, std::shared_ptr<int> _samples, std::shared_ptr<Screen> _screen)
{
    std::cout << "Rendering with " << *_samples << " samples... \n\n";
    // Start the timer
    std::chrono::steady_clock::time_point startTime = std::chrono::high_resolution_clock::now();
    // Start the renderer with the amount of threads specified
    _renderer->RenderScene(16);
    // Stop the timer
    std::chrono::steady_clock::time_point endTime = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    _l->logLn(std::to_string(*_samples) + ", " + std::to_string(milliseconds.count()));

   
    // Stop the display from rendering and conclude this test
    runLoop = false;
}

// Executes a testing suite that times how long it takes to render
// with differnet values in the depth varible
void runScatterDepthTestingSuite()
{
    // Check if directory exists, if it doesnt create it //
    // https://www.tenouk.com/cpluscodesnippet/createremovedirectorydeletefile.html
    // https://stackoverflow.com/questions/875249/how-to-get-current-directory
    // Get current directory
    TCHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, buffer, MAX_PATH);
    std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
    std::wstring loc = std::wstring(buffer).substr(0, pos);
    // Check / create new directory
    loc += L"\\ScatterDepthTest";
    LPCWSTR fp = &loc[0];
    if (!CreateDirectory(fp, NULL)) { printf("\nCouldn't create %S directory.\n", fp); }
    else { printf("\n%S directory successfully created.\n", fp); }


    // Create filename for the results
    std::string fileName = "ScatterDepthTest\\log_ScatterDepthTest_results.csv";
    // Create the logger for data output
    std::shared_ptr<hurst::logger> l = std::make_shared<hurst::logger>(fileName);

    int width = 1280;
    int height = 720;

    std::shared_ptr<HittableScene> world = std::make_shared<HittableScene>();

    std::shared_ptr<Material> m_Ground = std::make_shared<Lambertian>(glm::vec3(0.8f, 0.8f, 0.0f));
    std::shared_ptr<Material> m_Centre = std::make_shared<Lambertian>(glm::vec3(0.7f, 0.3f, 0.3f));
    std::shared_ptr<Material> m_Left = std::make_shared<Metal>(glm::vec3(0.8f, 0.8f, 0.8f));
    std::shared_ptr<Material> m_Right = std::make_shared<Metal>(glm::vec3(0.8f, 0.6f, 0.2f));

    world->add(std::make_shared<Sphere>(glm::vec3(0.0f, -100.5f, -1.0f), 100.0f, m_Ground));
    world->add(std::make_shared<Sphere>(glm::vec3(0.0f, 0.0f, -1.0f), 0.5f, m_Centre));
    world->add(std::make_shared<Sphere>(glm::vec3(-1.0f, 0.0f, -1.0f), 0.5f, m_Left));
    world->add(std::make_shared<Sphere>(glm::vec3(1.0f, 0.0f, -1.0f), 0.5f, m_Right));

    std::shared_ptr<Camera> cam = std::make_shared<Camera>(glm::ivec2(width, height));

    // Setup the frame and screen to be rendered to
    std::shared_ptr<Frame> frame = std::make_shared<Frame>(width, height);
    std::shared_ptr<Screen> screen = std::make_shared<Screen>(width, height);
    screen->updateFrame(frame);
    screen->render();

    // Start looping through all AA sample values
    for (int i = 1; i <= 225; i++)
    {
        // purge the screen
        frame->purge();
        // Enable the displaying to screen
        runLoop = true;

        // Create the renderer and set the sample value
        std::shared_ptr<MutexRenderer> renderer = std::make_shared<MutexRenderer>(world, cam, frame);
        renderer->setDepth(i);

        // Create a thread for the renderer to operate within
        std::thread renderThread(runScatterDepthTest, l, renderer, std::make_shared<int>(i));

        // Loop to allow display and rendering of output
        while (true)
        {
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                {
                    runLoop = false;
                }
                else if (event.type == SDL_KEYDOWN)
                {
                    // Quit the renderer if ESC is pressed
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                    {
                        runLoop = false;
                    }
                }
            }
            screen->render();
            if (!runLoop)
            {
                // Sleep Momentarly
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                // Save current Screen as image
                screen->render();
                std::string picName = "ScatterDepthTest\\img_" + std::to_string(i) + ".bmp";
                screen->takeScreenShot(picName);
                break;
            }
        }
        renderThread.join();

    }
    l->stop();
}
// Times and executes the render of a frame and outputs the results to the logger and a saved image of the render
void runScatterDepthTest(std::shared_ptr<hurst::logger> _l, std::shared_ptr<MutexRenderer> _renderer, std::shared_ptr<int> _depth)
{
    std::cout << "Rendering with a depth of " << *_depth << "\n\n";
    // Start the timer
    std::chrono::steady_clock::time_point startTime = std::chrono::high_resolution_clock::now();
    // Start the renderer with the amount of threads specified
    _renderer->RenderScene(16);
    // Stop the timer
    std::chrono::steady_clock::time_point endTime = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    _l->logLn(std::to_string(*_depth) + ", " + std::to_string(milliseconds.count()));


    // Stop the display from rendering and conclude this test
    runLoop = false;
}

// Executes a testing suite that times how long it takes to render
// with differnet values of the jobSize varible
void runJobSizeTestingSuite()
{
    // Create vector to hold testing resolutions
    std::vector<glm::ivec2> resolutions
    {
        glm::ivec2(320,240),
        glm::ivec2(640,480),
        glm::ivec2(1280,720)
    };

    // Create vector to hold testing job sizes
    std::vector<glm::ivec2> jobSizes
    {
        glm::ivec2(1,1),
        glm::ivec2(2,2),
        glm::ivec2(5,5),
        glm::ivec2(10,10),
        glm::ivec2(20,20),
        glm::ivec2(40,40),
        glm::ivec2(80,80)
    };

    // Create Basic Scene
    std::shared_ptr<HittableScene> world = std::make_shared<HittableScene>();

    std::shared_ptr<Material> m_Ground = std::make_shared<Lambertian>(glm::vec3(0.8f, 0.8f, 0.0f));
    std::shared_ptr<Material> m_Centre = std::make_shared<Lambertian>(glm::vec3(0.7f, 0.3f, 0.3f));
    std::shared_ptr<Material> m_Left = std::make_shared<Metal>(glm::vec3(0.8f, 0.8f, 0.8f));
    std::shared_ptr<Material> m_Right = std::make_shared<Metal>(glm::vec3(0.8f, 0.6f, 0.2f));

    world->add(std::make_shared<Sphere>(glm::vec3(0.0f, -100.5f, -1.0f), 100.0f, m_Ground));
    world->add(std::make_shared<Sphere>(glm::vec3(0.0f, 0.0f, -1.0f), 0.5f, m_Centre));
    world->add(std::make_shared<Sphere>(glm::vec3(-1.0f, 0.0f, -1.0f), 0.5f, m_Left));
    world->add(std::make_shared<Sphere>(glm::vec3(1.0f, 0.0f, -1.0f), 0.5f, m_Right));

    // Start looping through all testing resolutions 
    for (int i = 0; i < resolutions.size(); i++)
    {
        // Create filename for the results
        std::string fileName = "log_" + std::to_string(resolutions.at(i).x) + "X" + std::to_string(resolutions.at(i).y) + "_jobSize_results.csv";
        // Create the logger for data output
        std::shared_ptr<hurst::logger> l = std::make_shared<hurst::logger>(fileName);

        // Create the camera and the screen for this resolution
        std::shared_ptr<Camera> cam = std::make_shared<Camera>(resolutions.at(i));
        std::shared_ptr<Screen> screen = std::make_shared<Screen>(resolutions.at(i).x, resolutions.at(i).y);

        // Loop through a number of threads
        for (int js = 0; js < jobSizes.size()-1; js++)
        {
            // Loop through each test 5 times for an average
            for (int j = 0; j < 5; j++)
            {
                // Enable the displaying to screen
                runLoop = true;
                // Create a Frame and assign it to the screen to be rendered
                std::shared_ptr<Frame> frame = std::make_shared<Frame>(resolutions.at(i).x, resolutions.at(i).y);
                screen->updateFrame(frame);
                // Create the renderer
                std::shared_ptr<MutexRenderer> renderer = std::make_shared<MutexRenderer>(world, cam, frame);
                // Set the Job size
                renderer->setJobSize(jobSizes.at(js));
                // Create a thread for the renderer to operate within
                std::thread renderThread(runJobSizeTest, l, renderer, std::make_shared<glm::ivec2>(jobSizes.at(js)));

                // Loop to allow display and rendering of output
                while (runLoop)
                {
                    SDL_Event event;
                    while (SDL_PollEvent(&event))
                    {
                        if (event.type == SDL_QUIT)
                        {
                            runLoop = false;
                        }
                        else if (event.type == SDL_KEYDOWN)
                        {
                            // Quit the renderer if ESC is pressed
                            if (event.key.keysym.sym == SDLK_ESCAPE)
                            {
                                runLoop = false;
                            }
                        }
                    }
                    screen->render();
                }
                renderThread.join();
            }
            std::cout << "";
        }
        // Destroy and close the logger
        l->stop();
    }
}
// Times and executes the render of a frame and outputs the results to the logger
void runJobSizeTest(std::shared_ptr<hurst::logger> _l, std::shared_ptr<MutexRenderer> _renderer, std::shared_ptr<glm::ivec2> _jobSize)
{
    // Start the timer
    std::chrono::steady_clock::time_point startTime = std::chrono::high_resolution_clock::now();
    // Start the renderer with the amount of threads specified
    _renderer->RenderScene(16);
    // Stop the timer
    std::chrono::steady_clock::time_point endTime = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    _l->logLn(std::to_string(_jobSize->x * _jobSize->y) + ", " + std::to_string(milliseconds.count()));
    // Stop the display from rendering and conclude this test
    runLoop = false;
}

// Loads externally written scene and renders based on input parameters
void customRenderSetup()
{
    // Get path for scene file
    std::cout << "Entered Custom Setup Mode!\n\n"
        << "Please enter path for valid scene file path:\n\n//< ";
    std::string scenePath = "";
    std::cin >> scenePath;

    // Create all neccicary structs and varibles
    std::shared_ptr<Frame> frame = std::make_shared<Frame>();
    std::shared_ptr<Screen> screen = std::make_shared<Screen>();
    std::shared_ptr<HittableScene> scene = std::make_shared<HittableScene>();
    std::shared_ptr<Camera> camera = std::make_shared<Camera>();

    // Interpret and load the data from the file
    std::shared_ptr<MutexRenderer> renderer = std::make_shared<MutexRenderer>();
    renderer->loadRenderFromFile(frame, screen, scene, camera, scenePath);
    renderer = std::make_shared<MutexRenderer>(scene, camera, frame);
    screen->updateFrame(frame);
    
    // Get number of threads to use in rendering
    int threads = 1;
    std::cout << "\n\nPlease enter a valid number of threads to use for rendering:\n\n//< ";
    std::cin >> threads;

    // Get the number of samples per pixel to use in rendering
    int spp = 100;
    std::cout << "\n\nPlease enter a valid number of samples per pixel to use for AA in rendering:\n\n//< ";
    std::cin >> spp;
    renderer->setSamples(spp);

    // Get the ray scatter depth to use in rendering
    int rsd = 50;
    std::cout << "\n\nPlease enter a valid number for the max depth of scattering used in rendering:\n\n//< ";
    std::cin >> rsd;
    renderer->setDepth(rsd);

    // Begin and Display render
    std::cout << "\n\nRender begining, press ESC to quit once the render is complete . . .\n\n";
    // Create a thread for the renderer to operate within
    std::thread renderThread(runCustomRender, renderer, std::make_shared<int>(threads));
    // Loop to allow display and rendering of output
    while (runLoop)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                runLoop = false;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                // Quit the renderer if ESC is pressed
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    runLoop = false;
                }
            }
        }
        screen->render();
    }
    renderThread.join();
    // Keep displaying render till user exits
    renderLoop(screen);
}
// Executes the render of the custom scene/frame
void runCustomRender(std::shared_ptr<MutexRenderer> _renderer, std::shared_ptr<int> _threads)
{
    _renderer->RenderScene(*_threads);
}

