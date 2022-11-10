#pragma once

#include "framework/engine.h"
#include "glm/ext.hpp"

class Railway
{
public:
	Railway(const std::vector<glm::vec3>& points);
	~Railway();

	struct Position
	{
		int index{ 0 };
		float offset{ 0 };
	};

	void CalcPosition(Position& position, float offset, glm::mat4& transformation);
	void Draw();

protected:
	std::vector<glm::vec3> Path;
	Mesh SleeperMesh;
	Mesh RailMesh;

	void CreatePath(const std::vector<glm::vec3>& points);
	void CreateMesh();
};

