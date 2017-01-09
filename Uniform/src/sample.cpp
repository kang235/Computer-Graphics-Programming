/**********************************/
/* GLSL uniform parameter - bulge */
/* (C) Bedrich Benes 2016         */
/* bbenes ~ at ~ purdue.edu       */
/**********************************/

#include <algorithm>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <exception>
#include <stdexcept>
#include <string.h>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include <string>
#include <vector>			//Standard template library class
#include <GL/glew.h>
#include <GL/glut.h>
//in house created libraries
#include "math/vect3d.h"    //for vector manipulation
#include "shaders.h"    

#pragma warning(disable : 4996)
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glut32.lib")

using namespace std;

GLuint points=0; //number of points to display the object
GLuint stacks=15, slices=15;
bool needRedisplay=false;
GLfloat  sign=+1;

//shader program ID
GLuint shaderProgram;
GLfloat ftime, fbulge=0.0;
GLint ftimeParameter,fbulgeParameter;
enum Rotation { NONE, ROTATION_X, ROTATION_Y, ROTATION_Z };
Rotation rot = NONE;

GLfloat modelview[16];
GLfloat projection[16];
GLfloat axisangle[4] = { 1.f, 0.f, 0.f, 0.f };
GLfloat adelta = 0.01f;
GLfloat scale = 1.f;
GLfloat sdelta = 0.05f;
GLfloat color[4] = { 0.f, 0.f, 0.f, 1.f };
GLint fmodelviewParameter, fprojectionParameter, faxisangleParameter, fscaleParameter, fcolorParameter;

//the main window size
GLint wWindow=600;
GLint hWindow=600;

inline void AddVertex(vector <GLfloat> *a, const GLfloat *A)
{
	a->push_back(A[0]);a->push_back(A[1]);a->push_back(A[2]);
}

void CreateCube(vector <GLfloat> *a, float r)
{
	Vect3d p[8];

	p[0].Set(-r, r, r);
	p[1].Set(r, r, r);
	p[2].Set(r, -r, r);
	p[3].Set(-r, -r, r);
	p[4].Set(-r, r, -r);
	p[5].Set(r, r, -r);
	p[6].Set(r, -r, -r);
	p[7].Set(-r, -r, -r);

	AddVertex(a, p[0]); AddVertex(a, p[1]); AddVertex(a, p[2]);
	AddVertex(a, p[0]); AddVertex(a, p[2]); AddVertex(a, p[3]);
	AddVertex(a, p[1]); AddVertex(a, p[5]); AddVertex(a, p[6]);
	AddVertex(a, p[1]); AddVertex(a, p[6]); AddVertex(a, p[2]);
	AddVertex(a, p[5]); AddVertex(a, p[4]); AddVertex(a, p[7]);
	AddVertex(a, p[5]); AddVertex(a, p[7]); AddVertex(a, p[6]);
	AddVertex(a, p[0]); AddVertex(a, p[3]); AddVertex(a, p[7]);
	AddVertex(a, p[0]); AddVertex(a, p[7]); AddVertex(a, p[4]);
	AddVertex(a, p[0]); AddVertex(a, p[4]); AddVertex(a, p[1]);
	AddVertex(a, p[4]); AddVertex(a, p[5]); AddVertex(a, p[1]);
	AddVertex(a, p[2]); AddVertex(a, p[6]); AddVertex(a, p[3]);
	AddVertex(a, p[3]); AddVertex(a, p[6]); AddVertex(a, p[7]);
}

