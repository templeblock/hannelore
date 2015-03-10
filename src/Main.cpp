// std shit
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <map>

// Rendering shit
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.h>
#include <glm/matrix.h>
#include <glm/projection.h>
#include <glm/quaternion.h>
#include <glm/transform.h>
#include <glm/utils.h>
#include <glm/vector.h>
#include <cmath>

// My Stuff
#include <TestGame.h>

//
IGame *game;

void
on_key (GLFWwindow *window, int key, int scancode, int action, int mods) {
	game->on_key (key, scancode, action, mods);

	if (key == GLFW_KEY_F2
		&& action == GLFW_RELEASE) {
		int v = glfwGetInputMode (window, GLFW_CURSOR);
		glfwSetInputMode (
			window,
			GLFW_CURSOR,
			v == GLFW_CURSOR_HIDDEN ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_HIDDEN
		);
	}
}

void
on_framebuffer (GLFWwindow *window, int width, int height) {
	game->on_framebuffer (width, height);
}

void
on_cursor_position (GLFWwindow *window, double xpos, double ypos) {
	game->on_cursor_position (xpos, ypos);
}

void
on_cursor_enter (GLFWwindow *window, int entered) {
	if (entered) {
		// The cursor entered the client area of the window
		game->on_cursor_enter ();
	}
	else {
		// The cursor left the client area of the window
		game->on_cursor_leave ();
	}
}

void
on_mouse_button (GLFWwindow *window, int button, int action, int mods) {
	game->on_mouse_button (button, action, mods);
}

void
on_scroll (GLFWwindow *window, double xoffset, double yoffset) {
	game->on_scroll (xoffset, yoffset);
}

int
main (void) {
	GLFWwindow *window;

	// Initialize GLFW
	if (! glfwInit ()) {
		return 1;
	}

	std::cout
		<< "Build with GLFW version "
		<< GLFW_VERSION_MAJOR << "."
		<< GLFW_VERSION_MINOR << "."
		<< GLFW_VERSION_REVISION << std::endl;

	// Create a window and its OpenGL context
	window = glfwCreateWindow (640, 480, "Hello World", NULL, NULL);
	if (nullptr == window) {
		glfwTerminate ();

		return 1;
	}

	// THIS HAS TO BE CALLED BEFORE GLEW GETS LOADED!
	// Make the window's context current
	glfwMakeContextCurrent (window);

	// Extension wrangler initialising
	glewExperimental = GL_TRUE;
	GLuint glew_status = glewInit ();
	if (GLEW_OK != glew_status) {
		std::cout << "glew?" << std::endl;

		return 1;
	}

	// callbacks
	glfwSetKeyCallback (window, on_key);
	glfwSetFramebufferSizeCallback (window, on_framebuffer);
	glfwSetCursorPosCallback (window, on_cursor_position);
	glfwSetCursorEnterCallback (window, on_cursor_enter);
	glfwSetMouseButtonCallback (window, on_mouse_button);
	glfwSetScrollCallback (window, on_scroll);

	// init the game
	game = nullptr;
	try {
		game = new TestGame ();

		// Do setup stuff
		game->on_initialize ();

		// Loop until the user closes the window
		double lastTime = glfwGetTime ();
		while (game->running () && ! glfwWindowShouldClose (window)) {
			// Calculate time spend processing the last frame
			double deltaTime = glfwGetTime () - lastTime;
			lastTime = glfwGetTime ();

			// Do logical updates
			game->on_update (deltaTime);

			// Draw stuff onto screen
			game->on_render ();

			// Swap front and back buffers
			glfwSwapBuffers (window);

			// Poll for and process events
			glfwPollEvents ();
		}

		game->on_quit ();
	}
	catch (std::string &ex) {
		std::cout << "Cought: " << ex << std::endl;
	}
	catch (std::exception &ex) {
		std::cout << "Cought: " << ex.what () << std::endl;
	}
	catch (...) {
		std::cout << "Cought unkown exception :(" << std::endl;
	}

	if (nullptr != game) {
		delete game;
	}

	glfwTerminate ();

	return 0;
}
