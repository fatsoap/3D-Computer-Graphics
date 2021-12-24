#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

int window_id = -1;
int mode = -1;
int line_buffer[2] = { -1, -1 };
int polygon_buffer[2] = { -1, -1 };
int circle_buffer[2] = { -1, -1 };

struct dot {
	int x;
	int y;
	bool show;
};

struct line {
	int x1, x2;
	int y1, y2;
	bool show;
};

struct polygon {
	int x1, x2;
	int y1, y2;
	bool show;
};

struct circle {
	int x1, x2;
	int y1, y2;
	bool show;
};

vector<dot> dots;
vector<line> lines;
vector<polygon> polygons;
vector<circle> circles;

void display();
void mouseFunc(int bin, int state, int x, int y);
void initBuffer();
void keyboardFunc(unsigned char key, int x, int y);
void handleDot(int bin, int state, int x, int y);
void handleLine(int bin, int state, int x, int y);
void handlePolygon(int bin, int state, int x, int y);
void handleCircle(int bin, int state, int x, int y);
void handleClear();
void handleReset();
void drawLine(int x1, int y1, int x2, int y2);
void drawCircle(int x1, int y1, int x2, int y2);

void display() {
	// set bg color
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glPointSize(1);
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POINTS);
	for (int i = 0; i < dots.size(); i++) {
		if (!dots[i].show) continue;
		glVertex2i(dots[i].x, dots[i].y);
	}
	glColor3f(0.0f, 1.0f, 0.0f);
	for (int i = 0; i < lines.size(); i++) {
		if (!lines[i].show) continue;
		drawLine(lines[i].x1, lines[i].y1, lines[i].x2, lines[i].y2);
	}
	glColor3f(0.0f, 0.0f, 1.0f);
	for (int i = 0; i < polygons.size(); i++) {
		if (!polygons[i].show) continue;
		drawLine(polygons[i].x1, polygons[i].y1, polygons[i].x2, polygons[i].y2);
	}
	glColor3f(1.0f, 1.0f, 0.0f);
	for (int i = 0; i < circles.size(); i++) {
		if (!circles[i].show) continue;
		drawCircle(circles[i].x1, circles[i].y1, circles[i].x2, circles[i].y2);
	}
	glEnd();
	glutSwapBuffers();
}

void drawCircle(int x1, int y1, int x2, int y2) {
	int r = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
	int a = 0;
	int y = (int)(r * 1.0 / sqrt(2));
	float d0 = 1.25 - r;
	float d;
	int cx = 0, cy = r;
	while (a <= y) {
		glVertex2i(x1+a, y1+cy);
		glVertex2i(x1-a, y1-cy);
		glVertex2i(x1-a, y1+cy);
		glVertex2i(x1+a, y1-cy);
		glVertex2i(x1+cy, y1+a);
		glVertex2i(x1-cy, y1-a);
		glVertex2i(x1+cy, y1-a);
		glVertex2i(x1-cy, y1+a);

		a++;
		if (d0 <= 0 ){
			d0 = d0 + 2 * a + 3;
			cy =cy;
		}else {
			d0 = d0 + 2 * (a - cy) + 5;
			cy = cy - 1;
		}
	}
}

void drawLine(int x1, int y1, int x2, int y2) {
	if (abs(x1 - x2) >= abs(y1 - y2)) {
		if (x1 == x2) { // same -> dot
			glVertex2i(x1, y1);
		}
		if (x1 > x2) { //swap
			int tmp = x1;
			x1 = x2;
			x2 = tmp;
			tmp = y1;
			y1 = y2;
			y2 = tmp;
		}
		float dir = ((float)(y2)-(float)(y1)) / ((float)(x2)-(float)(x1));
		for (int j = x1; j <= x2; j++) {
			int k = (int)((j - x1) * dir) + y1;
			glVertex2i(j, k);
		}
	}
	else {
		if (y1 > y2) { //swap
			int tmp = x1;
			x1 = x2;
			x2 = tmp;
			tmp = y1;
			y1 = y2;
			y2 = tmp;
		}
		float dir = ((float)(x2)-(float)(x1)) / ((float)(y2)-(float)(y1));
		for (int j = y1; j <= y2; j++) {
			int k = (int)((j - y1) * dir) + x1;
			glVertex2i(k, j);
		}
	}
}

void handleDot(int bin, int state, int x, int y) {
	if (bin == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		struct dot new_dot;
		new_dot.x = x;
		new_dot.y = glutGet(GLUT_WINDOW_HEIGHT) - y;
		new_dot.show = true;
		dots.push_back(new_dot);
		glutPostRedisplay();
	}
}