void CreateSphere(vector <GLfloat> *a, GLfloat r,int stacks, int slices)
{
	Vect3d v;

	GLfloat deltaTheta=2*M_PI/(GLfloat)slices;
	GLfloat deltaPhi  =  M_PI/(GLfloat)stacks;

	for (GLint i=0;i<stacks;i++)
	{
		GLfloat phi=i*deltaPhi;
		for (GLint j=0;j<slices;j++)
		{
			GLfloat theta=j*deltaTheta;
//the first triangle
			v.Set(r*cos(theta)*sin(phi),
				  r*sin(theta)*sin(phi),
				             r*cos(phi));
			AddVertex(a,v);
			v.Set(r*cos(theta+deltaTheta)*sin(phi),
				  r*sin(theta+deltaTheta)*sin(phi),
				                        r*cos(phi));
			AddVertex(a,v);
			v.Set(r*cos(theta)*sin(phi+deltaPhi),
				  r*sin(theta)*sin(phi+deltaPhi),
				             r*cos(phi+deltaPhi));
			AddVertex(a,v);
//the second triangle
			v.Set(r*cos(theta+deltaTheta)*sin(phi),
				  r*sin(theta+deltaTheta)*sin(phi),
				                        r*cos(phi));
			AddVertex(a,v);
			v.Set(r*cos(theta)*sin(phi+deltaPhi),
				  r*sin(theta)*sin(phi+deltaPhi),
				             r*cos(phi+deltaPhi));
			AddVertex(a,v);
			v.Set(r*cos(theta+deltaTheta)*sin(phi+deltaPhi),
				  r*sin(theta+deltaTheta)*sin(phi+deltaPhi),
				                        r*cos(phi+deltaPhi));
			AddVertex(a,v);
		}
	}
}

void Inrotrray(int stacks, int slices)
{
	GLuint vaID;

	vector<GLfloat> v;
	//CreateSphere(&v,0.5f,stacks,slices); //radius stacks slices
	CreateCube(&v, 0.5);
	glGenVertexArrays(1,&vaID);
	glBindVertexArray(vaID);

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	points=v.size();
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
	gluOrtho2D(0.f,100.f,0.f,100.f);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3ub(0,0,255);
	glRasterPos2i(10,10);
	for(i=0;i<(int)strlen(message); i++ ) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,message[i]);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

//called when a window is reshaped
void Reshape(int w, int h)
{
  glViewport(0,0,w, h);       
  glEnable(GL_DEPTH_TEST);
//remember the settings for the camera
  wWindow=w;
  hWindow=h;
}


//the main rendering function
void RenderObjects()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //GL_LINE
	glColor3f(0,0,0);
	glPointSize(2);
	glLineWidth(2);
	glDrawArrays(GL_TRIANGLES, 0, 3*points);
}
	
void Idle(void)
{
  ftime+=0.5;

  switch (rot)
  {
  case ROTATION_X:
	  axisangle[3] += adelta;
	  axisangle[0] = 1.f; axisangle[1] = 0.f; axisangle[2] = 0.f;
	  break;
  case ROTATION_Y:
	  axisangle[3] += adelta; 
	  axisangle[0] = 0.f; axisangle[1] = 1.f; axisangle[2] = 0.f;
	  break;
  case ROTATION_Z:
	  axisangle[3] += adelta;
	  axisangle[0] = 0.f; axisangle[1] = 0.f; axisangle[2] = 1.f;
	  break;
  case NONE:
  default:
	  break;
  }

  glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
  glGetFloatv(GL_MODELVIEW_MATRIX, projection);
  glUniform1f(ftimeParameter,ftime);
  glUniform1f(fbulgeParameter,fbulge);
  glUniform4fv(faxisangleParameter, 1, axisangle);
  glUniform1f(fscaleParameter, scale);
  glUniform4fv(fcolorParameter, 1, color);
  glUniformMatrix4fv(fmodelviewParameter, 1, false, modelview);
  glUniformMatrix4fv(fprojectionParameter, 1, false, projection);
  glUseProgram(shaderProgram);
}

