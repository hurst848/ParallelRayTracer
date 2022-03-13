#include "Camera.h"
#include "HittableScene.h"

#include "include/glm/ext.hpp"

#include <memory>
#include <vector>

struct Frame;
struct Ray;

struct Job
{
public:
	struct GeneratedData
	{
	public:
		glm::vec3 colour;
		glm::ivec2 location;
	};
	
	glm::vec3 writeColour(glm::vec3 _colour);
	glm::vec3 getRayColour(std::shared_ptr<Ray> _ray, int _depth);

	std::vector<glm::ivec2> jobCoords;

	HittableScene _scene;
	Camera _camera;
	std::vector<GeneratedData> data;

	std::shared_ptr<Frame> _frame;

	int threadId;
	bool done;
};

struct newRenderer
{
public:
	newRenderer(HittableScene _scene, Camera _camera, std::shared_ptr<Frame> _frame);

	void RenderScene(int _threads);
	void RenderScene(int _threads, bool _true);

	static void RenderJob(std::shared_ptr<Job> _job);

private:
	void pushRenderData(std::shared_ptr<Job> _job);

	HittableScene scene;
	Camera camera;
	std::shared_ptr<Frame> frame;
};