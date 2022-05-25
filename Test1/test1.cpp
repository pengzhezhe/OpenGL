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
GLfloat lastX = SCR_WIDTH / 2;
GLfloat lastY = SCR_HEIGHT / 2;
#define STEP 6
#define SAMPLE_CNT (360/STEP)

static const GLfloat PI = 3.14159265358979323846f;
//立方体顶点坐标
static const GLfloat cube_vertices[] = {
	-1.0, 1.0, 1.0,
	-1.0,-1.0, 1.0,
	 1.0,-1.0, 1.0,
	 1.0, 1.0, 1.0,
	 1.0, 1.0,-1.0,
	 1.0,-1.0,-1.0,
	-1.0,-1.0,-1.0,
	-1.0, 1.0,-1.0
};
//立方体顶点颜色
static const GLfloat cube_colors[] = {
	0.0, 1.0, 1.0,
	0.0, 0.0, 1.0,
	1.0, 0.0, 1.0,
	1.0, 1.0, 1.0,
	1.0, 1.0, 0.0,
	1.0, 0.0, 0.0,
	0.0, 0.0, 0.0,
	0.0, 1.0, 0.0,
};
//立方体绘图索引
static const GLushort cube_indices[] = {
	0, 1, 2,   0, 2, 3,    // 前
	4, 5, 6,   4, 6, 7,    // 后
	0, 1, 6,   0, 6, 7,    // 左
	2, 3, 4,   2, 4, 5,    // 右
	0, 3, 4,   0, 4, 7,    // 上
	1, 2, 6,   2, 5, 6     // 下
};

double Z = 5.0f;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

int main()
{
	//Init GLFW
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "TEST1", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//Load GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	Shader shader("./shader.vs", "./shader.fs");

	//Cube
	GLint cube_model_matrix_loc = glGetUniformLocation(shader.ID, "model");
	GLint cube_view_matrix_loc = glGetUniformLocation(shader.ID, "view");
	GLint cube_projection_matrix_loc = glGetUniformLocation(shader.ID, "projection");

	GLuint cube_VAO, cube_points_VBO, cube_colors_VBO, EBO;
	glGenVertexArrays(1, &cube_VAO);
	glBindVertexArray(cube_VAO);

	glGenBuffers(1, &cube_points_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, cube_points_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &cube_colors_VBO); //VBO 1
	glBindBuffer(GL_ARRAY_BUFFER, cube_colors_VBO);// 绑定colors_VBO的性质
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_colors), cube_colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

	//Cylinder
	GLint cylinder_model_matrix_loc = glGetUniformLocation(shader.ID, "model");
	GLint cylinder_view_matrix_loc = glGetUniformLocation(shader.ID, "view");
	GLint cylinder_projection_matrix_loc = glGetUniformLocation(shader.ID, "projection");

	float p = 0.0f, Cr = 1.0f;
	GLfloat cylinder_vertices[(2 * SAMPLE_CNT) * 3];
	GLfloat cylinder_colors[(2 * SAMPLE_CNT) * 3];
	GLuint cylinder_indices[SAMPLE_CNT];
	//生成圆柱顶和底的坐标和颜色
	for (int i = 0; i < 2 * SAMPLE_CNT; i += 2)
	{
		p = (double)i * STEP * PI / 180;
		cylinder_vertices[i * 3] = cos(p) * Cr;          //x
		cylinder_vertices[i * 3 + 1] = 1.0f;             //y
		cylinder_vertices[i * 3 + 2] = sin(p) * Cr;      //z            
		cylinder_vertices[(i + 1) * 3] = cos(p) * Cr;
		cylinder_vertices[(i + 1) * 3 + 1] = -1.0f;
		cylinder_vertices[(i + 1) * 3 + 2] = sin(p) * Cr;
		cylinder_colors[i * 3] = 0.0;
		cylinder_colors[i * 3 + 1] = 1.0;
		cylinder_colors[i * 3 + 2] = 0.0;
		cylinder_colors[(i + 1) * 3] = 0.0;
		cylinder_colors[(i + 1) * 3 + 1] = 1.0;
		cylinder_colors[(i + 1) * 3 + 2] = 0.0;
	}
	//生成索引
	for (int i = 0; i < SAMPLE_CNT; i++)
	{
		cylinder_indices[i] = i;
	}
	GLuint cylinder_VAO, cylinder_points_VBO, cylinder_colors_VBO, cylinder_EBO;
	glGenVertexArrays(1, &cylinder_VAO);
	glBindVertexArray(cylinder_VAO);

	glGenBuffers(1, &cylinder_points_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, cylinder_points_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cylinder_vertices), cylinder_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &cylinder_colors_VBO); //VBO 1
	glBindBuffer(GL_ARRAY_BUFFER, cylinder_colors_VBO);// 绑定colors_VBO的性质
	glBufferData(GL_ARRAY_BUFFER, sizeof(cylinder_colors), cylinder_colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &cylinder_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cylinder_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cylinder_indices), cylinder_indices, GL_STATIC_DRAW);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);



	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, Z);
		glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), SCR_WIDTH / SCR_HEIGHT * 1.0f, 0.1f, 100.0f);
		shader.use();
		//Draw cube
		glBindVertexArray(cube_VAO);
		glm::mat4 cube_model = glm::mat4(1.0f);
		cube_model = glm::translate(cube_model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(cube_model_matrix_loc, 1, GL_FALSE, glm::value_ptr(cube_model));
		glUniformMatrix4fv(cube_view_matrix_loc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(cube_projection_matrix_loc, 1, GL_FALSE, glm::value_ptr(projection));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, NULL);
		//Draw cylinder
		glBindVertexArray(cylinder_VAO);
		glm::mat4 cylinder_model = glm::mat4(1.0f);
		cylinder_model = glm::translate(cylinder_model, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(cylinder_model_matrix_loc, 1, GL_FALSE, glm::value_ptr(cylinder_model));
		glUniformMatrix4fv(cylinder_view_matrix_loc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(cylinder_projection_matrix_loc, 1, GL_FALSE, glm::value_ptr(projection));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 2 * SAMPLE_CNT);
		glDrawElements(GL_TRIANGLE_FAN, SAMPLE_CNT, GL_UNSIGNED_INT, (GLvoid*)0);
		glDrawElementsBaseVertex(GL_TRIANGLE_FAN, SAMPLE_CNT, GL_UNSIGNED_INT, (GLvoid*)0, 1);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &cube_VAO);
	glDeleteVertexArrays(1, &cylinder_VAO);
	glDeleteBuffers(1, &cube_points_VBO);
	glDeleteBuffers(1, &cube_colors_VBO);
	glDeleteBuffers(1, &cylinder_points_VBO);
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Z += yoffset;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}