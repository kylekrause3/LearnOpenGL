#include <./src/Source.h>

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;


// timing
float currentTime = 0.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

unsigned int VAO, VBO, EBO, VAO_text, VBO_text;

// FreeType Character state information
struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};

// Holds all characters
std::map<GLchar, Character> Characters;

unsigned int texture1, texture2;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

int main() {
#pragma region initialization

    GLFWwindow* window = create_window();
    // create glfw window
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0); // vsync off
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    Shader ourShader("./GLSL/default.vert", "./GLSL/default.frag");
    Shader textShader("./GLSL/text_default.vert", "./GLSL/text_default.frag");
    Shader lightingShader("./GLSL/lighting.vert", "./GLSL/lighting.frag");
    Shader lightCubeShader("./GLSL/cube_lighting.vert", "./GLSL/cube_lighting.frag");

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
    textShader.use();
    glUniformMatrix4fv(glGetUniformLocation(textShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    load_text("./fonts/calibri.ttf");

#pragma endregion
    
    //load_textures();

    //gen_geometry_buffers(float verts[], int positions, int colors, int textures)
    //gen_geometry_buffers(shape_vertices, sizeof(shape_vertices), 3, 3, 2);

    float dims[] = {3, 0, 0};
    //gen_geometry_buffers(shape_vertices, sizeof(shape_vertices), dims, sizeof(dims));
    //gen_geometry_buffers(cube_vertices_with_textures, sizeof(cube_vertices_with_textures), dims, sizeof(dims));
    gen_geometry_buffers(cube_vertices, sizeof(cube_vertices), dims, sizeof(dims));


#pragma region Render Loop
    ourShader.use(); // don't forget to activate/use the shader before setting uniforms!
    // set uniforms via class:
    ourShader.setInt("texture1", 0);
    ourShader.setInt("texture2", 1);

    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    
    float lastTime = 0.0f;
    while (!glfwWindowShouldClose(window))
    {
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        int fps = (int)(1 / deltaTime);
        lastTime = currentTime;
        // input: [stay at top]
        processInput(window);

        // activate shader
        ourShader.use();
        perspectiveProjection(ourShader);

        // camera
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);

        // render:
        //background color
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
        
        //commenting out in colors section, because load_textures() is no longer called
        /*
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        */
        

        // draw shapes
        glBindVertexArray(VAO);
        for (int i = 0; i < sizeof(cubePositions) / sizeof(cubePositions[0]); i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, deg2rad(angle), glm::vec3(1.0f, 0.3f, 0.5f));

            do_transformations(ourShader, model);

            int modelUniformLocation = glGetUniformLocation(ourShader.ID, "model");
            glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); FOR EBO
        
        glBindVertexArray(0);

        


        //text();
        RenderText(textShader, "Kyle Krause OpenGL 3.3", 25.0f, 25.0f, 0.5f, glm::vec3(0.5, 0.5, 0.5));
        RenderText(textShader, std::to_string(fps), (SCR_WIDTH - 60) + 0.0f, (SCR_HEIGHT - 30) + 0.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));

        // check and call events and swap the buffers: (double buffered) [stay at bottom]
        glfwSwapBuffers(window);
        glfwPollEvents();

        
    }
#pragma endregion

    terminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    glfwSetCursorPos(window, SCR_WIDTH / 2, SCR_HEIGHT / 2);

    float xoffset = xpos - (SCR_WIDTH / 2);
    float yoffset = (SCR_HEIGHT / 2) - ypos;
    /*float lastX = xpos;
    float lastY = ypos;*/

    float sensitivity = 0.5f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

bool ESC_pressed, LCTRL_pressed, LSHIFT_pressed;
bool W_pressed, A_pressed, S_pressed, D_pressed;
bool UP_pressed, DOWN_pressed;
bool SPACE_pressed;

void processInput(GLFWwindow* window) {
    ESC_pressed     = glfwGetKey(window, GLFW_KEY_ESCAPE)       == GLFW_PRESS;
    W_pressed       = glfwGetKey(window, GLFW_KEY_W)            == GLFW_PRESS;
    S_pressed       = glfwGetKey(window, GLFW_KEY_S)            == GLFW_PRESS;
    A_pressed       = glfwGetKey(window, GLFW_KEY_A)            == GLFW_PRESS;
    D_pressed       = glfwGetKey(window, GLFW_KEY_D)            == GLFW_PRESS;
    UP_pressed      = glfwGetKey(window, GLFW_KEY_UP)           == GLFW_PRESS;
    DOWN_pressed    = glfwGetKey(window, GLFW_KEY_DOWN)         == GLFW_PRESS;
    SPACE_pressed   = glfwGetKey(window, GLFW_KEY_SPACE)        == GLFW_PRESS;
    LCTRL_pressed   = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;
    LSHIFT_pressed  = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)   == GLFW_PRESS;

    if (ESC_pressed) {
        glfwSetWindowShouldClose(window, true);
    }

    if (W_pressed) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (S_pressed) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (A_pressed) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (D_pressed) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    if (UP_pressed || SPACE_pressed) {
        camera.ProcessKeyboard(UP, deltaTime);
    }
    if (DOWN_pressed || LCTRL_pressed) {
        camera.ProcessKeyboard(DOWN, deltaTime);
    }
    if (LSHIFT_pressed) {
        camera.ProcessKeyboard(MODIFIER, deltaTime);
    }
}

