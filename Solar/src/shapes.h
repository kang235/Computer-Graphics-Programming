#pragma once

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <math.h>
#include <string>
#include <vector>
#include <GL/glut.h>
#include <glm/glm.hpp>


using namespace std;

//Base class for shapes
class ShapesC
{
public:
	virtual void SetMatrixParamToShader(GLuint uniform);
	virtual void SetMVP(glm::mat4 tmp);
	virtual void SetColorParamToShader(GLuint uniform);
	virtual void SetColor(GLfloat r, GLfloat b, GLfloat g, GLfloat a);
	virtual void Render();
	void AddVertex(vector <GLfloat> *a, const glm::vec3 *v);

protected:
  GLuint	 modelviewParameter; //shader uniform variable
  glm::mat4  mvp;				 //modelview matrix
  GLuint	 colorParameter; //shader uniform variable
  GLfloat    color[4];
};

//derived class from ShapesC
class SphereC: public ShapesC
{
public:
	SphereC();
	SphereC(int stacks, int slices, GLfloat r);
	virtual void Render();		

private:
	vector <GLfloat> data;
	GLuint vaID;
	GLuint buffer;
	GLuint points;
	GLuint stacks, slices;
	GLfloat r;
	void SphereC::Generate(int stacks, int slices, GLfloat r);
	void SphereC::InitArrays();
};

//derived class from ShapesC
class CubeC: public ShapesC
{
public:
	CubeC();
	virtual void Render();		
private:
	vector <GLfloat> data;
	GLuint vaID;
	GLuint buffer;
	GLuint points;
	void Generate();
	void InitArrays();
};

