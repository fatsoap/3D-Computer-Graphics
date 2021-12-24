#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <OpenGL/gl.h>

void display() {};
void mouseFunc(int bin, int state, int x, int y) {};
void keyboardFunc(unsigned char key, int x, int y) {};


int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
    glutCreateWindow("107502570 Lab1");

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboardFunc);
	glutMouseFunc(mouseFunc);
	gluOrtho2D(0, 500, 0, 500);
	glutMainLoop();
}