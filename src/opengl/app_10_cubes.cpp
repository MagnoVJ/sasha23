#include "app_10_cubes.hpp"

namespace sasha23 {

	const unsigned int SCR_WIDTH = 1366;
	const unsigned int SCR_HEIGHT = 768;

	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void process_input(GLFWwindow* window);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	GLFWwindow* window;

	Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

	bool firstMouse = true;
	float lastX = SCR_WIDTH / 2.0f;
	float lastY = SCR_HEIGHT / 2.0f;

	// timing
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	App::App() 
	{
		init();
	}

	void App::init() 
	{

		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Sasha23", nullptr, nullptr);

		if (window == nullptr) {
			std::cout << "ERROR::FAILED_TO_CREATE_WINDOW_GLFW\n" << std::endl;
			glfwTerminate();
		}

		glfwMakeContextCurrent(window);
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			std::cout << "ERROR::FAILED_TO_INITIALIZE_GLAD\n" << std::endl;
		}

		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	}


	void App::run()
	{

		glEnable(GL_DEPTH_TEST);

		Shader shader_obj("./src/opengl/shaders/texture_vs.glsl", "./src/opengl/shaders/texture_fs.glsl");

		float vertices[180];
		int i = 0;

		std::ifstream geoFile;
		std::string geoFileString;

		geoFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try {

			geoFile.open("./assets/cube.sgf");
			std::stringstream geoStringStream;
			geoStringStream << geoFile.rdbuf();
			geoFile.close();
			geoFileString = geoStringStream.str();

			for (auto it = geoFileString.cbegin(); it != geoFileString.cend(); ++it) {

				if (*it == '{') {

					while (*it != '}') {
						++it;
						std::stringstream ss;
						while (*it != ',' && *it != ' ' && *it != '\n' && *it != '}') {
							ss << *it;
							++it;
						}
						std::string s = ss.str();
						if (s != "") {
							vertices[i] = std::stof(s);
							i++;
						}

					}
				}
			}

		}
		catch (std::ifstream::failure& e) {
			std::cout << "ERROR::COULDN'T READ GEOMETRY FILE" << std::endl;
		}

		glm::vec3 cubePositions[] = {
			glm::vec3( 0.0f,  0.0f,  0.0f),
			glm::vec3( 2.0f,  5.0f, -15.0f),
			glm::vec3(-1.5f, -2.2f, -2.5f),
			glm::vec3(-3.8f, -2.0f, -12.3f),
			glm::vec3( 2.4f, -0.4f, -3.5f),
			glm::vec3(-1.7f,  3.0f, -7.5f),
			glm::vec3( 1.3f, -2.0f, -2.5f),
			glm::vec3( 1.5f,  2.0f, -2.5f),
			glm::vec3( 1.5f,  0.2f, -1.5f),
			glm::vec3(-1.3f,  1.0f, -1.5f)
		};

		unsigned int VBO, VAO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);

		// -- Textures --
		unsigned int texture1, texture2;
		glGenTextures(1, &texture1);
		glBindTexture(GL_TEXTURE_2D, texture1);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true);
		
		unsigned char* data = stbi_load("./assets/container.jpg", &width, &height, &nrChannels, 0);
		
		if (data) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			std::cout << "Failed to load texture" << std::endl;
		}

		stbi_image_free(data);

		glGenTextures(1, &texture2);
		glBindTexture(GL_TEXTURE_2D, texture2);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		data = stbi_load("./assets/GhoststarFINISHED.png", &width, &height, &nrChannels, 0);

		if (data) {
			// if it has alpha channel make it rgba
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			std::cout << "Failed to load texture" << std::endl;
		}

		stbi_image_free(data);

		shader_obj.use();
		shader_obj.setInt("texture1", 0);
		shader_obj.setInt("texture2", 1);

		while (!glfwWindowShouldClose(window)) {

			float currentFrame = static_cast<float>(glfwGetTime());
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			process_input(window);

			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, texture2);

			shader_obj.use();

			glm::mat4 projection = glm::mat4(1.0f);
			projection = glm::perspective(glm::radians(camera.zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			shader_obj.setMat4("projection", projection);

			glm::mat4 view = camera.getViewMatrix();
			shader_obj.setMat4("view", view);

			//shader_obj.setMat4("model", model);
			
			glBindVertexArray(VAO);

			for (unsigned short i = 0; i < 10; i++) {

				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, cubePositions[i]);
				float angle = 20.0f * i;
				model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
				shader_obj.setMat4("model", model);
				glDrawArrays(GL_TRIANGLES, 0, 36);

			}			

			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);

		glfwTerminate();
	}

	void process_input(GLFWwindow* window)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		float cameraSpeed = static_cast<float>(2.5 * deltaTime);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.processKeyboard(CameraMovement::FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.processKeyboard(CameraMovement::BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.processKeyboard(CameraMovement::LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.processKeyboard(CameraMovement::RIGHT, deltaTime);

	}

	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	}

	void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
	{
		float xpos = static_cast<float>(xposIn);
		float ypos = static_cast<float>(yposIn);

		if (firstMouse) {
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;

		camera.processMouseMovement(xoffset, yoffset);

	}

	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{

		camera.processMouseScroll(static_cast<float>(yoffset));

		//std::cout << fov << std::endl;
	}

}