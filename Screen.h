#include "contrib/SDL/include/SDL.h"
#include "include/glm/ext.hpp"

#include <memory>
#include <string>

struct Frame;

struct Screen
{
public:
	// Empty constructor
	Screen();
	// Constructor that takes resolution as a prameter
	Screen(int _x, int _y);

	// Empty Destructor
	~Screen();

	// Updates which frame that will be rendered 
	void updateFrame(std::shared_ptr<Frame> _frame);
	// Renders the current frame to the screen
	void render();
	// Stores the resolution of the screen
	glm::ivec2 resolution;

	// Takes a Screen shot of whats currently displayed and saves it
	void takeScreenShot(std::string _path);
private:
	// Renderes a black rectangle the size of the window
	void renderBackground();
	
	// Stores a rectangle the size of the window
	SDL_Rect background;
	// Stores the SDL window
	SDL_Window* window;
	// Stores the SDL renderer
	SDL_Renderer* renderer;
	// Stores the curernt frame to be rendered
	std::shared_ptr<Frame> currentFrame;
};
