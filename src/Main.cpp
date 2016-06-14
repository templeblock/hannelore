/*
OpenGL ES 2.0
Built-In Variable	Precision	Data Type

Vertex Shader Built-In Variables
gl_Position			highp		vec4
gl_FrontFacing		-			bool
gl_PointSize		mediump		float

Fragment Shader Built-In Variables
gl_FragColor		mediump		vec4
gl_FrontFacing		-			bool
gl_FragCoord		mediump		vec4
gl_PointCoord		mediump		vec2
*/

#define DEBUG


// std shit
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <map>

#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
// glm::vec3
#include <glm/vec3.hpp>
// glm::vec4, glm::ivec4
#include <glm/vec4.hpp>
// glm::mat4
#include <glm/mat4x4.hpp>
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/matrix_transform.hpp>
// glm::value_ptr
#include <glm/gtc/type_ptr.hpp>

//
#include <FileReader.h>
#include <FragmentShader.h>
#include <ShaderProgram.h>
#include <VertexShader.h>
#include <TextureLoader.h>
#include <CameraProcessor.h>
#include <Transform.h>
#include <Util.h>

//
#include <Mesh.h>
#include <MeshLoader.h>
#include <PathUtil.h>
#include <KeyCode.h>
#include <Game.h>
#include <Window.h>

#define TITLE "hannelore dev application"

using namespace blurryroots::hannelore;

Game game;

int
main (void) {
	// set debug level to only show errors and warnings
	blurryroots::util::log_level = blurryroots::util::DebugLogLevel::Error
		| blurryroots::util::DebugLogLevel::Warn
		;

	try {
		WindowManager::initialize ();
		WindowManager::register_handler (&game);
		Window w = WindowManager::open_window (TITLE, false);

		// initialize the game
		game.on_initialize ();
		// set initialie framebuffer size
		game.on_framebuffer (w.m_framebuffer_width, w.m_framebuffer_height);

		// loop until the user closes the window
		double last_frame_time = glfwGetTime ();
		while (game.m_is_running) {
			// calculate time spend processing the last frame
			double current_frame_time = glfwGetTime ();
			double delta_time = current_frame_time - last_frame_time;
			last_frame_time = current_frame_time;

			// do logical updates
			game.on_update (delta_time);

			// draw stuff onto screen
			game.on_render ();

			// update all window specific information
			WindowManager::update ();
		}

		game.on_quit ();
	}
	catch (std::exception &ex) {
		DEBUG_ERROR ("Cought: %s\n", ex.what ());
	}
	catch (...) {
		DEBUG_ERROR ("Cought unkown exception :(\n")
	}

	game.on_dispose ();

	WindowManager::shut_down ();

	return 0;
}
