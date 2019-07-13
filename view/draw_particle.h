#include <iostream>
#include "../common/particle.h"
#include "imgui/glew/GL/glew.h"
#define max_tep 125.00
#define min_tep -100.00
#define max_pre 1.00
#define min_pre -1.00

using namespace Simflow;

inline void DrawParticle(float x, float y, ParticleType type) {
	float radius = 1;
	switch (type)
	{
	case Simflow::ParticleType::Iron:
		glColor4f(1, 1, 1, 0);
		break;
	case Simflow::ParticleType::Sand:
		glColor4f(1, 1, 0, 0);
		break;
	case Simflow::ParticleType::Water:
		glColor4f(0, 1, 1, 0);
		radius = 2;
		break;
	default:
		break;
	}
	glRectf(x - radius, y - radius, x + radius, y + radius);
}

inline void set_color_heat(float t)
{
	int radius = 1.0f;
	if (t > max_tep) t = max_tep;
	if (t < min_tep) t = min_tep;
	enum class degree
	{
		cold_high = 0,
		cold_medium = 1,
		cold_light = 2,
		comfort = 3,
		hot_light = 4,
		hot_medium = 5,
		hot_high = 6
	};
	degree type;
	if (t >= min_tep && t <= min_tep + 35.00f) type = degree::cold_high;
	else if (t >= min_tep + 35.00f && t <= min_tep + 70.00f) type = degree::cold_medium;
	else if (t >= min_tep + 70.00f && t <= min_tep + 115.00f) type = degree::cold_light;
	else if (t >= min_tep + 115.00f && t <= min_tep + 125.00f) type = degree::comfort;
	else if (t >= min_tep + 125.00f && t <= min_tep + 155.00f) type = degree::hot_light;
	else if (t >= min_tep + 155.00f && t <= min_tep + 185.00f) type = degree::hot_medium;
	else if (t >= min_tep + 185.00f) type = degree::hot_high;
	int choice = (int)type;
	if (choice == 0) glColor3ub(25, 25, 112);
	else if (choice == 1) glColor3ub(30, 144, 255);
	else if (choice == 2) glColor3ub(176, 224, 230);
	else if (choice == 3) glColor3ub(255, 248, 220);
	else if (choice == 4) glColor3ub(255, 222, 173);
	else if (choice == 5) glColor3ub(255, 97, 3);
	else if (choice == 6) glColor3ub(178, 34, 34);
}

inline void set_color_pressure(float p) {
	vec3 color = vec3();
	if (p > 0) color += vec3(0.5f * p - 0, 0, 0);
	if (p < 0) color += vec3(0, 0, 0 - 0.5f * p);
	glColor3f(color.x, color.y, color.z);
}
