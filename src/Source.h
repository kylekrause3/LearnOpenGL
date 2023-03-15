#include <iostream>
#include <map>
#include <vector>

#include <glad/glad.h> //glad first be sure to include GLAD before other header files that require OpenGL (ex. glfw)
#include <GLFW/glfw3.h>

#include <./src/Shader.h>
#include <./src/Camera.h>
#include <./src/Text.h>
#include <./src/verts.h>
#include <./src/Spline.h>

#include <./src/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>


#include <ft2build.h>
#include FT_FREETYPE_H

#include <./src/Model.h>

// method prototypes

void			process_input(GLFWwindow* window);
GLFWwindow*		create_window();
int				glfw_configuration(GLFWwindow* window);
void			clear_glfw();
void			framebuffer_size_callback(GLFWwindow* window, int width, int height);
void			mouse_callback(GLFWwindow* window, double xpos, double ypos);
void			scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void			gen_geometry_buffers(float* verts, float verts_size, float* dimensions, float dimensions_size, unsigned int& VAO, unsigned int& VBO, unsigned int& EBO);
unsigned int	load_texture(const char* path);
void			perspective_projection(Shader& ourShader);
void			do_transformations(Shader& ourShader, glm::mat4& model);
float			magnitude(glm::vec3 vector);
glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 dest);
glm::quat LookAt(glm::vec3 direction, glm::vec3 desiredUp);

const float PI	= 3.1415927;
const float TAU = 6.2831853;

float deg2rad(float x) { return x * (PI / 180); }
float rad2deg(float x) { return x * (180 / PI); }

const glm::vec3 WORLD_UP(0, 1, 0);
const glm::vec3 WORLD_DOWN(0, -1, 0);
const glm::vec3 WORLD_RIGHT(1, 0, 0);
const glm::vec3 WORLD_LEFT(-1, 0, 0);
const glm::vec3 WORLD_FORWARD(0, 0, -1);
const glm::vec3 WORLD_BACKWARD(0, 0, 1);

const glm::vec3 attenuationCoefficients[] = {
	glm::vec3(1.0f, 0.7f,	 1.8f),
	glm::vec3(1.0f, 0.35f,	 0.44f),
	glm::vec3(1.0f, 0.22f,	 0.20f),
	glm::vec3(1.0f, 0.14f,	 0.07f),
	glm::vec3(1.0f, 0.09f,	 0.032f),
	glm::vec3(1.0f, 0.07f,	 0.017f),
	glm::vec3(1.0f, 0.045f,	 0.0075f),
	glm::vec3(1.0f, 0.027f,	 0.0028f),
	glm::vec3(1.0f, 0.022f,	 0.0019f),
	glm::vec3(1.0f, 0.014f,	 0.0007f),
	glm::vec3(1.0f, 0.007f,	 0.0002f),
	glm::vec3(1.0f, 0.0014f, 0.000007f),
};