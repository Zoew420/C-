#include <iostream>
#include "imgui/glew/GL/glew.h"
#define max_tep 1773.15f
#define min_tep 0.00f
#define max_pre 1.00f
#define min_pre -1.00f


inline void DrawPaticle(float x, float y, float radius, int type) 
{
    if (type == 1) glColor4f(1, 1, 1, 0);
    if (type == 2) glColor4f(1, 1, 0, 0);
	if (type == 3) glColor4f(0, 1, 1, 0);
    glRectf(x - radius, y - radius, x + radius, y + radius);
}

inline void DrawHeat(float x, float y, float radius, float temperature) 
{
	if (temperature > max_tep) temperature = max_tep;
	if (temperature < min_tep) temperature = min_tep;
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
	if (temperature >= min_tep && temperature <= min_tep + 150.00f) type = degree::cold_high;
	else if (temperature >= min_tep + 150.00f && temperature <= min_tep + 230.00f) type = degree::cold_medium;
	else if (temperature >= min_tep + 230.00f && temperature <= min_tep + 288.00f) type = degree::cold_light;
	else if (temperature >= min_tep + 288.00f && temperature <= min_tep + 293.00f) type = degree::comfort;
	else if (temperature >= min_tep + 293.00f && temperature <= min_tep + 600.00f) type = degree::hot_light;
	else if (temperature >= min_tep + 600.00f && temperature <= min_tep + 1200.00f) type = degree::hot_medium;
	else if (temperature >= min_tep + 1200.00f) type = degree::hot_high;
	int choice = (int)type;
	if (choice == 0) glColor3ub(25, 25, 112);
	else if (choice == 1) glColor3ub(30, 144, 255);
	else if (choice == 2) glColor3ub(176, 224, 230);
	else if (choice == 3) glColor3ub(255, 248, 220);
	else if (choice == 4) glColor3ub(255, 222, 173);
	else if (choice == 5) glColor3ub(255, 97, 3);
	else if (choice == 6) glColor3ub(178, 34, 34);
	glRectf(x - radius, y - radius, x + radius, y + radius);
}

inline void DrawPressure(float x, float y, float radius, float pressure)
{
	if (pressure > max_pre) pressure = max_pre;
	if (pressure < min_pre) pressure = min_pre;
	enum class degree 
	{
		low_high = 0,
		low_medium = 1,
		low_light = 2,
		comfort = 3,
		high_low = 4,
		high_medium = 5,
		high_high = 6
	};
	degree type;
	if (pressure >= min_pre && pressure <= min_pre + 0.30f) type = degree::low_high;
	else if (pressure >= min_pre + 0.30f && pressure <= min_pre + 0.60f) type = degree::low_medium;
	else if (pressure >= min_pre + 0.60f && pressure <= min_pre + 0.90f) type = degree::low_light;
	else if (pressure >= min_pre + 0.90f && pressure <= min_pre + 1.10f) type = degree::comfort;
	else if (pressure >= min_pre + 1.10f && pressure <= min_pre + 1.40f) type = degree::high_low;
	else if (pressure >= min_pre + 1.40f && pressure <= min_pre + 1.70) type = degree::high_medium;
	else if (pressure >= min_pre + 1.70f) type = degree::high_high;
	int choice = (int)type;
	if (choice == 0) glColor3ub(128, 138, 135);
	else if (choice == 1) glColor3ub(192, 192, 192);
	else if (choice == 2) glColor3ub(220, 220, 220);
	else if (choice == 3) glColor3ub(248, 248, 255);
	else if (choice == 4) glColor3ub(176, 224, 230);
	else if (choice == 5) glColor3ub(135, 206, 235);
	else if (choice == 6) glColor3ub(51, 161, 201);
	glRectf(x - radius, y - radius, x + radius, y + radius);
}