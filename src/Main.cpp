// std shit
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <map>

#include <cmath>

// Rendering shit
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4, glm::ivec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

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

#define TITLE "Hans die Wurst"

struct GameData {

	GLFWwindow *window;
	bool is_running;
	ShaderProgram program;

	struct {
		GLint vertex_position;
		GLint vertex_uv;
		GLint vertex_normal;
		GLint vertex_color;
	} attributes;

	int width, height;

	TextureLoader texture_loader;
	blurryroots::model::MeshLoader mesh_loader;

	struct {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;
	} matrices;

	Transform model;

	CameraProcessor camera_processor;

	bool fullscreen;

} game_data;

void
test_obj_loader (void);

//
void
initialize (void);

void
on_update (double dt);

void
on_render ();

void
dispose ();

// Callbacks
void
on_key (GLFWwindow *window, int key, int scancode, int action, int mods);

void
on_framebuffer (GLFWwindow *window, int width, int height);

void
on_cursor_position (GLFWwindow *window, double xpos, double ypos);

void
on_cursor_enter (GLFWwindow *window, int entered);

void
on_mouse_button (GLFWwindow *window, int button, int action, int mods);

void
on_scroll (GLFWwindow *window, double xoffset, double yoffset);

int
main (void) {
	try {
		// stuff to setup
		initialize ();

		// Loop until the user closes the window
		double lastTime = glfwGetTime ();
		while (game_data.is_running) {
			// Calculate time spend processing the last frame
			double deltaTime = glfwGetTime () - lastTime;
			lastTime = glfwGetTime ();

			// Do logical updates
			on_update (deltaTime);

			// Draw stuff onto screen
			on_render ();

			// Swap front and back buffers
			glfwSwapBuffers (game_data.window);

			// Poll for and process events
			glfwPollEvents ();
		}
	}
	catch (std::exception &ex) {
		std::cout << "Cought: " << ex.what () << std::endl;
	}
	catch (...) {
		std::cout << "Cought unkown exception :(" << std::endl;
	}

	dispose ();

	return 0;
}

