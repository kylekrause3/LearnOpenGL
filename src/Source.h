#include <iostream>
#include <map>

#include <./src/verts.h>

#include <glad/glad.h> //glad first be sure to include GLAD before other header files that require OpenGL (ex. glfw)
#include <GLFW/glfw3.h>

#include <./src/Shader.h>
#include <./src/Camera.h>
#include <./src/Text.h>

#include <./src/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

// method prototypes

void process_input(GLFWwindow* window);
GLFWwindow* create_window();
int glfw_configuration(GLFWwindow* window);
void clear_glfw();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void gen_geometry_buffers(float* verts, float verts_size, float* dimensions, float dimensions_size);
void load_textures();
void perspective_projection(Shader& ourShader);
void do_transformations(Shader& ourShader, glm::mat4& model);

const float PI = 3.1415927;
const float TAU = 6.2831853;

float deg2rad(float x) { return x * (PI / 180); }
float rad2deg(float x) { return x * (180 / PI); }

const glm::vec3 WORLD_UP(0, 1, 0);
const glm::vec3 WORLD_DOWN(0, -1, 0);
const glm::vec3 WORLD_RIGHT(1, 0, 0);
const glm::vec3 WORLD_LEFT(-1, 0, 0);
const glm::vec3 WORLD_FORWARD(0, 0, -1);
const glm::vec3 WORLD_BACKWARD(0, 0, 1);