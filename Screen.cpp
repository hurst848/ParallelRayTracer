#include "Screen.h"

#include "Frame.h"

// Empty constructor
Screen::Screen()
{
	// Inialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		printf("Initilizeation Failed \n");
		throw std::exception();
	}
	// Create a SDL window with a default resolution of 1280x720
	window = SDL_CreateWindow("Ray Tracer Results", 100, 100, 1280, 720, SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		printf("Failed to Create SDL Window \n");
		throw std::exception();
	}
	// Create a SDL accellerated renderer
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL)
	{
		printf("Failed to Create SDL Renderer \n");
		throw std::exception();
	}

	// Create the background rect
	background.x = 0;		background.y = 0;
	background.w = 1280;	background.h = 720;

	// Assign the resolution
	resolution = glm::ivec2(1280, 720);
}

// Constructor that takes resolution as a prameter
Screen::Screen(int _x, int _y)
{
	// Inialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		printf("Initilizeation Failed \n");
		throw std::exception();
	}
	// Create a SDL window with a specified resolution
	window = SDL_CreateWindow("Ray Tracer Results", 100, 100, _x, _y, SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		printf("Failed to Create SDL Window \n");
		throw std::exception();
	}
	// Create a SDL accellerated renderer
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL)
	{
		printf("Failed to Create SDL Renderer \n");
		throw std::exception();
	}

	// Create the background rect
	background.x = 0;		background.y = 0;
	background.w = _x;		background.h = _y;

	// Assign the resolution
	resolution = glm::ivec2(_x, _y);
}

// Empty Destructor
Screen::~Screen()
{
	// Destroy required SDL components
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}

// Updates which frame that will be rendered 
void Screen::updateFrame(std::shared_ptr<Frame> _frame)
{
	// Checks to see if the frame is the correct resolution
	if (_frame->getResolution() != resolution)
	{
		printf("Frame Resolution Not Same as Screen \n");
		throw std::exception();
	}
	// Assigns the frame
	currentFrame = _frame;
}

// Renders the current frame to the screen
void Screen::render()
{
	// Check if there is a frame held within currentFrame
	if (currentFrame == NULL)
	{
		printf("No Frame Assigned To Renderer\n");
		throw std::exception(0);
	}

	// Render the frame
	SDL_RenderClear(renderer);

	// Loop through all stored pixels and draw each indivdual pixel to the screen
	for (int x = 0; x < resolution.x; x++)
	{
		for (int y = 0; y < resolution.y; y++)
		{
			glm::vec3 pixelColour = currentFrame->readData(x, y);
			SDL_SetRenderDrawColor(renderer, pixelColour.x, pixelColour.y, pixelColour.z, 255);
			SDL_RenderDrawPoint(renderer, x, resolution.y - y);
		}
	}
	
	SDL_RenderPresent(renderer);
}

// Renderes a black rectangle the size of the window
void Screen::renderBackground()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(renderer, &background);
}

// Takes a Screen shot of whats currently displayed and saves it
void Screen::takeScreenShot(std::string _path)
{
	SDL_Surface* sshot = SDL_CreateRGBSurface(0, resolution.x, resolution.y, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, sshot->pixels, sshot->pitch);
	SDL_SaveBMP(sshot, _path.c_str());
	SDL_FreeSurface(sshot);
}