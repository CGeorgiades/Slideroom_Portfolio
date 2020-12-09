#ifndef MY_DRAW_H
#define MY_DRAW_H
#ifdef __APPLE__
#include "GLUT/glut.h"
#else
#include <GL/glut.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
static void rectangle(double x, double y, double width, double height) {
	glBegin(GL_POLYGON);
	glVertex3f(x, y, 0);
	glVertex3f(x + width, y, 0);
	glVertex3f(x + width, y + width, 0);
	glVertex3f(x, y + width, 0);
	glEnd();
}
static void circle_fill(double cx, double cy, double z, double radius,
    int sidect) {
	glBegin(GL_POLYGON);
	double theta;
	for (int i = 0; i < sidect; i = i + 1) {
		theta = i * 2 * M_PI / sidect;
		glVertex3f(cx + radius * cos(theta), cy + radius * sin(theta), z);
	}
	glEnd();
}
static void circle_out(double cx, double cy, double z, double radius,
    int sidect) {
	glBegin(GL_LINE_LOOP);
	double theta;
	for (int i = 0; i < sidect; i = i + 1) {
		theta = i * 2 * M_PI / sidect;
		glVertex3f(cx + radius * cos(theta), cy + radius * sin(theta), z);
	}
	glEnd();
}
static void poly(double cx, double cy, double z, double radius, double sidect,
    double rotation) {
	glBegin(GL_POLYGON);
	double theta;
	for (int i = 0; i < sidect; i = i + 1) {
		theta = i * 2 * M_PI / sidect;
		glVertex3f(cx + radius * cos(theta + rotation),
		    cy + radius * sin(theta + rotation), z);
	}
	glEnd();
}
static void line2(double x1, double y1, double x2, double y2, double z = 0) {
	glBegin(GL_LINE_LOOP);
	glVertex3f(x1, y1, z);
	glVertex3f(x2, y2, z);
	glEnd();
}
static void line3(double x1, double y1, double z1, double x2, double y2,
    double z2) {
	glBegin(GL_LINE_LOOP);
	glVertex3f(x1, y1, z1);
	glVertex3f(x2, y2, z2);
	glEnd();
}
class drawingobj {
public:
	bool isHoveredOver = false;
public:
	drawingobj() {

	}

public:
	virtual void draw(void) {

	}
public:
	virtual bool isMouseHovering(float x, float y) {
		return false;
	}
};
static drawingobj *all_draw_objs[1000];
static unsigned int next_obj_index;

class button: public drawingobj {
public:
	double x, y, width, height;
public:
	button(double x, double y, double width, double height) :
			drawingobj() {
		all_draw_objs[next_obj_index] = this;
		next_obj_index++;
	}
public:
	void draw(void) {
		if (isHoveredOver) {
			glColor3f(1, 0, 0);
		} else {
			glColor3f(0, 1, 0);
		}
		rectangle(x, y, width, height);
	}
public:
	bool isMouseHovering(float x, float y) {
		return (x > this->x && x < this->x + width && y > this->y
		    && y < this->y + height);
	}

};
class slider: public drawingobj {
	double x, y, height, width;
public:
	double position;
public:
	slider(double cenx, double ceny, double width, double height) :
			drawingobj() {
		all_draw_objs[next_obj_index] = this;
		next_obj_index++;
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
	}
public:
	void draw(void) {

	}
public:
	bool isMouseHovering(float x, float y) {
		return (x > this->x && x < this->x + width && y > this->y
		    && y < this->y + height);
	}
};
static void mouse_drawing_objs(float mousex, float mousey) {

	for (int i = 0; i < next_obj_index; i++) {
		all_draw_objs[i]->isHoveredOver = all_draw_objs[i]->isMouseHovering(mousex,
		    mousey);
	}
}
static void draw_drawing_objs(void) {
	for (int i = 0; i < next_obj_index; i++) {
		all_draw_objs[i]->draw();
	}
}
;

#endif /* MY_DRAW_H */
