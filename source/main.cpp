#include "framework/engine.h"
#include "framework/utils.h"
#include "railway.h"
#include "train.h"
#include <memory>

using namespace std;
using namespace glm;

/*
* Coordinate system:
* x - right
* y - up
* z - backward
*/

int main()
{
	// initialization
	Engine *engine = Engine::get();
	engine->init(1600, 900, "UNIGINE Test Task");

	// set up camera
	Camera &cam = engine->getCamera();
	cam.Position = vec3(0.0f, 12.0f, 17.0f);
	cam.Yaw = -90.0f;
	cam.Pitch = -45.0f;
	cam.UpdateCameraVectors();

	// create shared meshes
	Mesh plane_mesh = createPlane();
	Mesh sphere_mesh = createSphere();

	// create background objects
	Object *plane = engine->createObject(&plane_mesh);
	plane->setColor(0.2f, 0.37f, 0.2f); // green
	plane->setPosition(0, -0.5f, 0);
	plane->setRotation(-90.0f, 0.0f, 0.0f);
	plane->setScale(20.0f);

	// path
	const vector<glm::vec3> path = {
		{ 0.0f, -0.375f,  7.0f}, // 1
		{-6.0f, -0.375f,  5.0f}, // 2
		{-8.0f, -0.375f,  1.0f}, // 3
		{-4.0f, -0.375f, -6.0f}, // 4
		{ 0.0f, -0.375f, -7.0f}, // 5
		{ 1.0f, -0.375f, -4.0f}, // 6
		{ 4.0f, -0.375f, -3.0f}, // 7
		{ 8.0f, -0.375f,  7.0f}  // 8
	};
	//vector<Object *> points;
	//for (const auto vec : path)
	//{
	//Object *sphere = engine->createObject(&sphere_mesh);
	//	sphere->setColor(1, 0, 0);
	//	sphere->setPosition(vec.x, vec.y, vec.z);
	//	sphere->setScale(0.25f);
	//	points.push_back(sphere);
	//}
	//LineDrawer path_drawer(reinterpret_cast<const float*>(path.data()), path.size(), true);

	shared_ptr<Railway> railway(new Railway(path));

	float speed = 5.0f;
	vector<shared_ptr<Train>> trains;
	for (int i = 0; i < 8; ++i)
	{
		shared_ptr<Train> train(new Train(railway, i * 1.1f, speed));
		trains.push_back(train);
	}

	double time = 0;
	double old_time = 0;

	// main loop
	while (!engine->isDone())
	{
		engine->update();
		engine->render();

		//path_drawer.draw();
		railway->Draw();
		
		time = glfwGetTime();
		for (auto train : trains)
		{
			train->Update(time - old_time);
			train->Draw();
		}

		old_time = time;

		engine->swap();
	}

	engine->shutdown();
	return 0;
}
