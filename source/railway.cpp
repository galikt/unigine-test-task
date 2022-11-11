#include "railway.h"
#include <cmath>

//растояние между ключевыми точками
const float STEP{ 0.5f };

//удаленость от центра рельс
const float RAIL_ALIGNMENT{ 0.2f };
//ширена рельс
const float RAIL_WIDTH{ 0.03f };
//высота рельс
const float RAIL_HEIGHT{ 0.2f };

//растояние между шпалами
const float SLEEPER_STEP{ 0.3f };
//ширена шпалы
const float SLEEPER_WIDTH{ 0.05f };
//высота шпалы
const float SLEEPER_HEIGHT{ 0.4f };

Railway::Railway(const std::vector<glm::vec3>& points)
{
	CreatePath(points);
	CreateMesh();
}

Railway::~Railway()
{
}

void Railway::CreatePath(const std::vector<glm::vec3>& points)
{
	int size = points.size();
	float accuracy;
	for (int i = 0; i < size; ++i)
	{
		const glm::vec3 v1 = points[i];
		const glm::vec3 v2 = points[(i + 1) % size];
		const glm::vec3 v3 = points[(i + 2) % size];
		const glm::vec3 v4 = points[(i + 3) % size];

		//расчет шага для равномерного распределение точек на кривой
		float length = glm::length(v3 - v2);
		accuracy = 1.0 / (length / STEP);

		glm::vec3 position;
		for (float t = 0.0f; t < 1.0f; t += accuracy)
		{
			position = glm::catmullRom(v1, v2, v3, v4, t);
			Path.push_back(position);
		}
	}
}

void Railway::CreateMesh()
{
	//шпалы
	std::vector<Vertex> sleeper_vertices;
	std::vector<unsigned int> sleeper_indices;
	int count = 0;
	Position position;
	int before;
	int after;
	glm::mat4 mat;
	for (;;)
	{
		before = position.index;
		CalcPosition(position, SLEEPER_STEP, mat);
		after = position.index;

		//начался новый круг
		if (after - before < 0)
			break;

		std::vector<glm::vec4> points{
			{-SLEEPER_HEIGHT, 0.0f, SLEEPER_WIDTH, 1.0f },
			{ SLEEPER_HEIGHT, 0.0f, SLEEPER_WIDTH, 1.0f },
			{ SLEEPER_HEIGHT, 0.0f,-SLEEPER_WIDTH, 1.0f },
			{-SLEEPER_HEIGHT, 0.0f,-SLEEPER_WIDTH, 1.0f },
		};

		for (int i = 0; i < points.size(); ++i)
		{
			points[i] = mat * points[i];
		}

		//расчет нормали к поверхности
		glm::vec3 vec1(points[1] - points[0]);
		glm::vec3 vec2(points[3] - points[0]);
		glm::vec3 norm = glm::cross(vec1, vec2);
		norm = glm::normalize(norm);

		sleeper_vertices.push_back({ glm::vec3(points[0]), norm, glm::vec2(0,0) });
		sleeper_vertices.push_back({ glm::vec3(points[1]), norm, glm::vec2(1,0) });
		sleeper_vertices.push_back({ glm::vec3(points[2]), norm, glm::vec2(1,1) });
		sleeper_vertices.push_back({ glm::vec3(points[3]), norm, glm::vec2(0,1) });

		sleeper_indices.push_back(count + 0);
		sleeper_indices.push_back(count + 1);
		sleeper_indices.push_back(count + 2);
		sleeper_indices.push_back(count + 2);
		sleeper_indices.push_back(count + 3);
		sleeper_indices.push_back(count + 0);

		count += 4;
	}
	SleeperMesh.set(sleeper_vertices, sleeper_indices);

	//Рельсы
	std::vector<Vertex> rail_vertices;
	std::vector<unsigned int> rail_index;
	count = 0;
	position.index = 0;
	position.offset = 0.0f;
	glm::mat4 old_mat;
	CalcPosition(position, 0.0f, old_mat);
	bool run = true;
	while (run)
	{
		before = position.index;
		CalcPosition(position, RAIL_HEIGHT, mat);
		after = position.index;

		//начался новый круг
		if (after - before < 0)
			run = false;

		float z = 0.005f;
		std::vector<glm::vec4> points{
			{-RAIL_ALIGNMENT - RAIL_WIDTH, z, 0.0f, 1.0f },
			{-RAIL_ALIGNMENT + RAIL_WIDTH, z, 0.0f, 1.0f },
			{-RAIL_ALIGNMENT + RAIL_WIDTH, z, 0.0f, 1.0f },
			{-RAIL_ALIGNMENT - RAIL_WIDTH, z, 0.0f, 1.0f },

			{ RAIL_ALIGNMENT - RAIL_WIDTH, z, 0.0f, 1.0f },
			{ RAIL_ALIGNMENT + RAIL_WIDTH, z, 0.0f, 1.0f },
			{ RAIL_ALIGNMENT + RAIL_WIDTH, z, 0.0f, 1.0f },
			{ RAIL_ALIGNMENT - RAIL_WIDTH, z, 0.0f, 1.0f },
		};

		for (int i = 0; i < points.size(); i += 4)
		{
			points[i]     = old_mat * points[i];
			points[i + 1] = old_mat * points[i + 1];
			points[i + 2] = mat     * points[i + 2];
			points[i + 3] = mat     * points[i + 3];
		}

		//расчет нормали к поверхности
		glm::vec3 vecl1(points[1] - points[0]);
		glm::vec3 vecl2(points[3] - points[0]);
		glm::vec3 norm_left = glm::cross(vecl1, vecl2);
		norm_left = glm::normalize(norm_left);

		glm::vec3 vecr1(points[5] - points[4]);
		glm::vec3 vecr2(points[7] - points[4]);
		glm::vec3 norm_right = glm::cross(vecr1, vecr2);
		norm_right = glm::normalize(norm_left);

		rail_vertices.push_back({ glm::vec3(points[0]), norm_left, glm::vec2(0,0) });
		rail_vertices.push_back({ glm::vec3(points[1]), norm_left, glm::vec2(1,0) });
		rail_vertices.push_back({ glm::vec3(points[2]), norm_left, glm::vec2(1,1) });
		rail_vertices.push_back({ glm::vec3(points[3]), norm_left, glm::vec2(0,1) });

		rail_vertices.push_back({ glm::vec3(points[4]), norm_right, glm::vec2(0,0) });
		rail_vertices.push_back({ glm::vec3(points[5]), norm_right, glm::vec2(1,0) });
		rail_vertices.push_back({ glm::vec3(points[6]), norm_right, glm::vec2(1,1) });
		rail_vertices.push_back({ glm::vec3(points[7]), norm_right, glm::vec2(0,1) });

		rail_index.push_back(count + 0);
		rail_index.push_back(count + 1);
		rail_index.push_back(count + 2);
		rail_index.push_back(count + 2);
		rail_index.push_back(count + 3);
		rail_index.push_back(count + 0);

		rail_index.push_back(count + 4);
		rail_index.push_back(count + 5);
		rail_index.push_back(count + 6);
		rail_index.push_back(count + 6);
		rail_index.push_back(count + 7);
		rail_index.push_back(count + 4);

		count += 8;
		old_mat = mat;
	}
	RailMesh.set(rail_vertices, rail_index);
}

