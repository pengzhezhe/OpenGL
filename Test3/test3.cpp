#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include "shader.h"
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;
const GLfloat PI = 3.14159265358979323846f;
#define nSlices 50    //经线
#define nStacks 50   //纬线


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
	//Init GLFW
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "TEST3", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//Load GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//Config Shader program
	Shader shader("./shader.vs", "./shader.fs");
	GLint model_matrix_loc = glGetUniformLocation(shader.ID, "model");
	GLint view_matrix_loc = glGetUniformLocation(shader.ID, "view");
	GLint projection_matrix_loc = glGetUniformLocation(shader.ID, "projection");
	//生成球的顶点数据
	int nVerts = (nSlices + 1) * (nStacks + 1);
	int elements = nSlices * nStacks * 6;
	GLfloat theta, phi;
	GLfloat thetaFac = (GLfloat)PI * 2.0f / nSlices;
	GLfloat phiFac = (GLfloat)PI * 1.0f / nStacks;
	GLfloat nx, ny, nz, s, t, Sr = 1.0f;
	GLuint idx = 0, tIdx = 0;
	GLfloat* vertices = (GLfloat*)malloc(nVerts * 3 * sizeof(GLfloat));
	GLushort* indices = (GLushort*)malloc(elements * sizeof(GLushort));

	for (GLuint i = 0; i <= nSlices; i++) {
		theta = i * thetaFac;
		s = (GLfloat)i / nSlices;
		for (GLuint j = 0; j <= nStacks; j++) {
			phi = (GLfloat)j * phiFac;
			t = (GLfloat)j / nStacks;
			nx = sinf(phi) * cosf(theta);
			ny = sinf(phi) * sinf(theta);
			nz = cosf(phi);
			vertices[idx * 3] = Sr * nx;
			vertices[idx * 3 + 1] = Sr * ny;
			vertices[idx * 3 + 2] = Sr * nz;
			idx++;
			tIdx++;
		}
	}
	//生成索引
	GLushort* id = indices;
	for (GLuint i = 0; i < nStacks; i++) {
		for (GLuint j = 0; j < nSlices; j++) {
			int i0 = i * (nSlices + 1) + j;
			int i1 = i0 + 1;
			int i2 = i0 + (nSlices + 1);
			int i3 = i2 + 1;
			if ((j + i) % 2 == 1) {
				*id++ = i0; *id++ = i2; *id++ = i1;
				*id++ = i1; *id++ = i2; *id++ = i3;
			}
			else {
				*id++ = i0; *id++ = i2; *id++ = i3;
				*id++ = i0; *id++ = i3; *id++ = i1;
			}
		}
	}

	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, nVerts * sizeof(GLfloat) * 3, vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements * sizeof(GLushort), indices, GL_STATIC_DRAW);
	glBindVertexArray(0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader.use();
		glBindVertexArray(VAO);
		glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
		glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), SCR_WIDTH / SCR_HEIGHT * 1.0f, 0.1f, 100.0f);
		glm::mat4 cube_model = glm::mat4(1.0f);
		glUniformMatrix4fv(model_matrix_loc, 1, GL_FALSE, glm::value_ptr(cube_model));
		glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, glm::value_ptr(projection));
		glDrawElements(GL_TRIANGLES, elements, GL_UNSIGNED_SHORT, 0);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}