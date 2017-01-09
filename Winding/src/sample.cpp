/********************************/
/* .net 2015 Math Example       */
/* (C) Bedrich Benes 2016       */
/* bbenes ~ at ~ purdue.edu     */
/********************************/

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <math.h>
#include <time.h>
#include <string>
#include <vector>			//Standard template library class
#include <GL/glut.h>
#pragma warning(disable : 4996)

//in house created libraries
#include "math/vect3d.h"    //for vector manipulation

using namespace std;

bool needRedisplay = false;

enum State { POLY_DRAWING, TESTPOINT };
State state = POLY_DRAWING;
const char *prompt[2] = { "Use left/right click to draw/complete a polygon;", "Use left click to change the test point position; Press 'r' to restart a test;" };

//the main container of the polylines is a STL vector of pointers to 
vector <Vect3d> points;
Vect3d testPoint;

//the main window size
GLint wWindow = 800;
GLint hWindow = 600;

/*********************************
Some OpenGL-related functions
**********************************/
//displays the text message in the GL window
void GLMessage(char *message, int line)
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
	glRasterPos2i(10, line * 5);
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

void DisplayPolygon(vector <Vect3d> &points, State state) {
	glColor3ub(0, 0, 255);
	//if the polygon is under construction 
	if (state == POLY_DRAWING)
	{
		//show the first point
		if (points.size() == 1)
		{
			glPointSize(4);
			glBegin(GL_POINTS);
			glVertex3fv(points[0]);
			glEnd();
		}
		else
		{
			glLineWidth(4);
			glBegin(GL_LINE_STRIP);
			for (unsigned int i = 0; i < points.size(); i++)
				glVertex3fv(points[i]);
			glEnd();
		}
	}
	//if the polygon is completed
	else
	{
		glLineWidth(4);
		glBegin(GL_LINE_LOOP);
		for (unsigned int i = 0; i < points.size(); i++)
			glVertex3fv(points[i]);
		glEnd();
	}
}

void DisplayPoint(Vect3d p) {
	glColor3ub(0, 0, 0);
	glPointSize(4);
	glBegin(GL_POINTS);
	glVertex3fv(p);
	glEnd();
}

// Winding(): winding angle test for a point in a polygon
//      Input:   points = vertex points of a polygon 
//               testPoint = a point
//      Return:  true for inside the polygon
//				 false for outside the polygon
bool Winding(vector <Vect3d> points, Vect3d testPoint) {
	float eps = 0.0001f;
	float angleSum = 0.f;
	//vectors from test point to points
	vector<Vect3d> tp2p;

	for (unsigned i = 0; i < points.size(); ++i)
		tp2p.push_back(points[i] - testPoint);
	tp2p.push_back(tp2p[0]);

	//sum up the angles subtended by each side of the polygon
	for (unsigned i = 0; i < tp2p.size() - 1; ++i)
	{
		float cosa = tp2p[i].Dot(tp2p[i + 1]) / (tp2p[i].Length()*tp2p[i + 1].Length());
		//to determin cw or ccw
		Vect3d v = tp2p[i].Cross(tp2p[i + 1]);
		//sign
		int s = v.GetZ() > 0 ? 1 : -1;
		angleSum += s*acos(cosa);
	}

	if (fabs(angleSum) < eps)
		return false;
	return true;
}

//A more efficient way - modified from the code in the following page
//http://geomalgorithms.com/a03-_inclusion.html
// isLeft(): tests if a point is Left|On|Right of an infinite line.
//    Input:  three points P0, P1, and P2
//    Return: >0 for P2 left of the line through P0 and P1
//            =0 for P2  on the line
//            <0 for P2  right of the line
inline int isLeft(Vect3d P0, Vect3d P1, Vect3d P2)
{
	return ((P1.GetX() - P0.GetX()) * (P2.GetY() - P0.GetY())
		- (P2.GetX() - P0.GetX()) * (P1.GetY() - P0.GetY()));
}

