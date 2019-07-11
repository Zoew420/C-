#include <iostream>
#include "imgui/glew/GL/glew.h"
#define PI 3.14159265358979323846

inline void DrawPaticle(float x, float y, float radius, int type) {
    // 画圆有点降FPS，暂时改成方形
    int count;
    int sections = 200;
    if (type == 1) glColor4f(1, 1, 1, 0);
    if (type == 2) glColor4f(1, 1, 0, 0);
    if (type == 3) {
        glColor4f(0, 1, 1, 0);
        //radius = 3;
    }
    glRectf(x - radius, y - radius, x + radius, y + radius);

    //if (type == 1) {
    //	glColor4f(1, 1, 0, 0);
    //	glBegin(GL_TRIANGLE_FAN);
    //	glVertex2f(x, y);
    //	for (count = 0; count <= sections; count++)
    //	{
    //		glVertex2f(x + radius * cos(count * 2 * PI / sections), y + radius * sin(count * 2 * PI / sections));
    //	}
    //	glEnd();
    //}
    //else if (type == 2) {
    //	glColor4f(1, 1, 1, 0);
    //	glBegin(GL_TRIANGLE_FAN);
    //	glVertex2f(x, y);
    //	for (count = 0; count <= sections; count++)
    //	{
    //		glVertex2f(x + radius * cos(count * 2 * PI / sections), y + radius * sin(count * 2 * PI / sections));
    //	}
    //	glEnd();
    //}
}