GLFWwindow* create_window() {
    glfwInit(); //initialize glfw
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    return glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
}

void terminate() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &VAO_text);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &VBO_text);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();
}

void gen_geometry_buffers(float *verts, float verts_size, float *dimensions, float dimensions_size) {
    //VBO vertex buffer object, deals with verticies
    glGenBuffers(1, &VBO);

    //EBO element buffer object, deals with indices
    //glGenBuffers(1, &EBO);
    //vertex array object, contains VBO
    glGenVertexArrays(1, &VAO);
    // 1. bind Vertex Array Object
    glBindVertexArray(VAO);
    // 2. copy our vertices array in a vertex buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verts_size, verts, GL_STATIC_DRAW);
    // 3. copy our index array in a element buffer for OpenGL to use
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // 4. then set the vertex attributes pointers

    // *THIS IS THE STRIDE AND OFFSET STUFF:
    
    //glVertAtPt(index (location in shader), dimensions, dim_type, normalized?, stride, offset)
    int stride = 0;

    int i;
    int dimensions_length = dimensions_size / sizeof(dimensions[0]);
    for (i = 0; i < dimensions_length; i++) {
        stride += dimensions[i];
    }

    int lastOffset = 0;
    for (i = 0; i < dimensions_length; i++) {
        if (dimensions[i] != 0) {
            glVertexAttribPointer(i, dimensions[i], GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(lastOffset * sizeof(float)));
            glEnableVertexAttribArray(i);
        }

        lastOffset += dimensions[i];
    }
}

void gen_text_buffers() {
    glGenVertexArrays(1, &VAO_text);
    glGenBuffers(1, &VBO_text);
    glBindVertexArray(VAO_text);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_text);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void load_text(const char* font) {
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        terminate();
    }

    FT_Face face;
    if (FT_New_Face(ft, font, 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        terminate();
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }

    //clear freetype once done
    FT_Done_Face(face);
    FT_Done_FreeType(ft);


    // text vao/vbo
    gen_text_buffers();
}

void load_textures() {
    //FOR GL_CLAMP_TO_BORDER

    //float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f }; // must define border color
    //glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    int tex_width, tex_height, nrChannels;
    unsigned char* tex_data;
    stbi_set_flip_vertically_on_load(true);

    // texture 1
    // ---------
    glGenTextures(1, &texture1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // texture wrapping behavior
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // GL_REPEAT on s (x) axis
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // GL_REPEAT on t (y) axis
    // texture zoom filtering behavior
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    tex_data = stbi_load("./resources/container.jpg", &tex_width, &tex_height, &nrChannels, 0);

    if (tex_data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(tex_data); // free image memory

    // texture 2
    // ---------
    glGenTextures(1, &texture2);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // texture wrapping behavior
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // GL_REPEAT on s (x) axis
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // GL_REPEAT on t (y) axis
    // texture zoom filtering behavior
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    tex_data = stbi_load("./resources/worms_andrew.png", &tex_width, &tex_height, &nrChannels, 0);

    if (tex_data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(tex_data); // free image memory
}

float deg2rad(float x) { return x * (PI / 180); }

float rad2deg(float x) { return x * (180 / PI); }

void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color)
{
    // activate corresponding render state	
    shader.use();
    glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO_text);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float text_vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO_text);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(text_vertices), text_vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void perspectiveProjection(Shader& ourShader) {
    ourShader.use();

    // view matrix translates world space coordinates to camera view space
    int viewUniformLocation = glGetUniformLocation(ourShader.ID, "view");
    glm::mat4 view = glm::mat4(1.0f);
    // note that we're translating the scene in the reverse direction of where we want to move
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    // projection matrix translates camera view space to clip space
    int projectionUniformLocation = glGetUniformLocation(ourShader.ID, "projection");
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    glUniformMatrix4fv(viewUniformLocation, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionUniformLocation, 1, GL_FALSE, glm::value_ptr(projection));
}

void do_transformations(Shader& ourShader, glm::mat4 &model) {
    // model matrix translates local object vertices into world space coordinates
    int modelUniformLocation = glGetUniformLocation(ourShader.ID, "model");
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    ourShader.use();

    model = glm::translate(model, glm::vec3(0, 0, 0));

    // rotation over time
    model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5, 1, 0));


    //model = glm::scale(model, glm::vec3(0.5f, 1.0f, 0.3f));

    glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, glm::value_ptr(model));
}

void do_projections(Shader& ourShader) {
    ourShader.use();

    //glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

}