// Winding2(): winding number test for a point in a polygon
//      Input:   points = vertex points of a polygon
//               testPoint = a point
//      Return:  true for inside the polygon
//				 false for outside the polygon
bool Winding2(vector <Vect3d> points, Vect3d testPoint)
{
	int   wn = 0;    // the  winding number counter
	// loop through all edges of the polygon
	for (unsigned i = 0; i < points.size() - 1; i++) {   // edge from V[i] to  V[i+1]
		if (points[i].GetY() <= testPoint.GetY()) {          // start y <= P.y
			if (points[i + 1].GetY()  > testPoint.GetY())      // an upward crossing
				if (isLeft(points[i], points[i + 1], testPoint) > 0)  // P left of  edge
					++wn;            // have  a valid up intersect
		}
		else {                        // start y > P.y (no test needed)
			if (points[i + 1].GetY() <= testPoint.GetY())     // a downward crossing
				if (isLeft(points[i], points[i + 1], testPoint) < 0)  // P right of  edge
					--wn;            // have  a valid down intersect
		}
	}

	if (wn == 0) return false;
	return true;
}

void clean()
{
	state = POLY_DRAWING;
	points.clear();
	testPoint.Set(0, 0, 0);
}

void Idle(void)
{
	static clock_t tStart = 0, tEnd = 1;
	tStart = clock();
	double fps = 1.f / ((float)tEnd - tStart);
	tEnd = clock();
}


void Display(void)
{
	glClearColor(0.5, 0.5, 0.5, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-10, 10, -10, 10, 0, 10); //set the camera
	glMatrixMode(GL_MODELVIEW); //set the scene
	glLoadIdentity();
	if (state == TESTPOINT)
		DisplayPoint(testPoint);
	DisplayPolygon(points, state);

	char message[200];
	if (state == TESTPOINT)
	{
		sprintf(message, "Is the point inside? %s", Winding(points, testPoint) ? "true" : "false");
		//sprintf(message, "Is the point inside? %s", Winding2(points, testPoint) ? "true" : "false");
		GLMessage(message, 1);
	}
	sprintf(message, "%s", state == POLY_DRAWING ? prompt[0] : prompt[1]);
	GLMessage(message, 2);

	glutSwapBuffers();
}

//keyboard callback
void Kbd(unsigned char a, int x, int y)
{
	cout << "The key you have pressed is " << a << endl;
	switch (a)
	{
	case 27: exit(0); break;
	case 32: {needRedisplay = !needRedisplay; break; }
	case 'r': clean(); break;
	}
	int modifier = glutGetModifiers();
	switch (modifier)
	{
	case GLUT_ACTIVE_SHIFT: cout << "shift was pressed "; break;
	case GLUT_ACTIVE_ALT:   cout << "alt was pressed "; break;
	case GLUT_ACTIVE_CTRL:  cout << "ctrl was pressed "; break;
	case GLUT_ACTIVE_SHIFT | GLUT_ACTIVE_ALT:  cout << "shift AND alt were pressed "; break;
	default: break;
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

//Initialization of the polygon
void InitShapes()
{
}

void Mouse(int button, int mState, int x, int y)
{
	//cout << "Location is " << "[" << x << "'" << y << "]" << endl; 

	if (mState == GLUT_DOWN) return;

	if (button == GLUT_LEFT_BUTTON)
	{
		//convert screen coordinates to gl coordinates
		GLdouble modelMatrix[16];
		GLdouble projMatrix[16];
		GLint viewport[4];

		glGetIntegerv(GL_VIEWPORT, viewport);
		glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
		glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);

		GLdouble xx, yy, zz;
		gluUnProject(x, viewport[3] - y, 0, modelMatrix, projMatrix, viewport, &xx, &yy, &zz);

		if (state == TESTPOINT)
		{
			testPoint.Set(xx, yy, 0);
		}
		else
		{
			points.push_back(Vect3d(xx, yy, 0));
		}
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == POLY_DRAWING)
			state = TESTPOINT;
	}

	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	//  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitDisplayString("stencil>=2 rgb double depth samples");
	glutInit(&argc, argv);
	glutInitWindowSize(wWindow, hWindow);
	glutInitWindowPosition(500, 100);
	glutCreateWindow("Winding");
	glutIgnoreKeyRepeat(GL_FALSE);
	glutDisplayFunc(Display);
	glutIdleFunc(Idle);
	glutMouseFunc(Mouse);
	glutReshapeFunc(Reshape);
	glutIgnoreKeyRepeat(1);//YES, ignore key repeat please
	glutKeyboardFunc(Kbd); //+ and -
	glutSpecialUpFunc(SpecKbdRelease); //smooth motion
	glutSpecialFunc(SpecKbdPress);
	InitShapes();
	glutMainLoop();
	return 0;
}
