#include "teapot.h"

void TeaPotC::InitArrays()
{
	glGenVertexArrays(1, &vaID);
	glBindVertexArray(vaID);

	glGenBuffers(1, &vbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
	points = data.size();
	glBufferData(GL_ARRAY_BUFFER, points * sizeof(GLfloat), &data[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &ebuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebuffer);
	nindex = indices.size();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, nindex * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	data.clear(); //no need for the data, it is on the GPU now
	indices.clear();
}

TeaPotC::TeaPotC()
{
	Generate();
	InitArrays();
}

void TeaPotC::Generate()
{
	// Vertices
	for (int p = 0; p < TEAPOT_NB_PATCHES; p++) {
		glm::vec3 control_points_k[ORDER + 1][ORDER + 1];
		BuildCtrlPoints(p, control_points_k);
		for (int ru = 0; ru <= RESU - 1; ru++) {
			float u = 1.0 * ru / (RESU - 1);
			for (int rv = 0; rv <= RESV - 1; rv++) {
				float v = 1.0 * rv / (RESV - 1);
				//tpot_vertices.push_back(ComputePos(control_points_k, u, v));
				glm::vec3 tmp = ComputePos(control_points_k, u, v); 
				data.push_back(tmp.x);
				data.push_back(tmp.y);
				data.push_back(tmp.z);
			}
		}
	}

	// Elements
	for (int p = 0; p < TEAPOT_NB_PATCHES; p++) {
		for (int ru = 0; ru < RESU - 1; ru++) {
			for (int rv = 0; rv < RESV - 1; rv++) {
				// 1 square ABCD = 2 triangles ABC + CDA
				// ABC
				indices.push_back(p*RESU*RESV + ru   *RESV + rv);
				indices.push_back(p*RESU*RESV + ru   *RESV + (rv + 1));
				indices.push_back(p*RESU*RESV + (ru + 1)*RESV + (rv + 1));

				// CDA
				indices.push_back(p*RESU*RESV + (ru + 1)*RESV + (rv + 1));
				indices.push_back(p*RESU*RESV + (ru + 1)*RESV + rv);
				indices.push_back(p*RESU*RESV + ru   *RESV + rv);
			}
		}
	}
}

void TeaPotC::SetMVP(glm::mat4 tmp)
{
	glm::mat4 m = glm::mat4(1.0);
	//m = glm::scale(m, glm::vec3(0.5, 0.5, 0.5));
	m = glm::translate(m, glm::vec3(0.0, -1.5f, 0.0));
	m = glm::rotate(m, -90.f, glm::vec3(1, 0, 0));
	mvp = m * tmp;
}

void TeaPotC::Render()
{
	glBindVertexArray(vaID);
	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebuffer);
	glUniformMatrix4fv(modelviewParameter, 1, GL_FALSE, glm::value_ptr(mvp));
	glUniform4f(colorParameter, color[0], color[1], color[2], color[3]);
	glDrawElements(GL_TRIANGLES, nindex, GL_UNSIGNED_SHORT, 0);
}

glm::vec3 TeaPotC::ComputePos(glm::vec3 control_points_k[][ORDER + 1], float u, float v) {
	glm::vec3 result = glm::vec3( 0.0, 0.0, 0.0 );
	for (int i = 0; i <= ORDER; i++) {
		for (int j = 0; j <= ORDER; j++) {
			float poly_i = BernsteinPolynomial(i, ORDER, u);
			float poly_j = BernsteinPolynomial(j, ORDER, v);
			result.x += poly_i * poly_j * control_points_k[i][j].x;
			result.y += poly_i * poly_j * control_points_k[i][j].y;
			result.z += poly_i * poly_j * control_points_k[i][j].z;
		}
	}
	return result;
}

void TeaPotC::BuildCtrlPoints(int p, glm::vec3 control_points_k[][ORDER + 1])
{
	for (int i = 0; i <= ORDER; i++)
		for (int j = 0; j <= ORDER; j++)
			control_points_k[i][j] = cpdata[patchdata[p][i][j] - 1];
}

float TeaPotC::BernsteinPolynomial(int i, int n, float u) {
	return BinomialCoefficient(i, n) * powf(u, i) * powf(1 - u, n - i);
}

float TeaPotC::BinomialCoefficient(int i, int n) {
	assert(i >= 0); assert(n >= 0);
	return 1.0f * Factorial(n) / (Factorial(i) * Factorial(n - i));
}

int TeaPotC::Factorial(int n) {
	assert(n >= 0);
	int result = 1;
	for (int i = n; i > 1; i--)
		result *= i;
	return result;
}