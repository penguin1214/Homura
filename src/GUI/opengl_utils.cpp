#include "GUI/opengl_utils.h"

void drawGrid(float left, float right, float bottom, float top, float dx, float dy) {
	glBegin(GL_LINES);
	for (float x = left; x <= right; x += dx) {
		glVertex2f(x, top);
		glVertex2f(x, bottom);
	}
	for (float y = bottom; y <= top; y += dy) {
		glVertex2f(left, y);
		glVertex2f(right, y);
	}
	glEnd();
}