void handleLine(int bin, int state, int x, int y) {
	if (bin == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (line_buffer[0] == -1 && line_buffer[1] == -1) { // first
			line_buffer[0] = x, line_buffer[1] = glutGet(GLUT_WINDOW_HEIGHT) - y;
		}
		else { // second
			struct line new_line;
			new_line.x1 = line_buffer[0];
			new_line.y1 = line_buffer[1];
			new_line.x2 = x;
			new_line.y2 = glutGet(GLUT_WINDOW_HEIGHT) - y;
			new_line.show = true;
			lines.push_back(new_line);
			line_buffer[0] = -1, line_buffer[1] = -1;
			glutPostRedisplay();
		}
	}
}

void handlePolygon(int bin, int state, int x, int y) {
	if (bin == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (polygon_buffer[0] == -1 && polygon_buffer[1] == -1) { // first
			polygon_buffer[0] = x, polygon_buffer[1] = glutGet(GLUT_WINDOW_HEIGHT) - y;
		}
		else { // second
			struct polygon new_polygon;
			new_polygon.x1 = polygon_buffer[0];
			new_polygon.y1 = polygon_buffer[1];
			new_polygon.x2 = x;
			new_polygon.y2 = glutGet(GLUT_WINDOW_HEIGHT) - y;
			new_polygon.show = true;
			polygons.push_back(new_polygon);
			polygon_buffer[0] = x, polygon_buffer[1] = glutGet(GLUT_WINDOW_HEIGHT) - y;
			glutPostRedisplay();
		}
	}
}

void handleCircle(int bin, int state, int x, int y) {
	if (bin == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (circle_buffer[0] == -1 && circle_buffer[1] == -1) { // first
			circle_buffer[0] = x, circle_buffer[1] = glutGet(GLUT_WINDOW_HEIGHT) - y;
		}
	}
	if (bin == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		if (circle_buffer[0] != -1 || circle_buffer[1] != -1) { // second
			struct circle new_circle;
			new_circle.x1 = circle_buffer[0];
			new_circle.y1 = circle_buffer[1];
			new_circle.x2 = x;
			new_circle.y2 = glutGet(GLUT_WINDOW_HEIGHT) - y;
			new_circle.show = true;
			circles.push_back(new_circle);
			circle_buffer[0] = -1, circle_buffer[1] = -1;
			glutPostRedisplay();
		}
	}
}

void handleClear() {
	for (int i = 0; i < dots.size(); i++) {
		dots[i].show = false;
	}
	for (int i = 0; i < lines.size(); i++) {
		lines[i].show = false;
	}
	for (int i = 0; i < polygons.size(); i++) {
		polygons[i].show = false;
	}
	for (int i = 0; i < circles.size(); i++) {
		circles[i].show = false;
	}
	initBuffer();
	glutPostRedisplay();

}

void handleReset() {
	for (int i = 0; i < dots.size(); i++) {
		dots[i].show = true;
	}
	for (int i = 0; i < lines.size(); i++) {
		lines[i].show = true;
	}
	for (int i = 0; i < polygons.size(); i++) {
		polygons[i].show = true;
	}
	for (int i = 0; i < circles.size(); i++) {
		circles[i].show = true;
	}
	initBuffer();
	glutPostRedisplay();
}

void mouseFunc(int bin, int state, int x, int y) {
	switch (mode) {
	case 0:
		handleDot(bin, state, x, y);
		break;
	case 1:
		handleLine(bin, state, x, y);
		break;
	case 2:
		handlePolygon(bin, state, x, y);
		break;
	case 3:
		handleCircle(bin, state, x, y);
		break;
	}
}

void initBuffer() {
	line_buffer[0] = -1, line_buffer[1] = -1;
	polygon_buffer[0] = -1, polygon_buffer[1] = -1;
	circle_buffer[0] = -1, circle_buffer[1] = -1;
}

void keyboardFunc(unsigned char key, int x, int y) {
	switch (key) {
	case 'd':
		if (mode != 0) initBuffer();
		mode = 0;
		break;
	case 'l':
		if (mode != 1) initBuffer();
		mode = 1;
		break;
	case 'p':
		if (mode != 2) initBuffer();
		mode = 2;
		break;
	case 'o':
		if (mode != 3) initBuffer();
		mode = 3;
		break;
	case 'c':
		handleClear();
		break;
	case 'r':
		handleReset();
		break;
	case 'q':
		if (window_id != -1) {
			glutDestroyWindow(window_id);
			exit(0);
		}		
		break;
	}
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
    window_id = glutCreateWindow("107502570 Lab1");

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboardFunc);
	glutMouseFunc(mouseFunc);
	gluOrtho2D(0, 500, 0, 500);
	glutMainLoop();
}