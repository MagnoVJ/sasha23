#pragma once

#include "../engine/camera.hpp"
#include "../engine/shader.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>

#include <iostream>

namespace sasha23 {

	class App
	{
	public:
		App();
		void init();
		void run();
	};
}