void
open_window (GameData &ctx, const std::string &title, bool fullscreen) {
	// Initialize GLFW
	THROW_IF (GL_TRUE != glfwInit (),
		"Could not initialize glfw!"
	);

	GLFWmonitor *monitor = glfwGetPrimaryMonitor ();
	THROW_IF (nullptr == monitor,
		"Could not primary monitor!"
	);

	const GLFWvidmode *mode = glfwGetVideoMode (monitor);
	THROW_IF (nullptr == mode,
		"Could get video mode for primary monitor!"
	);
	glfwWindowHint (GLFW_RED_BITS, mode->redBits);
	glfwWindowHint (GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint (GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint (GLFW_REFRESH_RATE, mode->refreshRate);
	glfwWindowHint (GLFW_VISIBLE, GL_FALSE);

	// Create a window and its OpenGL context
	ctx.window = glfwCreateWindow (
		mode->width, mode->height,
		title.c_str (),
		fullscreen ? monitor : nullptr,
		nullptr // OpendGL context sharing
	);
	THROW_IF (nullptr == ctx.window,
		"Could not create window!"
	);
	ctx.fullscreen = fullscreen;

	// THIS HAS TO BE CALLED BEFORE GLEW GETS LOADED!
	// Make the window's context current
	glfwMakeContextCurrent (ctx.window);

	// Extension wrangler initialising
	glewExperimental = GL_TRUE;
	GLuint glew_status = glewInit ();
	THROW_IF (GLEW_OK != glew_status,
		"Could not initialize glew!"
	);

	glfwShowWindow (ctx.window); {
		int width, height;
		glfwGetFramebufferSize (ctx.window, &width, &height);
		glViewport (0, 0, width, height);
	}

	// callbacks
	glfwSetKeyCallback (ctx.window, on_key);
	glfwSetFramebufferSizeCallback (ctx.window, on_framebuffer);
	glfwSetCursorPosCallback (ctx.window, on_cursor_position);
	glfwSetCursorEnterCallback (ctx.window, on_cursor_enter);
	glfwSetMouseButtonCallback (ctx.window, on_mouse_button);
	glfwSetScrollCallback (ctx.window, on_scroll);

	glEnable (GL_ALPHA_TEST);
	glAlphaFunc (GL_GREATER, 0.0f);

	// Enable alpha
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LESS);

	glEnable (GL_CULL_FACE);
	glCullFace (GL_BACK);
	glFrontFace (GL_CCW);
}

void
initialize (void) {

	std::cout
		<< "Build with GLFW version "
		<< GLFW_VERSION_MAJOR << "."
		<< GLFW_VERSION_MINOR << "."
		<< GLFW_VERSION_REVISION << std::endl;
	{
		int major, minor, revision;
		glfwGetVersion (&major, &minor, &revision);
		THROW_IF (GLFW_VERSION_MAJOR != major,
			"GLFW major version differs! Runtime: ",
			std::to_string (GLFW_VERSION_MAJOR), " "
			"Build: ", std::to_string (major)
		);
		THROW_IF (GLFW_VERSION_MINOR != minor,
			"GLFW minor version differs! Runtime: ",
			std::to_string (GLFW_VERSION_MINOR), " "
			"Build: ", std::to_string (minor)
		);
		THROW_IF (GLFW_VERSION_REVISION != revision,
			"GLFW revision version differs! Runtime: ",
			std::to_string (GLFW_VERSION_REVISION), " "
			"Build: ", std::to_string (revision)
		);
	}

	//
	open_window (game_data, TITLE, false);

	// setup game stuff
	game_data.is_running = true;

	game_data.program = ShaderProgramBuilder ()
		.add_shader (VertexShader (FileReader ("shaders/es/basic.vert").to_string ()))
		.add_shader (FragmentShader (FileReader ("shaders/es/basic.frag").to_string ()))
		.link ()
		;

	game_data.texture_loader.load ("textures/wool.png", "ship", 0);
	game_data.mesh_loader.load (
		"models/objs/suzanne.smooth.obj",
		"suzanne",
		game_data.program
	);
	//game_data.model = load_model ("models/objs/suzanne.smooth.obj", game_data.program);
	//game_data.model = load_model ("models/objs/suzanne.smooth.obj", game_data.program);

	game_data.camera_processor.on_initialize ();
	game_data.camera_processor.transform.translate (glm::vec3 (0, 0, 3));
}

void
on_update (double dt) {
	game_data.is_running = game_data.is_running
		&& ! glfwWindowShouldClose (game_data.window)
		;

	game_data.camera_processor.on_update (dt);
}

void
render_model (
	ShaderProgram &program,
	TextureLoader &texture_loader,
	const std::string &texture_key,
	blurryroots::model::MeshLoader &mesh_loader,
	const std::string &mesh_key,
	Transform &transform
) {
	program.use ();
	texture_loader.bind (texture_key);

	auto model = game_data.mesh_loader.get (mesh_key);

	program.set_uniform_mat4 ("m", transform.to_matrix ());

	GLint vertex_position =
		program.get_attribute_location ("vertex_position");
	glEnableVertexAttribArray (vertex_position);

	GLint vertex_uv =
		program.get_attribute_location ("vertex_uv");
	glEnableVertexAttribArray (vertex_uv);

	GLint vertex_normal =
		program.get_attribute_location ("vertex_normal");
	glEnableVertexAttribArray (vertex_normal);

	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, model->index_buffer); {
		int size; glGetBufferParameteriv (
			GL_ELEMENT_ARRAY_BUFFER,
			GL_BUFFER_SIZE,
			&size
		);

		glDrawElements (GL_TRIANGLES,
			size / sizeof (unsigned int),
			GL_UNSIGNED_INT,
			0
		);
	}
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableVertexAttribArray (vertex_normal);
	glDisableVertexAttribArray (vertex_uv);
	glDisableVertexAttribArray (vertex_position);

	texture_loader.unbind ();
	program.deactivate ();
}

void
on_render () {
	glClearColor (0.01f, 0.01f, 0.01f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	game_data.program.use ();
	game_data.camera_processor.on_render (game_data.program);
	game_data.program.deactivate ();

	render_model (
		game_data.program,
		game_data.texture_loader, "ship",
		game_data.mesh_loader, "suzanne",
		game_data.model
	);
}

void
dispose () {
	game_data.program.dispose ();
	game_data.texture_loader.dispose ();
	game_data.mesh_loader.dispose ();

	glfwTerminate ();
}

// Callbacks
void
on_key (GLFWwindow *window, int key, int scancode, int action, int mods) {
	if (GLFW_KEY_ESCAPE == key && GLFW_RELEASE == action) {
		game_data.is_running = false;
	}

	game_data.camera_processor.on_key (key, scancode, action, mods);
}

void
on_framebuffer (GLFWwindow *window, int width, int height) {
	std::cout << "framebuffer changed from"
		<< "\nw: " << game_data.width << " to " << width
		<< "\nh: " << game_data.height << " to " << height
		<< "\non window " << window
		<< std::endl;
		;
	game_data.width = width;
	game_data.height = height;

	glViewport (0, 0, game_data.width, game_data.height);
	game_data.camera_processor.on_viewport_changed (width, height);
}

void
on_cursor_position (GLFWwindow *window, double xpos, double ypos) {
	//
	game_data.camera_processor.on_cursor_position (xpos, ypos);
}

void
on_cursor_enter (GLFWwindow *window, int entered) {
	if (entered) {
		// The cursor entered the client area of the window
		game_data.camera_processor.activate ();
	}
	else {
		// The cursor left the client area of the window
		game_data.camera_processor.deactivate ();
	}
}

void
on_mouse_button (GLFWwindow *window, int button, int action, int mods) {
	//
}

void
on_scroll (GLFWwindow *window, double xoffset, double yoffset) {
	//
	std::cout << "scrolling ?" << xoffset << ":" << yoffset << std::endl;
}
