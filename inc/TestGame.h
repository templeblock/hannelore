#ifndef __GAME__
#define __GAME__

// std shit
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <map>

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

//
#include <IGame.h>
#include <Shader.h>
#include <ShaderProgram.h>
#include <MeshData.h>
#include <Transform.h>
#include <MeshLoader.h>
#include <MeshRenderer.h>

class TestGame : public IGame {

private:
	bool is_running;
	bool is_initialized;
	int width;
	int height;

	ShaderProgram program;

	float camera_speed;
	glm::vec3 camera_movement;

	float mouse_speed;
	glm::vec2 mouse_position;
	glm::vec2 mouse_position_offset;
	glm::vec2 mouse_center;

	MeshLoader mesh_loader;
	MeshRenderer mesh_renderer;

	MeshData *
	create_square_mesh (void);

	MeshData *
	create_cube_mesh (void);

	MeshData *
	create_triangle_mesh (void);

public:
	TestGame (void);

	virtual
	~TestGame (void);

	void dispose (void);

	void
	update (double dt);

	void
	render (void);

	void
	on_key (int key, int scancode, int action, int mods);

	void
	on_framebuffer (int width, int height);

	void
	on_cursor_position (double xpos, double ypos);

	bool
	running (void);

	void
	on_cursor_enter (void);

	void
	on_cursor_leave (void);

	void
	on_mouse_button (int button, int action, int mods);

	void
	on_scroll (double xoffset, double yoffset);

	void
	on_quit (void);

	void
	quit (void);

};

#endif
