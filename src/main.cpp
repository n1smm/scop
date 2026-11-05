#include "mymath/mymath.hpp"
#include "scop.h"

std::optional<float> parse_float(std::string_view str)
{
	float value = 0.0f;
	auto [ptr, er_code] = std::from_chars(str.data(), str.data() + str.size(), value);
	if (er_code != std::errc(0) || ptr != str.data() + str.size())
		return std::nullopt;
	return value;
}

int main(int argc, char **argv) 
{
	try {
		if (argc < 2 || argc > 3)
		{
			std::cerr << "usage: ./scop <model.obj> <scalar;optional>" << std::endl;
			return 1;
		}
		std::string model_path = std::string(argv[1]);

		if (model_path.empty())
			return 1;

		GLfloat scalar = 1.0f;
		if (argc == 3)
		{
			auto value = parse_float(argv[2]);
			if (value)
				scalar = value.value();
		}
				


		Window mainWindow(WIDTH, HEIGHT);
		if (mainWindow.initialize() != 0)
			return 1;

		GLint				buffer_width = mainWindow.getBufferWidth();
		GLint				buffer_height = mainWindow.getBufferHeight();
		std::vector<Shader>	shader_list;
		std::vector<Mesh>	mesh_list;
		Texture				tex("textures/brick.png");
		// Model				new_model("models/teapot2.obj");
		Model				new_model(model_path.c_str());

		if (!new_model.isValid())
		{
			std::cerr << "invalid model: " << model_path << std::endl;
			throw std::runtime_error("stopping program");
		}

		returnTmpMesh(&mesh_list);
		shader_list.emplace_back(vShaderPath, fShaderPath);

		Camera	camera(mymath::vec3(0.0f, 10.0f, 5.0f),
					mymath::vec3(0.0f, 1.0f, 0.0f),
					-90.0f, -60.0f, 10.0f, 0.5f);


		mymath::vec3	model_center = new_model.getCenter();
		mymath::mat4	projection = mymath::perspective(45.0f, (GLfloat)buffer_width / (GLfloat)buffer_height, 0.1f, 100.0f);
		GLint	uniform_model = 0, uniform_projection = 0, uniform_view = 0, uniform_time = 0;
		GLint	uniform_surface_switch = 0, uniform_in_color = 0;
		GLfloat	mouse_pitch = 0.0f, mouse_yaw = 0.0f;
		GLfloat	delta_time = 0.0f, last_time = 0.0f, transition_start = 0.0f;
		const GLfloat duration = 1.0f;
		SurfaceState curr_state = new_model.getSurfaceState();
		SurfaceState new_state = curr_state;
		bool transition_active = false;
		bool initial_wipe = true;



		uniform_model = shader_list[0].getUniformModelLocation();
		uniform_projection = shader_list[0].getUniformProjectionLocation();
		uniform_view = shader_list[0].getUniformViewLocation();
		uniform_surface_switch = shader_list[0].getUniformSurfaceSwitchLocation();
		uniform_in_color = shader_list[0].getUniformInColorLocation();
		uniform_time = shader_list[0].getUniformTimeLocation();


		transition_start = last_time = glfwGetTime();
		transition_active = true;
		while(!mainWindow.getWindowShouldClose())
		{
			GLfloat	now = glfwGetTime();
			delta_time = now - last_time;
			last_time = now;

			glfwPollEvents();
			SurfaceState prev_state = new_model.getSurfaceState();
			new_model.setSurfaceState(mainWindow.getKeys());	
			SurfaceState state_now = new_model.getSurfaceState();
			if (state_now != prev_state)
			{
				curr_state = prev_state;
				new_state = state_now;
				transition_start = last_time;
				transition_active = true;
				initial_wipe = false;
			}
			camera.keyControl(mainWindow.getKeys(), delta_time);
			camera.mouseControl(mainWindow.getMouseEvents(),
					   		mainWindow.getXchange(),
					   		mainWindow.getYchange());
			mainWindow.resetMouseDelta();

			glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			shader_list[0].useShader();
				glUniform1f(uniform_time, static_cast<GLfloat>(now - transition_start));
				glUniformMatrix4fv(uniform_projection, 1, GL_FALSE, mymath::value_ptr(projection));
				glUniformMatrix4fv(uniform_view, 1, GL_FALSE, mymath::value_ptr(camera.calculateViewMatrix()));
				glUniform1i(uniform_surface_switch, static_cast<GLint>(new_model.getSurfaceState())); 

				mouse_pitch = camera.getPitchOut();
				mouse_yaw 	= camera.getYawOut();
				
				
				mymath::mat4	model(1.0f);

				// x-wing transformation
				mymath::vec3 original = mymath::vec3(1.0f, 1.0f, 1.0f);
				
				// model = mymath::translate(model, (model_center * scalar));
				model = mymath::scale(model, original * scalar);

				// mouse rotation of model
				// model = mymath::translate(model, model_center);
				model = mymath::rotate(model, mymath::radians(mouse_yaw), mymath::vec3(0.0f, 1.0f, 0.0f));	
				model = mymath::rotate(model, mymath::radians(mouse_pitch), mymath::vec3(1.0f, 0.0f, 0.0f));	
				model = mymath::translate(model, -(model_center));
				// model = mymath::translate(model, mymath::vec3(0.0f, 0.0f, 0.0f));
				// end of mouse rotation

			
				glUniformMatrix4fv(uniform_model, 1, GL_FALSE, mymath::value_ptr(model));


				GLfloat progress = std::clamp((now - transition_start) / duration, 0.0f, 1.0f);

				if (!transition_active)
				{
					// glEnable(GL_SCISSOR_TEST);
					// glScissor(200, 200, buffer_width - 500, buffer_height - 300);
					glUniform1i(uniform_surface_switch, static_cast<GLint>(new_state));
					new_model.renderModel(uniform_in_color, new_state);
					// glDisable(GL_SCISSOR_TEST);
				}
				else
				{
					glEnable(GL_SCISSOR_TEST);
					GLsizei edge = static_cast<GLsizei>(progress * buffer_width);
					glScissor(0, 0, edge, buffer_height);
					glUniform1i(uniform_surface_switch, static_cast<GLint>(new_state));
					new_model.renderModel(uniform_in_color, new_state);
					if (!initial_wipe)
					{
						GLsizei edge = static_cast<GLsizei>(progress * buffer_width);
						glScissor(edge, 0, buffer_width - edge, buffer_height);
						glUniform1i(uniform_surface_switch, static_cast<GLint>(curr_state));
						new_model.renderModel(uniform_in_color, curr_state);
					}

					glDisable(GL_SCISSOR_TEST);

					if (progress >= 1.0f)
					{
						transition_active = false;
						initial_wipe = false;
						curr_state = new_state;
					}
				}
				

			glUseProgram(0);
			mainWindow.swapBuffers();
		}
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
    return 0;
}
