#pragma once

#include "railway.h"
#include <memory>

class Train
{
public:
	Train(std::shared_ptr<Railway> railway, float offset, float speed);
	~Train();

	void Update(const double dt);
	void Draw();

protected:
	glm::mat4 Transformation;
	Mesh CurrentMesh;

	std::shared_ptr<Railway> AttachRailway;
	Railway::Position Position;
	float Speed;
	glm::vec3 Color{ 0.65f, 0.62f, 0.53f };
};
