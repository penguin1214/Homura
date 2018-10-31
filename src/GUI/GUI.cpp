#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include "GUI/opengl_utils.h"
#include "core/io/ObjHandler.h"
#include "core/math/Geometry.h"
#include "core/math/Mat4f.h"
#include "renderer/sensors/Orthographic.h"
#include "renderer/Scene.h"
#include "renderer/primitives/TriangleMesh.h"

using namespace Homura;

int window_width = 800, window_height = 600;
double initPosx, initPosy;
double exitPosx, exitPosy;
double camOffsetx, camOffsety;

void error_callback(int error, const char* description);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
GLuint loadShader(const char* vs_path, const char* fs_path);
void prepareForRender(TriangleMesh* mesh, GLuint &VBO, GLuint &VAO);
void oglRender(TriangleMesh *mesh, GLuint VBO, GLuint VAO);

int main() {
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		return -1;

	GLFWwindow *window = glfwCreateWindow(window_width, window_height, "Homura", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));

	double camX = 5.f, camY = 5.f;
	glViewport(0, 0, window_width, window_height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
		camX, camY, 30,
		0, 0, 0,
		0, 1, 0);
	glMatrixMode(GL_PROJECTION);
	glOrtho(-100, 100, -100, 100, 0.1, 1000);

	std::vector<Point3f> verts;
	std::vector<Vec3i> indecies;
	std::vector<Vec3f> normals;
	ObjHandler::readObj("D:/Scenes/teddy_bear.obj", verts, indecies, normals);
	TriangleMesh obj(verts, indecies);

	GLuint VBO, VAO;
	prepareForRender(&obj, VBO, VAO);

	while (!glfwWindowShouldClose(window)) {
		// Render here
		glClear(GL_COLOR_BUFFER_BIT);

		camX += camOffsetx;
		camY += camOffsety;
		//printf("exit: (%f, %f)\n", exitPosx, exitPosy);
		//printf("init: (%f, %f)\n", initPosx, initPosy);
		//printf("camPos: (%f, %f)\n", camX, camY);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(
			camX, camY, 30,
			0, 0, 0,
			0, 1, 0);

		oglRender(&obj, VBO, VAO);

		// swap buffer
		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void error_callback(int error, const char *description) {
	fprintf(stderr, "Error: %s\n", description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		printf("escape key pressed!");
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		glfwGetCursorPos(window, &initPosx, &initPosy);
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		glfwGetCursorPos(window, &exitPosx, &exitPosy);
		camOffsetx = 0.001f*(exitPosx - initPosx);
		camOffsety = 0.001f*(exitPosy - initPosy);
	}
}

void prepareForRender(TriangleMesh* mesh, GLuint &VBO, GLuint &VAO) {
#if 1
	float *vertex_arr = new float[9 * mesh->_indecies.size()];

	for (int i = 0; i < 3 * mesh->_indecies.size(); i++) {
		vertex_arr[3 * i] = mesh->_vertices[mesh->_indecies[i / 3][i % 3]-1][0];
		vertex_arr[3 * i + 1] = mesh->_vertices[mesh->_indecies[i / 3][i % 3]-1][1];
		vertex_arr[3 * i + 2] = mesh->_vertices[mesh->_indecies[i / 3][i % 3]-1][2];
	}

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 9*mesh->_indecies.size()*sizeof(float), vertex_arr, GL_STATIC_DRAW);

	delete[] vertex_arr;
#else
	float *vertex_arr = new float[9];
	vertex_arr[0] = 0.f, vertex_arr[1] = 1.f, vertex_arr[2] = 0.f;
	vertex_arr[3] = 1.f, vertex_arr[4] = 0.f, vertex_arr[5] = 0.f;
	vertex_arr[6] = -1.f, vertex_arr[7] = 0.f, vertex_arr[8] = 0.f;

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 9*sizeof(float), vertex_arr, GL_STATIC_DRAW);
#endif

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glUseProgram(loadShader("vertex_shader.glsl", "fragment_shader.glsl"));
}

GLuint loadShader(const char* vs_path, const char* fs_path) {
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	std::string vsCode;
	std::ifstream vsStream(vs_path, std::ios::in);
	if (vsStream.is_open()) {
		std::string line;
		while (std::getline(vsStream, line)) {
			vsCode += line + "\n";
		}
		vsStream.close();
	}
	else {
		std::cerr << "ERROR::GUI::LOAD_SHADER::VERTEX::FILE_OPEN_FAILED\n" << std::endl;
		exit(-1);
	}

	std::string fsCode;
	std::ifstream fsStream(fs_path, std::ios::in);
	if (fsStream.is_open()) {
		std::string line;
		while (std::getline(fsStream, line)) {
			fsCode += line + "\n";
		}
		fsStream.close();
	}
	else {
		std::cerr << "ERROR::GUI::LOAD_SHADER::FRAGMENT::FILE_OPEN_FAILED\n" << std::endl;
		exit(-1);
	}

	int success;
	char info[1024];

	const char* vs = vsCode.c_str();
	glShaderSource(vertexShaderID, 1, &vs, NULL);
	glCompileShader(vertexShaderID);
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShaderID, 1024, NULL, info);
		std::cerr << "ERROR::GUI::LOAD_SHADER::VERTEX::COMPILATION_FAILED\n" << info << std::endl;
	}

	const char* fs = fsCode.c_str();
	glShaderSource(fragmentShaderID, 1, &fs, NULL);
	glCompileShader(fragmentShaderID);
	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShaderID, 1024, NULL, info);
		std::cerr << "ERROR::GUI::LOAD_SHADER::FRAGMENT::COMPILATION_FAILED\n" << info << std::endl;
	}

	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);

	glLinkProgram(programID);
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(programID, 1024, NULL, info);
		std::cerr << "ERROR::GUI::LOAD_SHADER::PROGRAM::LINKING_FAILED\n" << info << std::endl;
	}

	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
}

void oglRender(TriangleMesh *mesh, GLuint VBO, GLuint VAO) {


	glBindVertexArray(VAO);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(GL_TRIANGLES, 0, 3*mesh->_indecies.size());
}
