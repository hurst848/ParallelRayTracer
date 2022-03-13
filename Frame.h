#include <Vector>
#include "include/glm/ext.hpp"

struct Frame
{
public:
	// Empty constructor
	Frame();
	// Constructor that takes resoluiton as parameter
	Frame(int _width, int _height);

	// Writes data to the frame given 2 interger coordinate values and vec3 colour value
	void writeData(int _x, int _y, glm::vec3 _colour);
	// Writes data to the frame given a ivec2 coordinate value and vec3 colour value
	void writeData(glm::ivec2 _coord, glm::vec3 _colour);
	// Reads data from the frame given x and y coordinate values and returns a vec3 colour value
	glm::vec3 readData(int _x, int _y);
	
	// Wipes the frame of all data, reseting all pixels to black
	void purge();

	// Return the resolution of the frame
	glm::ivec2 getResolution();

private:
	// Stores the width of the frame
	int width;
	// Stores the height of the frame
	int height;
	// Where the pixel data of the frame is stored
	std::vector<glm::vec3> pixelData;
};