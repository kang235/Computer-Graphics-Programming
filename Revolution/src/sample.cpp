/********************************/
/* surface of revolution        */
/* (C) Bedrich Benes 2012       */
/* bbenes ~ at ~ purdue.edu     */
/********************************/

#include <stdio.h>
#include <iostream>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include <string>
#include <vector>			//Standard template library class
#include <GL/glew.h>
#include <GL/glut.h>
//in house created libraries
#include "math/vect3d.h"    //for vector manipulation

#pragma warning(disable : 4996)
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glut32.lib")

using namespace std;

static GLfloat angle = 0;
enum AxisEnum { AX, AY, AZ };
AxisEnum axis = AX;
bool isCurve = false;

struct BezierPoints
{
	Vect3d p0 = Vect3d(0.8, 0.6, 0);
	Vect3d p1 = Vect3d(0.2, 0.5, 0);
	Vect3d p2 = Vect3d(0.8, 0.2, 0);
	Vect3d p3 = Vect3d(0.2, 0.1, 0);
} bcurve;

GLuint points = 0; //number of points to display the object
int steps = 30;//# of recursive subdivisions
bool needRedisplay = false;
GLfloat  sign = +1;
const GLfloat defaultIncrement = 0.2;
GLfloat  angleIncrement = defaultIncrement;


//the main window size
GLint wWindow = 800;
GLint hWindow = 600;

inline void AddVertex(vector <GLfloat> *a, const GLfloat *A)
{
	a->push_back(A[0]); a->push_back(A[1]); a->push_back(A[2]);
}

Vect3d GetBezierCurvePoint(float t, BezierPoints curve)
{
	float b0, b1, b2, b3;
	Vect3d point;

	b0 = (1 - t)*(1 - t)*(1 - t);
	b1 = 3 * t*(1 - t)*(1 - t);
	b2 = 3 * t *t*(1 - t);
	b3 = t * t * t;

	point.SetX(curve.p0.GetX()*b0 + curve.p1.GetX()*b1 + curve.p2.GetX()*b2 + curve.p3.GetX()*b3);
	point.SetY(curve.p0.GetY()*b0 + curve.p1.GetY()*b1 + curve.p2.GetY()*b2 + curve.p3.GetY()*b3);

	return point;
}

//default
GLfloat P(GLfloat u)
{
	return 0.2*sin(4 * M_PI*u) + 1.f;
}

//sklenicka
GLfloat P1(GLfloat u)
{
	u = 1 - u;
	return (0.2*sin(2 * M_PI*u)*(u + 0.2) + 0.2);
}

//sphere
GLfloat P2(GLfloat u)
{
	u = 1 - 2 * u;//reparametrize to <-1,1>
	return (sqrt(1.f - u*u));
}

inline Vect3d S(GLfloat u, GLfloat v, GLfloat(*f)(GLfloat))
{
	Vect3d r;

	switch (axis)
	{
	case AX:
		r = Vect3d((*f)(u), u*sin(2 * M_PI*v), u*cos(2 * M_PI*v));
		break;
	case AY:
		r = Vect3d((*f)(u)*sin(2 * M_PI*v), u, (*f)(u)*cos(2 * M_PI*v));
		break;
	case AZ:
		float radius = sqrt((*f)(u)*(*f)(u) + u*u);
		r = Vect3d(radius*cos(2 * M_PI*v), radius*sin(2 * M_PI*v), 0);
		break;
	}

	return r;
}

//for bezier curve
inline Vect3d S(GLfloat u, GLfloat v)
{
	Vect3d r;
	Vect3d point = GetBezierCurvePoint(u, bcurve);

	switch (axis)
	{
	case AX:
		r = Vect3d(point.GetX(), point.GetY()*sin(2 * M_PI*v), point.GetY()*cos(2 * M_PI*v));
		break;
	case AY:
		r = Vect3d(point.GetX()*sin(2 * M_PI*v), point.GetY(), point.GetX()*cos(2 * M_PI*v));
		break;
	case AZ:
		float radius = sqrt(point.GetX()*point.GetX() + point.GetY()*point.GetY());
		r = Vect3d(radius*cos(2 * M_PI*v), radius*sin(2 * M_PI*v), 0);
		break;
	}

	return r;
}

