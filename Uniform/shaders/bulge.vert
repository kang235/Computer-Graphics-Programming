#version 330
/*	(C) 2016
	Bedrich Benes 
	Purdue University
	bbenes<#!&^#>purdue<.>edu
*/	

layout (location=0) in vec4 iPosition;
out vec4 theColor;
uniform float time;
uniform float bulge;
uniform mat4 modelview;
uniform mat4 projection;
uniform vec4 axisangle;
uniform float scale;
uniform vec4 rgb;

//neat method: http://www.neilmendoza.com/glsl-rotation-about-an-arbitrary-axis/
mat4 rotationMatrix(vec4 axisangle)
{
    vec3 axis = normalize(axisangle.xyz);
    float s = -sin(axisangle.w);
    float c = cos(axisangle.w);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

mat4 scaleMatrix(float s)
{
	return mat4(s,  0.0,0.0,0.0,
				0.0,s,  0.0,0.0,
				0.0,0.0,s,  0.0,
				0.0,0.0,0.0,1.0);
}

void main()					
{
	float delta=bulge*sin(iPosition.x*time)*sin(iPosition.y*time/2.0)*sin(iPosition.z*time/3.0);
	vec4 dir=normalize(iPosition);
	vec4 tmp=iPosition+delta*dir;
	tmp.w=1; //make sure w is not modified!
	gl_Position=projection*modelview*rotationMatrix(axisangle)*scaleMatrix(scale)*tmp; 
	theColor=rgb;			//set the output color
}
