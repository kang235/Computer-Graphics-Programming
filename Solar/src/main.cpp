/**********************************/
/* Simple transformations	      */
/* (C) Bedrich Benes 2012         */
/* bbenes ~ at ~ purdue.edu       */
/**********************************/

#include <algorithm>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include <string>
#include <vector>			//Standard template library class
#include <unordered_map>
#include <GL/glew.h>
#include <GL/glut.h>
//glm
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
//in house created libraries
#include "shaders.h"    
#include "shapes.h"    
#include "teapot.h"

#pragma warning(disable : 4996)
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glut32.lib")

using namespace std;

bool needRedisplay = false;
GLint stacks = 5, slices = 5;
unordered_map<std::string, ShapesC*> solarmap;

//shader program ID
GLuint shaderProgram;
GLfloat ftime = 0.f;
GLint ftimeParameter;

glm::mat4 view = glm::mat4(1.0);
glm::mat4 proj = glm::perspective(80.0f,//fovy
	1.0f,//aspect
	0.01f, 1000.f); //near, far
GLint modelParameter;
GLint viewParameter;
GLint projParameter;
GLint colorParameter;

GLfloat xa = 0.f;
GLfloat dxa = 0.1f;
GLfloat XA_MAX = M_PI / 2;
GLfloat z = 50.f;

//the main window size
GLint wWindow = 800;
GLint hWindow = 800;

/*********************************
Some OpenGL-related functions
**********************************/
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
	glColor3f(0, 0, 0);
	glPointSize(1);
	glLineWidth(1);

	//set the projection and view once for the scene
	glUniformMatrix4fv(projParameter, 1, GL_FALSE, glm::value_ptr(proj));
	view = glm::lookAt(glm::vec3(z * cos(xa), z * sin(xa), 0.f),//eye
		glm::vec3(0, 0, 0),  //destination
		glm::vec3(0, 1, 0)); //up

	glUniformMatrix4fv(viewParameter, 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 m = glm::mat4(1.0);

	m=glm::rotate(m,30.f*ftime,glm::vec3(0,0,1));
	solarmap["sun"]->SetMVP(m);
	solarmap["sun"]->Render();

	m = glm::mat4(1.0);
	m = glm::rotate(m, 200.0f*ftime, glm::vec3(0.0, 1.0, 0.0));
	m = glm::translate(m, glm::vec3(4.f, 0.f, 0.f));
	m = glm::rotate(m, 60.0f*ftime, glm::vec3(0.0, 1.0, 0.0));
	m = glm::scale(m, glm::vec3(0.38f, 0.38f, 0.38f));
	solarmap["mercury"]->SetMVP(m);
	solarmap["mercury"]->Render();

	m = glm::mat4(1.0);
	m = glm::rotate(m, 100.0f*ftime, glm::vec3(0.0, 1.0, 0.0));
	m = glm::translate(m, glm::vec3(7.f, 0.f, 0.f));
	m = glm::rotate(m, 250.0f*ftime, glm::vec3(0.0, 1.0, 0.0));
	m = glm::scale(m, glm::vec3(0.95f, 0.95f, 0.95f));
	solarmap["venus"]->SetMVP(m);
	solarmap["venus"]->Render();

	m = glm::mat4(1.0);
	m = glm::rotate(m, 36.0f*ftime, glm::vec3(0.0, 1.0, 0.0));
	m = glm::translate(m, glm::vec3(10.f, 0.f, 0.f));
	m = glm::rotate(m, ftime, glm::vec3(0.0, 1.0, 0.0));
	solarmap["earth"]->SetMVP(m);
	solarmap["earth"]->Render();

	m = glm::rotate(m, 72.0f*ftime, glm::vec3(0.0, 1.0, 0.0));
	m = glm::translate(m, glm::vec3(1.5f, 0.f, 0.f));
	m = glm::rotate(m, 2.f*ftime, glm::vec3(0.0, 1.0, 0.0));
	m = glm::scale(m, glm::vec3(0.1f, 0.1f, 0.1f));
	solarmap["moon"]->SetMVP(m);
	solarmap["moon"]->Render();

	m = glm::mat4(1.0);
	m = glm::rotate(m, 100.0f*ftime, glm::vec3(0.0, 1.0, 0.0));
	m = glm::translate(m, glm::vec3(15.f, 0.f, 0.f));
	m = glm::rotate(m, 1.1f*ftime, glm::vec3(0.0, 1.0, 0.0));
	m = glm::scale(m, glm::vec3(0.53f, 0.53f, 0.53f));
	solarmap["mars"]->SetMVP(m);
	solarmap["mars"]->Render();

	m = glm::mat4(1.0);
	m = glm::rotate(m, 150.f*ftime, glm::vec3(0.0, 1.0, 0.0));
	m = glm::translate(m, glm::vec3(30.f, 0.f, 0.f));
	m = glm::rotate(m, 20.f*ftime, glm::vec3(0.0, 1.0, 0.0));
	m = glm::scale(m, glm::vec3(5.f, 5.f, 5.f));
	solarmap["jupiter"]->SetMVP(m);
	solarmap["jupiter"]->Render();

	m = glm::rotate(m, 72.0f*ftime, glm::vec3(0.0, 1.0, 0.0));
	m = glm::translate(m, glm::vec3(1.5f, 0.f, 0.f));
	m = glm::rotate(m, 200.f*ftime, glm::vec3(0.0, 1.0, 0.0));
	m = glm::scale(m, glm::vec3(0.1f, 0.1f, 0.1f));
	solarmap["jupitermoon"]->SetMVP(m);
	solarmap["jupitermoon"]->Render();
}

