#include <stdio.h>
#include <iostream>
#include <string.h>
#include <math.h>
#include <string>
#include <vector>
#include <GL/glut.h>
#include "math/vect3d.h"

using namespace std;

//Base class for shapes
class ShapesC
{
public:
	ShapesC();
	virtual void SetPos(Vect3d tmp);
	virtual void SetScale(float tmp);
	virtual void SetRotation(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
	virtual void SetSolid(bool flag);
	virtual void SetColor(GLubyte r,GLubyte b,GLubyte g);
	virtual void Render();

protected:
  Vect3d  pos;
  float   sca;
  float   rot[4];
  bool	  isSolid;
  GLubyte color[3];
};

//derived class from ShapesC
class TeapotC: public ShapesC
{
public:
	virtual void Render();		
};

//derived class from ShapesC
class SphereC: public ShapesC
{
public:
	virtual void Render();		
};

//derived class from ShapesC
class CubeC : public ShapesC
{
public:
	virtual void Render();
};

//derived class from ShapesC
class ConeC : public ShapesC
{
public:
	virtual void Render();
};

//derived class from ShapesC
class TorusC : public ShapesC
{
public:
	virtual void Render();
};


