#ifndef hannelore_CameraProcessor_h
#define hannelore_CameraProcessor_h

#define PI 3.1415f
#define PI_OVER_4 PI / 4.0f
#define HORIZONTAL_MOUSE_LOOK_FACTOR -1.0f * PI_OVER_4 / 150.0f

#include <Transform.h>
#include <KeyCode.h>
#include <CameraData.h>
#include <yanecos/IDataProcessor.h>

#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4, glm::ivec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <glm/gtc/epsilon.hpp> // glm::epsilonEqual

#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <sstream>

class CameraProcessor : public blurryroots::yanecos::IDataProcessor {

public:
	Transform transform;
	CameraData data;

public:
	void
	activate (void) override final {
		this->is_running = true;
	}

	void
	deactivate (void) override final {
		this->is_running = false;
	}

	void
	on_initialize (void) override final {
		this->data.field_of_view = 70.0f;
		this->data.near = 0.1f;
		this->data.far = 100.0f;
		this->data.aspect_ratio = 4.0f / 3.0f;
	}

	void
	on_update (double dt) override final {
		if (false == this->is_running) {
			return;
		}

		float fdt = static_cast<float> (dt);

		{
			glm::mat4 inv_rotation = glm::inverse (this->transform.to_rotation ());
			glm::vec3 right   = Transform::to_right (inv_rotation);
			//glm::vec3 up      = Transform::to_up (inv_rotation);

			if (0 != this->data.yaw) {
				// Use the world up vector to create a obsever like camera look.
				// If you want to create a spaceship like camera use the local
				// up vector.
				float v = static_cast<float> (this->data.yaw) * fdt;
				this->transform.rotate (
					v, Transform::UP
				);
			}

			if (0 != this->data.pitch) {
				float v = static_cast<float> (this->data.pitch) * fdt;
				this->transform.rotate (
					v, right
				);
			}
		}

		{
			float speed = 1.618f;
			glm::mat4 inv_rotation = glm::inverse (this->transform.to_rotation ());
			glm::vec3 right   = Transform::to_right (inv_rotation);
			//glm::vec3 up      = Transform::to_up (inv_rotation);
			glm::vec3 forward = Transform::to_forward (inv_rotation);

			auto zero_movement =
				glm::epsilonEqual (Transform::ZERO, this->data.movement, 0.01f);
			if (false == zero_movement.x) {
				glm::vec3 direction = this->data.movement.x * right;
				this->transform.translate (
					fdt * speed * direction
				);
			}
			if (false == zero_movement.y) {
				glm::vec3 direction = this->data.movement.y * Transform::UP;
				this->transform.translate (
					fdt * speed * direction
				);
			}
			if (false == zero_movement.z) {
				glm::vec3 direction = this->data.movement.z * forward;
				this->transform.translate (
					fdt * speed * direction
				);
			}
		}

		this->data.view = this->transform.to_rotation ()
			* glm::inverse (this->transform.to_translation ())
			;
	}

	void
	on_render (ShaderProgram& program) override final {
		program.set_uniform_mat4 ("v", this->data.view);
		program.set_uniform_mat4 ("p", this->data.projection);
	}

	void
	on_viewport_changed (int width, int height) {
		if (0 >= height) {
			DEBUG_WARN ("height has been %i! (width: %i)\n", height, width);
			return;
		}

		this->data.aspect_ratio = width / height;

		this->data.projection = glm::perspective (
			this->data.field_of_view,
			this->data.aspect_ratio,
			this->data.near,
			this->data.far
		);
	}

	void
	on_key_up (KeyCode key, KeyModifier mods) {
		if (KeyCode::left == key) {
			this->data.yaw += 1;
		}

		if (KeyCode::right == key) {
			this->data.yaw -= 1;
		}

		if (KeyCode::up == key) {
			this->data.pitch += 1;
		}

		if (KeyCode::down == key) {
			this->data.pitch -= 1;
		}

		if (KeyCode::w == key) {
			this->data.movement.z += 1;
		}

		if (KeyCode::s == key) {
			this->data.movement.z -= 1;
		}

		if (KeyCode::a == key) {
			this->data.movement.x += 1;
		}

		if (KeyCode::d == key) {
			this->data.movement.x -= 1;
		}

		if (KeyCode::q == key) {
			this->data.movement.y -= 1;
		}

		if (KeyCode::e == key) {
			this->data.movement.y += 1;
		}

		if (KeyCode::space == key) {
			DEBUG_LOG ("Camera @ %s",
				vec3_to_string (Transform::to_position (
					glm::inverse (this->transform.to_translation ())
					))
				);
		}

		if (KeyCode::escape == key) {
			this->transform.reset_translation ();
			this->transform.reset_rotation ();
		}
	}

	void
	on_key_down (KeyCode key, KeyModifier mods) {
		if (KeyCode::left == key) {
			this->data.yaw -= 1;
		}

		if (KeyCode::right == key) {
			this->data.yaw += 1;
		}

		if (KeyCode::up == key) {
			this->data.pitch -= 1;
		}

		if (KeyCode::down == key) {
			this->data.pitch += 1;
		}

		if (KeyCode::w == key) {
			this->data.movement.z -= 1;
		}

		if (KeyCode::s == key) {
			this->data.movement.z += 1;
		}

		if (KeyCode::a == key) {
			this->data.movement.x -= 1;
		}

		if (KeyCode::d == key) {
			this->data.movement.x += 1;
		}

		if (KeyCode::q == key) {
			this->data.movement.y += 1;
		}

		if (KeyCode::e == key) {
			this->data.movement.y -= 1;
		}
	}

	void
	on_cursor_position (double x, double y) {
	}

	static std::string
	vec3_to_string (const glm::vec3 &v) {
		std::stringstream ss;
		ss
			<< "("
			<< "x:" << v.x << ","
			<< "y:" << v.y << ","
			<< "z:" << v.z << ")"
			;

		return ss.str ();
	}

	CameraProcessor ()
	: is_running (true)
	, transform ()
	, data () {}

private:
	bool is_running;

};

#endif
