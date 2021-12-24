#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <iostream>
using namespace std;

int window_id = -1;
int mode = -1;
int line_buffer[2] = { -1, -1 };
int polygon_buffer[2] = { -1, -1 };
int circle_buffer[2] = { -1, -1 };

void display();
void mouseFunc(int bin, int state, int x, int y);
void keyboardFunc(unsigned char key, int x, int y);
void initBuffer();
void handleClear();
void handleReset();


void display() {};
void handleClear() {

};

void handleReset() {

};

void mouseFunc(int bin, int state, int x, int y) {};

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
};

void initBuffer() {
    line_buffer[0] = -1, line_buffer[1] = -1;
	polygon_buffer[0] = -1, polygon_buffer[1] = -1;
	circle_buffer[0] = -1, circle_buffer[1] = -1;
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