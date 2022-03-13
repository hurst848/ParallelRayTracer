#include "Frame.h"


// Empty constructor
Frame::Frame()
{
	// Inialize to a default resolution of 1280x720
	width = 1280;
	height = 720;
	// Push back black pixels till the frame is generated
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			pixelData.push_back(glm::vec3(0,0,0));
		}
	}
}

// Constructor that takes resoluiton as parameter
Frame::Frame(int _width, int _height)
{
	// Take resoluiton parameters and set respective values
	width = _width;
	height = _height;
	// Push back black pixels till the frame is generated
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			pixelData.push_back(glm::vec3(0, 0, 0));
		}
	}
}

// Wipes the frame of all data, reseting all pixels to black
void Frame::purge()
{
	// Loop through all pixels and reset to black
	for (int i = 0; i < pixelData.size(); i++)
	{
		pixelData.at(i) = glm::vec3(0, 0, 0);
	}
}

// Writes data to the frame given 2 interger coordinate values and vec3 colour value
void Frame::writeData(int _x, int _y, glm::vec3 _colour)
{
	// Cast x and y coordinates to 1 dimentional array location
	int index = _x + (_y * width);
	// Set pixel data at coordinate to _colour parameter
	pixelData.at(index) = _colour;
}

// Writes data to the frame given a ivec2 coordinate value and vec3 colour value
void Frame::writeData(glm::ivec2 _coord, glm::vec3 _colour)
{
	// Cast x and y coordinates to 1 dimentional array location
	int index = _coord.x + (_coord.y * width);
	// Set pixel data at coordinate to _colour parameter
	pixelData.at(index) = _colour;
}

// Reads data from the frame given x and y coordinate values and returns a vec3 colour value
glm::vec3 Frame::readData(int _x, int _y)
{
	// Cast x and y coordinates to 1 dimentional array location
	int index = _x + (_y * width);
	// Return pixle data at coordinate
	return pixelData.at(index);
}

// Return the resolution of the frame
glm::ivec2 Frame::getResolution()
{
	return glm::ivec2(width, height);
}