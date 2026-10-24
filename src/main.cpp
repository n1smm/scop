#include "mymath/mymath.hpp"
#include "scop.h"


void	returnTmpMesh(std::vector<Mesh> *meshList)
{
	unsigned int	indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat			vertices[] = {
	//   x     y       z   		 u		v 	norm x     y     z 
		-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	Mesh	obj1;
	obj1.createMesh(vertices, indices, 32, 12);
	meshList->push_back(std::move(obj1));
}

int main() 
{
	try {
		Window mainWindow(WIDTH, HEIGHT);
		if (mainWindow.initialize() != 0)
			return 1;

		GLint				buffer_width = mainWindow.getBufferWidth();
		GLint				buffer_height = mainWindow.getBufferHeight();
		std::vector<Shader>	shader_list;
		std::vector<Mesh>	mesh_list;

		returnTmpMesh(&mesh_list);
		shader_list.emplace_back(vShaderPath, fShaderPath);

		Camera	camera(mymath::vec3(0.0f, -0.5f, 3.0f),
					mymath::vec3(0.0f, 1.0f, 0.0f),
					-90.0f, 0.0f, 10.0f, 0.5f);


		mymath::mat4	projection = mymath::perspective(45.0f, (GLfloat)buffer_width / (GLfloat)buffer_height, 0.1f, 100.0f);
		GLint	uniform_model = 0, uniform_projection = 0, uniform_view = 0;
		GLfloat	delta_time = 0.0f, last_time = 0.0f;

		uniform_model = shader_list[0].getUniformModelLocation();
		uniform_projection = shader_list[0].getUniformProjectionLocation();
		uniform_view = shader_list[0].getUniformViewLocation();

		while(!mainWindow.getWindowShouldClose())
		{
			GLfloat	now = glfwGetTime();
			delta_time = now - last_time;
			last_time = now;

			glfwPollEvents();
			camera.keyControl(mainWindow.getKeys(), delta_time);
			camera.mouseControl(mainWindow.getMouseEvents(),
					   		mainWindow.getXchange(),
					   		mainWindow.getYchange());
			mainWindow.resetMouseDelta();

			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			shader_list[0].useShader();
				glUniformMatrix4fv(uniform_projection, 1, GL_FALSE, mymath::value_ptr(projection));
				glUniformMatrix4fv(uniform_view, 1, GL_FALSE, mymath::value_ptr(camera.calculateViewMatrix()));

				mymath::mat4	model(1.0f);
				// model = mymath::translate(model, mymath::vec3(0.0f, 0.4f, 0.0f));
				model = mymath::scale(model, mymath::vec3(0.4f, 0.4f, 1.0f));
				// model = mymath::rotate(model, mymath::radians(45), mymath::vec3(0.0f, 1.0f, 0.0f));
				glUniformMatrix4fv(uniform_model, 1, GL_FALSE, mymath::value_ptr(model));
				mesh_list[0].renderMesh();

			glUseProgram(0);
			mainWindow.swapBuffers();
		}
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
    return 0;
}
