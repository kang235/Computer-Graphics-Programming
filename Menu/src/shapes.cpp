#include <stdio.h>
#include <iostream>
#include <string.h>
#include <math.h>
#include <string>
#include <vector>
#include <GL/glut.h>
#include "math/vect3d.h"
#include "shapes.h"

using namespace std;

ShapesC::ShapesC() :
	pos(Vect3d(0, 1, -1.2)),
	sca(1.f),
	rot{ 0, 0, 1, 0 },
	isSolid(false),
	color{ 255,0,0 }
{
}

void ShapesC::Render()
{
	cout << "Base class cannot render" << "\n";
}

void ShapesC::SetPos(Vect3d tmp)
{
	pos = tmp;
}

void ShapesC::SetScale(float tmp)
{
	sca = tmp;
}

void ShapesC::SetRotation(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
	rot[0] = angle;
	rot[1] = x;
	rot[2] = y;
	rot[3] = z;
}

void ShapesC::SetSolid(bool flag)
{
	isSolid = flag;
}

void ShapesC::SetColor(GLubyte r, GLubyte g, GLubyte b)
{
	color[0] = r;
	color[1] = g;
	color[2] = b;
}

void TeapotC::Render()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glColor3ubv(color);
	glTranslatef(pos.GetX(), pos.GetY(), pos.GetZ());
	glRotatef(rot[0], rot[1], rot[2], rot[3]);
	if (!isSolid)
		glutWireTeapot(sca);
	else
		glutSolidTeapot(sca);
	glPopMatrix();
}

void SphereC::Render()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glColor3ubv(color);
	glTranslatef(pos.GetX(), pos.GetY(), pos.GetZ());
	glRotatef(rot[0], rot[1], rot[2], rot[3]);
	if (!isSolid)
		glutWireSphere(sca, 20, 20);
	else
		glutSolidSphere(sca, 20, 20);
	glPopMatrix();
}

void CubeC::Render()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glColor3ubv(color);
	glTranslatef(pos.GetX(), pos.GetY(), pos.GetZ());
	glRotatef(rot[0], rot[1], rot[2], rot[3]);
	if (!isSolid)
		glutWireCube(sca);
	else
		glutSolidCube(sca);
	glPopMatrix();
}

void ConeC::Render()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glColor3ubv(color);
	glTranslatef(pos.GetX(), pos.GetY(), pos.GetZ());
	glRotatef(rot[0], rot[1], rot[2], rot[3]);
	if (!isSolid)
		glutWireCone(sca, sca * 2, 20, 20);
	else
		glutSolidCone(sca, sca * 2, 20, 20);
	glPopMatrix();
}

void TorusC::Render()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glColor3ubv(color);
	glTranslatef(pos.GetX(), pos.GetY(), pos.GetZ());
	glRotatef(rot[0], rot[1], rot[2], rot[3]);
	if (!isSolid)
		glutWireTorus(sca*0.1, sca, 20, 20);
	else
		glutSolidTorus(sca*0.1, sca, 20, 20);
	glPopMatrix();
}