void Idle(void)
{
	glClearColor(0.1, 0.1, 0.1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ftime += 0.01;
	glUniform1f(ftimeParameter, ftime);
	glUseProgram(shaderProgram);

	RenderObjects();
	glutSwapBuffers();
}

void Display(void)
{

}

//keyboard callback
void Kbd(unsigned char a, int x, int y)
{
	switch (a)
	{
	case 27: exit(0); break;
	case 32: {needRedisplay = !needRedisplay; break; }
	case 'S':
	case 's': break;
	case 'a': {stacks++; break; }
	case 'A': {
		stacks--;
		if (stacks < 2) stacks = 2;
		break;
	}
	case 'b': {slices++; break; }
	case 'B': {
		slices--;
		if (slices < 2) slices = 2;
		break;
	}
	case '=':
	case '+': {
		GLfloat tmp = xa + dxa;
		if (tmp <= XA_MAX) xa = tmp;
		break;
	}
	case '_':
	case '-': {
		GLfloat tmp = xa - dxa;
		if (tmp >= 0.f) xa = tmp;
		break;
	}
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


void Mouse(int button, int state, int x, int y)
{
	cout << "Location is " << "[" << x << "'" << y << "]" << endl;
}


GLuint InitializeProgram(GLuint *program)
{
	std::vector<GLuint> shaderList;

	//load and compile shaders 	
	shaderList.push_back(CreateShader(GL_VERTEX_SHADER, LoadShader("shaders/transform.vert")));
	shaderList.push_back(CreateShader(GL_FRAGMENT_SHADER, LoadShader("shaders/passthrough.frag")));

	//create the shader program and attach the shaders to it
	*program = CreateProgram(shaderList);

	//delete shaders (they are on the GPU now)
	std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);

	ftimeParameter = glGetUniformLocation(*program, "time");
	modelParameter = glGetUniformLocation(*program, "model");
	viewParameter = glGetUniformLocation(*program, "view");
	projParameter = glGetUniformLocation(*program, "proj");
	colorParameter = glGetUniformLocation(*program, "color");

	return modelParameter;
}

void InitShapes()
{
	ShapesC *tmp;
	tmp = new TeaPotC();
	tmp->SetMVP(glm::mat4(1.0));
	tmp->SetMatrixParamToShader(modelParameter);
	tmp->SetColor(1.f, 1.f, 0.f, 1.f);
	tmp->SetColorParamToShader(colorParameter);
	solarmap["sun"] = tmp;

	tmp = new CubeC();
	tmp->SetMVP(glm::mat4(1.0));
	tmp->SetMatrixParamToShader(modelParameter);
	tmp->SetColor(.75f, 0.75f, 0.75f, 1.f);
	tmp->SetColorParamToShader(colorParameter);
	solarmap["mercury"] = tmp;

	tmp = new CubeC();
	tmp->SetMVP(glm::mat4(1.0));
	tmp->SetMatrixParamToShader(modelParameter);
	tmp->SetColor(.74f, 0.56f, 0.56f, 1.f);
	tmp->SetColorParamToShader(colorParameter);
	solarmap["venus"] = tmp;

	tmp = new SphereC();
	tmp->SetMVP(glm::mat4(1.0));
	tmp->SetMatrixParamToShader(modelParameter);
	tmp->SetColor(0.f, 0.41f, 0.58f, 1.f);
	tmp->SetColorParamToShader(colorParameter);
	solarmap["earth"] = tmp;

	tmp = new CubeC();
	tmp->SetMVP(glm::mat4(1.0));
	tmp->SetMatrixParamToShader(modelParameter);
	tmp->SetColor(.99f, .98f, 0.84f, 1.f);
	tmp->SetColorParamToShader(colorParameter);
	solarmap["moon"] = tmp;

	tmp = new CubeC();
	tmp->SetMVP(glm::mat4(1.0));
	tmp->SetMatrixParamToShader(modelParameter);
	tmp->SetColor(.99f, 0.f, 0.f, 1.f);
	tmp->SetColorParamToShader(colorParameter);
	solarmap["mars"] = tmp;

	tmp = new SphereC();
	tmp->SetMVP(glm::mat4(1.0));
	tmp->SetMatrixParamToShader(modelParameter);
	tmp->SetColor(.9f, 0.8f, 0.2f, 1.f);
	tmp->SetColorParamToShader(colorParameter);
	solarmap["jupiter"] = tmp;

	tmp = new CubeC();
	tmp->SetMVP(glm::mat4(1.0));
	tmp->SetMatrixParamToShader(modelParameter);
	tmp->SetColor(.0f, 0.8f, 0.2f, 1.f);
	tmp->SetColorParamToShader(colorParameter);
	solarmap["jupitermoon"] = tmp;
}

int main(int argc, char **argv)
{
	glutInitDisplayString("stencil>=2 rgb double depth samples");
	glutInit(&argc, argv);
	glutInitWindowSize(wWindow, hWindow);
	glutInitWindowPosition(500, 100);
	glutCreateWindow("Bulge using GLSL");
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	glutDisplayFunc(Display);
	glutIdleFunc(Idle);
	glutMouseFunc(Mouse);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Kbd); //+ and -
	glutSpecialUpFunc(SpecKbdRelease); //smooth motion
	glutSpecialFunc(SpecKbdPress);
	InitializeProgram(&shaderProgram);
	InitShapes();
	glutMainLoop();
	return 0;
}
