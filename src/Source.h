#include <iostream>
#include <map>

#include <glad/glad.h> //glad first be sure to include GLAD before other header files that require OpenGL (ex. glfw)
#include <GLFW/glfw3.h>

#include <./src/Shader.h>
#include <./src/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color);
void gen_geometry_buffers(float* verts, float verts_size, float* dimensions, float dimensions_size);
void gen_text_buffers();
void load_text(const char* font);
GLFWwindow* create_window();
void terminate();
unsigned int* load_textures();
float deg2rad(float x);
float rad2deg(float x);
void perspectiveProjection(Shader& ourShader);
void do_transformations(Shader& ourShader, glm::mat4& model);
void do_projections(Shader& ourShader);

const float PI = 3.1415927;
const float TAU = 6.2831853;

const glm::vec3 UP(0, 1, 0);