void CreateRevo(vector <GLfloat> *a, int n, bool drawCurve)
{
	GLfloat step = 1.f / n;

	if (!drawCurve)
	{
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				//lower triangle
				AddVertex(a, S(i*step, j*step));
				AddVertex(a, S((i + 1)*step, j*step));
				AddVertex(a, S((i + 1)*step, (j + 1)*step));
				//upper triangle
				AddVertex(a, S(i*step, j*step));
				AddVertex(a, S((i + 1)*step, (j + 1)*step));
				AddVertex(a, S(i*step, (j + 1)*step));
			}
		}
	}
	else
	{
		for (int i = 0; i < n; i++)
		{
			Vect3d p = GetBezierCurvePoint(i*step, bcurve);
			GLfloat point[3] = { p.GetX(), p.GetY(), 0 };
			AddVertex(a, point);
		}
	}
}

void InitArray(int n, bool drawCurve)
{
	GLuint vaID;

	vector<GLfloat> v;
	CreateRevo(&v, n, drawCurve);
	glGenVertexArrays(1, &vaID);
	glBindVertexArray(vaID);

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	points = v.size();
	glBufferData(GL_ARRAY_BUFFER, points*sizeof(GLfloat), &v[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	v.clear(); //no need for the data, it is on the GPU now
}



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
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(0, 0, 1);
	glPointSize(2);
	glLineWidth(1);
	//glDrawArrays(GL_TRIANGLES, 0, points / 3);
	glDrawArrays(GL_LINE_STRIP, 0, points / 3);
}


void Idle(void)
{
	angle += angleIncrement;
	if (angle >= 360.f) angle = 0.f;
}

void Display(void)
{
	glClearColor(1.9, 1.9, 1.9, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLMessage("Surface of revolution, press 'c' to toggle to curve");
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40, (GLfloat)wWindow / (GLfloat)hWindow, 0.01, 100); //set the camera
	glMatrixMode(GL_MODELVIEW); //set the scene
	glLoadIdentity();
	gluLookAt(0, 2, 5, 0, 0, 0, 0, 1, 0);
	glRotatef(sign*angle, 0, 1, 0);
	RenderObjects();
	glutSwapBuffers();
	glutPostRedisplay();
}

void Kbd(unsigned char a, int x, int y)//keyboard callback
{
	switch (a)
	{
	case 27: exit(0); break;
	case 32: {
		if (angleIncrement == 0) angleIncrement = defaultIncrement;
		else angleIncrement = 0;
		break;
	}
	case 's': {sign = -sign; break; }
	case '-':
	case '_': {
		steps--;
		if (steps < 1) steps = 1;
		InitArray(steps, isCurve);
		break;
	}
	case '+':
	case '=': {
		steps++;
		InitArray(steps, isCurve);
		break;
	}
	case 'x':
	case 'X': {
		cout << "Revolution around the X axis" << endl;
		axis = AX;
		InitArray(steps, isCurve);
		break;
	}
	case 'y':
	case 'Y': {
		cout << "Revolution around the Y axis" << endl;
		axis = AY;
		InitArray(steps, isCurve);
		break;
	}
	case 'z':
	case 'Z': {
		cout << "Revolution around the Z axis" << endl;
		axis = AZ;
		InitArray(steps, isCurve);
		break;
	}
	case 'c':
	case 'C': {
		cout << "Toggle isCurve" << endl;
		isCurve = !isCurve;
		InitArray(steps, isCurve);
		break;
	}
	}

	cout << "[steps]=[" << steps << "]" << endl;
	glutPostRedisplay();
}


int main(int argc, char **argv)
{
	glutInitDisplayString("stencil>=2 rgb double depth samples");
	glutInit(&argc, argv);
	glutInitWindowSize(wWindow, hWindow);
	glutInitWindowPosition(500, 100);
	glutCreateWindow("Surface of Revolution");
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	glutDisplayFunc(Display);
	glutIdleFunc(Idle);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Kbd); //+ and -
	glutMouseFunc(NULL);
	glutSpecialUpFunc(NULL);
	glutSpecialFunc(NULL);
	InitArray(steps, isCurve);
	glutMainLoop();
	return 0;
}
