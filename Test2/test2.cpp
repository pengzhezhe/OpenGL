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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
	//Init GLFW
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "TEST2", NULL, NULL);
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
	Shader ourShader("./shader.vs", "./shader.fs");
	GLint model_matrix_loc = glGetUniformLocation(ourShader.ID, "model");
	GLint view_matrix_loc = glGetUniformLocation(ourShader.ID, "view");
	GLint projection_matrix_loc = glGetUniformLocation(ourShader.ID, "projection");

	//Cube
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
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ourShader.use();
		//Draw cube
		glBindVertexArray(cube_VAO);
		glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 15.0f);
		glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), SCR_WIDTH / SCR_HEIGHT * 1.0f, 0.1f, 100.0f);
		glm::mat4 cube_model = glm::mat4(1.0f);
		cube_model = glm::scale(cube_model, glm::vec3(2.0, 2.0, 2.0));
		cube_model = glm::rotate(cube_model, glm::radians(45.0f), glm::vec3(0.0, 0.0, 1.0));
		cube_model = glm::translate(cube_model, glm::vec3(1.0f, 0.0f, -1.0f));
		glUniformMatrix4fv(model_matrix_loc, 1, GL_FALSE, glm::value_ptr(cube_model));
		glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, glm::value_ptr(projection));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, NULL);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &cube_VAO);
	glDeleteBuffers(1, &cube_points_VBO);
	glDeleteBuffers(1, &cube_colors_VBO);
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