void Display(void)
{
	glClearColor(1.0f, 1.f, 1.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	RenderObjects();
	glutSwapBuffers();
	glutPostRedisplay();
}

//keyboard callback
void Kbd(unsigned char a, int x, int y)
{
	switch(a)
	{
 	  case 27 : exit(0);break;
	  case 32 : {needRedisplay=!needRedisplay;break;}

	  case '+': {fbulge+=0.01f; cout << "[fbulge]=[" << fbulge << "]" << endl; break;}
	  case '-': {fbulge-=0.01f;	cout << "[fbulge]=[" << fbulge << "]" << endl; break;}

	  case 'x':
	  case 'X': {
		  rot = rot == ROTATION_X ? NONE : ROTATION_X;
		  axisangle[3] = 0;
		  cout << "X pressed" << endl; 
		  break; }

	  case 'y':
	  case 'Y': {
		  rot = rot == ROTATION_Y ? NONE : ROTATION_Y;
		  axisangle[3] = 0;
		  cout << "Y pressed" << endl;
		  break; }

	  case 'z':
	  case 'Z': {
		  rot = rot == ROTATION_Z ? NONE : ROTATION_Z;
		  axisangle[3] = 0;
		  cout << "Z pressed" << endl;
		  break; }
	  case 's': {
		  scale -= sdelta;
		  cout << "s pressed" << endl;
		  break; }
	  case 'S': {
		  scale += sdelta;
		  cout << "S pressed" << endl;
		  break; }
	  case 'r':
	  case 'R': {
		  color[0] = 1.f; color[1] = 0.f; color[2] = 0.f;
		  cout << "R pressed" << endl;
		  break; }
	  case 'g':
	  case 'G': {
		  color[0] = 0.f; color[1] = 1.f; color[2] = 0.f;
		  cout << "G pressed" << endl;
		  break; }
	  case 'b':
	  case 'B': {
		  color[0] = 0.f; color[1] = 0.f; color[2] = 1.f;
		  cout << "B pressed" << endl;
		  break; }
	}
	glutPostRedisplay();
}


//special keyboard callback
void SpecKbdPress(int a, int x, int y)
{
   	switch(a)
	{
 	  case GLUT_KEY_LEFT  : 
		  {
			  break;
		  }
	  case GLUT_KEY_RIGHT : 
		  {
			break;
		  }
 	  case GLUT_KEY_DOWN    : 
		  {
			break;
		  }
	  case GLUT_KEY_UP  :
		  {
			break;
		  }

	}
	glutPostRedisplay();
}

//called when a special key is released
void SpecKbdRelease(int a, int x, int y)
{
	switch(a)
	{
 	  case GLUT_KEY_LEFT  : 
		  {
			  break;
		  }
	  case GLUT_KEY_RIGHT : 
		  {
			  break;
		  }
 	  case GLUT_KEY_DOWN  : 
		  {
			break;
		  }
	  case GLUT_KEY_UP  :
		  {
			break;
		  }
	}
	glutPostRedisplay();
}


void Mouse(int button,int state,int x,int y)
{
	cout << "Location is " << "[" << x << "'" << y << "]" << endl;
}


void InitializeProgram(GLuint *program)
{
	std::vector<GLuint> shaderList;

//load and compile shaders 	
	shaderList.push_back(CreateShader(GL_VERTEX_SHADER,   LoadShader("shaders/bulge.vert")));
	shaderList.push_back(CreateShader(GL_FRAGMENT_SHADER, LoadShader("shaders/bulge.frag")));

//create the shader program and attach the shaders to it
	*program = CreateProgram(shaderList);

//delete shaders (they are on the GPU now)
	std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);

    ftimeParameter = glGetUniformLocation(*program,"time");
	fbulgeParameter = glGetUniformLocation(*program, "bulge");
	fmodelviewParameter = glGetUniformLocation(*program, "modelview");
	fprojectionParameter = glGetUniformLocation(*program, "projection");
	faxisangleParameter = glGetUniformLocation(*program, "axisangle");
	fscaleParameter = glGetUniformLocation(*program, "scale");
	fcolorParameter = glGetUniformLocation(*program, "rgb");
}



int main(int argc, char **argv)
{ 
  glutInitDisplayString("stencil>=2 rgb double depth samples");
  glutInit(&argc, argv);
  glutInitWindowSize(wWindow,hWindow);
  glutInitWindowPosition(200,100);
  glutCreateWindow("Lab 4 GLSL uniform parameters");
  GLenum err = glewInit();
  if (GLEW_OK != err){
   fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
  }
  glutDisplayFunc(Display);
  glutIdleFunc(Idle);
  glutMouseFunc(Mouse);
  glutReshapeFunc(Reshape);
  glutKeyboardFunc(Kbd); //+ and -
  glutSpecialUpFunc(SpecKbdRelease); //smooth motion
  glutSpecialFunc(SpecKbdPress);
  Inrotrray(stacks,slices);
  InitializeProgram(&shaderProgram);
  glutMainLoop();
  return 0;        
}
	