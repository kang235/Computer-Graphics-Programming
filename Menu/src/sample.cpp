/********************************/
/* .net 2010 sample framework   */
/* (C) Bedrich Benes 2012       */
/* bbenes ~ at ~ purdue.edu     */
/********************************/

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <math.h>
#include <string>
#include <unordered_map>			//Standard template library class
#include <GL/glut.h>

//in house created libraries
#include "math/vect3d.h"    //for vector manipulation
#include "shapes.h"			//shapes used in this project

using namespace std;

//the main container of the shapes is a STL vector of pointers to ShapesC
//ShapesC are polymorphed to other shapes in shapes.cpp
unordered_map<string, ShapesC*> shapes;
ShapesC* currentShape;
enum MenuEnum { RED, GREEN, BLUE, NONECOLOR, TEAPOT, SPHERE, CUBE, SOLID, WIRE, CONE, TORUS, QUIT};

bool isMouseLeftButtonDown = false;
int mouseLastPosX;
float angle = 0.f;
float delta = 3.f;

//the main window size
GLint wWindow = 800;
GLint hWindow = 600;

/*********************************
Some OpenGL-related functions
**********************************/
//displays the text message in the GL window
void GLMessage(char *message)
{
	static int i;
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.f, 100.f, 0.f, 100.f);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3ub(0, 0, 255);
	glRasterPos2i(10, 10);
	for (i = 0; i < (int)strlen(message); i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, message[i]);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

//called when a window is reshaped
void Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glEnable(GL_DEPTH_TEST);
	//remember the settings for the camera
	wWindow = w;
	hWindow = h;
}


//the main rendering function
void RenderObjects()
{
	//display the ground
	glBegin(GL_QUADS);
	glColor3ub(0, 128, 0);
	glVertex3f(-100, -0.6, -100);
	glVertex3f(100, -0.6, -100);
	glVertex3f(100, -0.6, 100);
	glVertex3f(-100, -0.6, 100);
	glEnd();

	currentShape->Render();
}


//the display callback
void Display(void)
{
	glClearColor(0.9, 0.9, 0.9, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//GLMessage("Hi There");
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40, (GLfloat)wWindow / (GLfloat)hWindow, 0.01, 100); //set the camera
	glMatrixMode(GL_MODELVIEW); //set the scene
	glLoadIdentity();
	glTranslatef(0, -1, -5);      //move the scena a little bit back and down
	RenderObjects();			  //render the scene
	glutSwapBuffers();
}

//keyboard callback
void Kbd(unsigned char a, int x, int y)
{
	switch (a)
	{
	case 27: exit(0); break;
	}
	glutPostRedisplay();
}


//special keyboard callback
void SpecKbdPress(int a, int x, int y)
{
	switch (a)
	{
	case GLUT_KEY_LEFT:
	{
		break;
	}
	case GLUT_KEY_RIGHT:
	{
		break;
	}
	case GLUT_KEY_DOWN:
	{
		break;
	}
	case GLUT_KEY_UP:
	{
		break;
	}

	}
	glutPostRedisplay();
}

//called when a special key is released
void SpecKbdRelease(int a, int x, int y)
{
	switch (a)
	{
	case GLUT_KEY_LEFT:
	{
		break;
	}
	case GLUT_KEY_RIGHT:
	{
		break;
	}
	case GLUT_KEY_DOWN:
	{
		break;
	}
	case GLUT_KEY_UP:
	{
		break;
	}
	}
	glutPostRedisplay();
}

void Mouse(int button, int mState, int x, int y)
{
	//cout << "Location is " << "[" << x << "'" << y << "]" << endl;

	if (mState == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
	{
		isMouseLeftButtonDown = true;
		mouseLastPosX = x;
	}
	else
	{
		isMouseLeftButtonDown = false;
	}
}

void Motion(int x, int y)
{
	//cout << "Location is " << "[" << x << "'" << y << "]" << endl;

	if (isMouseLeftButtonDown)
	{
		int deltaX = mouseLastPosX - x;
		mouseLastPosX = x;

		for (unsigned i = 0; i < shapes.size(); ++i)
		{
			if (deltaX > 0)
				angle += delta;
			else if (deltaX < 0)
				angle -= delta;
			currentShape->SetRotation(angle, 0, 1, 0);
		}
		glutPostRedisplay();
	}
}

void PassiveMotion(int x, int y)
{
}

//Initialization of the objects
//each object is created in dynamic memory (using new) 
//pointer to it is added to the ShapeC* container
void InitShapes()
{
	TeapotC *teapot;
	SphereC *sphere;
	CubeC   *cube;
	ConeC   *cone;
	TorusC  *torus;

	teapot = new(TeapotC);
	shapes["teapot"] = teapot;
	currentShape = teapot;

	sphere = new(SphereC);
	shapes["sphere"] = sphere;

	cube = new(CubeC);
	shapes["cube"] = cube;

	cone = new(ConeC);
	shapes["cone"] = cone;

	torus = new(TorusC);
	shapes["torus"] = torus;
}

void Menu(int val) {
	switch (val)
		{
		case RED: 
			currentShape->SetColor(255, 0, 0); 
			break;
		case GREEN:
			currentShape->SetColor(0, 255, 0);
			break;
		case BLUE:
			currentShape->SetColor(0, 0, 255);
			break;
		case NONECOLOR:
			currentShape->SetColor(0, 0, 0);
			break;	
		case TEAPOT:
			currentShape = shapes["teapot"];
			break;
		case SPHERE:
			currentShape = shapes["sphere"];
			break;
		case CUBE:
			currentShape = shapes["cube"];
			break;
		case CONE:
			currentShape = shapes["cone"];
			break;
		case TORUS:
			currentShape = shapes["torus"];
			break;
		case SOLID:
			currentShape->SetSolid(true);
			break;
		case WIRE:
			currentShape->SetSolid(false);
			break;
		case QUIT:
			exit(0);
			break;
		}
	glutPostRedisplay();
}

void InitMenu() {
	int s1, s2, s3;

	s1 = glutCreateMenu(Menu);
	glutAddMenuEntry("R", RED);
	glutAddMenuEntry("G", GREEN);
	glutAddMenuEntry("B", BLUE);
	glutAddMenuEntry("None", NONECOLOR);

	s2 = glutCreateMenu(Menu); 
	glutAddMenuEntry("Teapot", TEAPOT);
	glutAddMenuEntry("Sphere", SPHERE);
	glutAddMenuEntry("Cube", CUBE);
	glutAddMenuEntry("Cone", CONE);
	glutAddMenuEntry("Torus", TORUS);

	s3 = glutCreateMenu(Menu);
	glutAddMenuEntry("Solid", SOLID);
	glutAddMenuEntry("Wire", WIRE);

	glutCreateMenu(Menu);
	glutAddSubMenu("Color", s1);
	glutAddSubMenu("Object", s2);
	glutAddSubMenu("Render", s3);
	glutAddMenuEntry("Quit", QUIT);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(wWindow, hWindow);
	glutCreateWindow("Lab 2");
	glutIgnoreKeyRepeat(GL_FALSE);
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutIgnoreKeyRepeat(1);//YES, ignore key repeat please
	glutKeyboardFunc(Kbd); //+ and -
	glutSpecialUpFunc(SpecKbdRelease); //smooth motion
	glutSpecialFunc(SpecKbdPress);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutPassiveMotionFunc(PassiveMotion);
	InitShapes();
	InitMenu();
	glutMainLoop();
	return 0;
}
