#include <string>
#include <map>

#include <glad/glad.h> //glad first be sure to include GLAD before other header files that require OpenGL (ex. glfw)
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <./src/Shader.h>

#include <ft2build.h>
#include FT_FREETYPE_H

// FreeType Character state information
struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};



class Text {
public:
    Text(const char* path, float scr_width, float scr_height);

    ~Text();

    void render_text(std::string text, float x, float y, float scale, glm::vec3 color);
protected:
    // Holds all characters
    std::map<GLchar, Character> Characters;
    unsigned int                VAO_text, VBO_text;
    Shader                      shader;

    void load_text(const char* font);

    void gen_text_buffers(unsigned int& VAO, unsigned int& VBO);
};