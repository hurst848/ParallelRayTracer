#include "Camera.h"
#include "HittableScene.h"

#include "include/glm/ext.hpp"

#include <memory>
#include <vector>


struct Frame;
struct HittableScene;
struct Ray;
struct Camera;

struct RendererData
{
public:
	glm::ivec2 location;
	glm::vec3 colour;
};

struct RenderJob
{
public:
	void addTask(glm::ivec2 _task) { pixelsToRender.push_back(_task); }
	std::vector<glm::ivec2> pixelsToRender;
};

struct CompleteRenderTask
{
public:
	std::vector<RenderJob> jobs;
	
	int maxDepth;
	int samplesPerPixel;

	HittableScene scene;
	std::shared_ptr<Frame> frame;
	Camera camera;

	glm::vec3 writeColour(glm::vec3 _colour);
	glm::vec3 getRayColour(std::shared_ptr<Ray> _ray, int _depth);

	void pushRenderData(std::vector<RendererData> _data);

};


struct Renderer
{
public:
	Renderer();
	Renderer(std::shared_ptr<HittableScene> _scene, std::shared_ptr<Frame> _frame, std::shared_ptr<Camera> _camera, int _depth, int _samples);


	void Render();
	void Render(int _threads);
	

private:
	int maxDepth;
	int samplesPerPixel;

	std::shared_ptr<HittableScene> scene;
	std::shared_ptr<Frame> frame;
	std::shared_ptr<Camera> camera;

	glm::vec3 writeColour(glm::vec3 _colour);
	glm::vec3 getRayColour(std::shared_ptr<Ray> _ray, int _depth);

	static void RenderJobs(std::shared_ptr<CompleteRenderTask> _task);
};

