#include<iostream>
#include<GL/glew.h>
#include<GLFW/glfw3.h>

//WINDOW DIMESIONS
const GLint WIDTH = 800, HEIGHT = 600;

using namespace std;

int main() 
{
	//Initialize GLFW
	if (!glfwInit()) {
		cout << "GLFW Initialization failed!!" << endl;;
		glfwTerminate();
		return 1;
	}

	//Setup GLFW window properties
	//OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//CORE PROFILE = no backwards compatibility
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//Allow FORWARD compatibility
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test Window", NULL, NULL);

	if (!mainWindow) {
		cout << "GLFW window creation failed!!" << endl;
		glfwTerminate();
		return 1;
	}

	//Get buffer size information
	int bufferWidth, bufferHeight;

	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	//Set GLEW context for windows
	glfwMakeContextCurrent(mainWindow);

	//Allow modern extension features
	glewExperimental = GL_TRUE;

	if (!glewInit()) {
		cout << "glew Init failed!!" << endl;
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	//Set up viewport size


	return 0;
}