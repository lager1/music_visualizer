#include <iostream>
using std::cout;
using std::endl;

#include "Window.h"

#include "audio_data.h" // VISUALIZER_BUFSIZE

Window::Window(int _width, int _height, bool& is_ok) : width(_width), height(_height), size_changed(true), mouse() {
	is_ok = false;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_DECORATED, false);

	window = glfwCreateWindow(width, height, "Music Visualizer", NULL, NULL);
	if (window == NULL) {
		cout << "GLFW window creation failed." << endl;
		return;
	}

	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, this);
	auto mouse_button_func = [](GLFWwindow * window, int button, int action, int mods) {
		static_cast<Window*>(glfwGetWindowUserPointer(window))->mouse_button_callback(button, action, mods);
	};
	auto cursor_pos_func = [](GLFWwindow * window, double xpos, double ypos) {
		static_cast<Window*>(glfwGetWindowUserPointer(window))->cursor_position_callback(xpos, ypos);
	};
	auto window_size_func = [](GLFWwindow * window, int width, int height) {
		static_cast<Window*>(glfwGetWindowUserPointer(window))->window_size_callback(width, height);
	};
	auto keyboard_func = [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		static_cast<Window*>(glfwGetWindowUserPointer(window))->keyboard_callback(key, scancode, action, mods);
	};
	glfwSetKeyCallback(window, keyboard_func);
	glfwSetCursorPosCallback(window, cursor_pos_func);
	glfwSetMouseButtonCallback(window, mouse_button_func);
	glfwSetWindowSizeCallback(window, window_size_func);

	glewExperimental = GL_TRUE;
	glewInit();
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported "<< version << endl;

	glfwSwapInterval(0);
	//glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES, &max_output_vertices);
	//glEnable(GL_DEPTH_TEST); // maybe allow as option so that geom shaders are more useful
	glDisable(GL_DEPTH_TEST);

	// Required by gl but unused.
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Generate audio textures
	for (int i = 0; i < 4; ++i) {
		GLuint tex;
		glGenTextures(1, &tex);
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_1D, tex);
		glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, VISUALIZER_BUFSIZE, 0, GL_RED, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	}

	is_ok = true;
}

Window::~Window() {
	// If we're being destroyed, then the app is shutting down.
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Window::window_size_callback(int _width, int _height) {
	width = _width;
	height = _height;
	size_changed = true;
}

void Window::cursor_position_callback(double xpos, double ypos) {
	mouse.x = float(xpos);
	mouse.y = height-float(ypos);
}

void Window::mouse_button_callback(int button, int action, int mods) {
	// TODO
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		mouse.down = true;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) 
		mouse.down = false;
	// if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) 
	// if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) 
}

void Window::keyboard_callback(int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

bool Window::is_alive() {
	return !glfwWindowShouldClose(window); 
}

void Window::poll_events() {
	// size_changed should've been noticed by renderer this frame, so reset
	size_changed = false;
	glfwPollEvents();
}

void Window::swap_buffers() {
	glfwSwapBuffers(window);
}