void Railway::CalcPosition(Position& position, float offset, glm::mat4& transformation)
{
	const int size = Path.size();
	int index = position.index % size;

	glm::vec3 p1;
	glm::vec3 p2;
	glm::vec3 p3;

	glm::vec3 d1;
	glm::vec3 d2;

	//растояние между ключевыми точками
	auto calc_length = [&]() -> float
	{
		p1 = Path[index];
		p2 = Path[(index + 1) % size];
		d1 = p2 - p1;
		return glm::length(d1);
	};

	//поиск индекса вершины в масиве Path
	position.offset += offset;
	float length;
	while (position.offset > (length = calc_length()))
	{
		position.offset -= length;
		++index;
		index = index % size;
	}

	p3 = Path[(index + 2) % size];
	d2 = p3 - p2;
	d2 = glm::normalize(d2);

	//расчет переноса меша
	d1 = glm::normalize(d1);
	glm::vec3 translate = p1 + (d1 * position.offset);

	//расчет поворота меша
	float t = position.offset / length;

	glm::vec3 up(0.0f, 1.0, 0.0f);
	glm::quat r1 = glm::quatLookAt(d1, up);
	glm::quat r2 = glm::quatLookAt(d2, up);
	glm::quat rotate = glm::slerp(r1, r2, t);

	glm::mat4 mat(1.0f);
	mat *= glm::translate(mat, translate);
	mat *= glm::mat4_cast(rotate);

	transformation = mat;
	position.index = index;
}

void Railway::Draw()
{
	Engine* engine = Engine::get();

	engine->getShader().setMat4("model", glm::mat4(1.0f));
	engine->getShader().setVec3("albedo", glm::vec3(0.63f, 0.27f, 0.03f));
	SleeperMesh.draw(GL_TRIANGLES);

	engine->getShader().setVec3("albedo", glm::vec3(0.28f, 0.26f, 0.22f));
	RailMesh.draw(GL_TRIANGLES);
}