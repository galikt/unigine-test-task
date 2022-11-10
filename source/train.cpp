#include "train.h"

Train::Train(std::shared_ptr<Railway> railway, float offset, float speed) :
	CurrentMesh(createCube())
{
	AttachRailway = railway;
	Speed = speed;

	if (AttachRailway)
		AttachRailway->CalcPosition(Position, offset, Transformation);
}

Train::~Train()
{
}

void Train::Update(const double dt)
{
	if (AttachRailway)
		AttachRailway->CalcPosition(Position, Speed * dt, Transformation);
}

void Train::Draw()
{	
	Engine* engine = Engine::get();

	glm::mat4 mat(1.0f);
	mat *= glm::scale(mat, glm::vec3(1.0f, 0.8, 1.0f));
	mat *= glm::translate(mat, glm::vec3(0.0f, 0.5, 0.0f));

	Transformation *= mat;

	engine->getShader().setMat4("model", Transformation);
	engine->getShader().setVec3("albedo", Color);
	
	CurrentMesh.draw();
}