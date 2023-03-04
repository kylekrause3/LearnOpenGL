#include <./src/Source.h>

// settings
const unsigned int SCR_WIDTH    = 800;
const unsigned int SCR_HEIGHT   = 800;


// timing
float currentTime   = 0.0f;
float deltaTime     = 0.0f;
float lastFrame     = 0.0f;

unsigned int VAO, VBO, EBO, cubeVAO;

unsigned int texture1, texture2;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

// lighting
//glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main() {
    GLFWwindow* window = create_window();
    // create glfw window
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    int glfw_configure_success = glfw_configuration(window);
    if (glfw_configure_success == -1) {
        return -1;
    }

    if (glfwRawMouseMotionSupported()) {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }
    stbi_set_flip_vertically_on_load(true);

    // light cubes
    Shader lightCubeShader("./GLSL/cube_lighting.vert", "./GLSL/cube_lighting.frag");
    float dims[] = { 3, 3, 2 };
    gen_geometry_buffers(cube_vertices_with_normals_textures, sizeof(cube_vertices_with_normals_textures), dims, sizeof(dims), cubeVAO, VBO, EBO);

    Text calibri("./fonts/calibri.ttf", SCR_WIDTH, SCR_HEIGHT);

    Shader modelShader("./GLSL/lighting.vert", "./GLSL/lighting_multiple.frag");

    Model backpack("../3dModels/backpack/backpack.obj");
    Model Neon("../3dModels/Neon/Neon.fbx");

    glm::vec3 pointLightPositions[] = {
        glm::vec3(1, -1, -1.3),
        //glm::vec3(1.5, .75, 1.5),
        glm::vec3(-1.2f, 0.8f, 0),
        glm::vec3(1.2f, 0.0f, 0),
    };


    float lastTime = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        currentTime = glfwGetTime();
        deltaTime   = currentTime - lastTime;
        int fps     = (int)(1 / deltaTime);
        lastTime    = currentTime;
        // input: [stay at top]
        process_input(window);

        //background color [stay at top]
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        modelShader.use();
        perspective_projection(modelShader);

        // camera [must go after shader activation]
        glm::mat4 view = camera.GetViewMatrix();
        modelShader.setMat4("view", view);

        modelShader.setVec3("viewPos", camera.Position);
        modelShader.setFloat("material.shininess", 64.0f);

        //directional light
        modelShader.setVec3("dirLight.direction", -1, -1, 2); // directional light
        modelShader.setVec3("dirLight.ambient", glm::vec3(0.0f));
        modelShader.setVec3("dirLight.diffuse", glm::vec3(0.1f));
        modelShader.setVec3("dirLight.specular", 0.2f, 0.2f, 0.2f);

        // spotlight:
        //modelShader.setVec3("spotLight.position", camera.Position);
        //modelShader.setVec3("spotLight.direction", camera.Front);
        //modelShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(3.5f)));
        //modelShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(6.0f)));
        //modelShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        //modelShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        //modelShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        //modelShader.setVec3("spotLight.attenuationCoefficients", attenuationCoefficients[5]);


        // pointlight:
        std::string light;
        for (int i = 0; i < sizeof(pointLightPositions) / sizeof(pointLightPositions[0]); i++) {
            light = "pointLights[" + std::to_string(i) + "]";
            modelShader.setVec3(light + ".position", pointLightPositions[i]);
            modelShader.setVec3(light + ".ambient", glm::vec3(0.6f));
            modelShader.setVec3(light + ".diffuse", glm::vec3(0.7f));
            modelShader.setVec3(light + ".specular", glm::vec3(1.0f));
            modelShader.setVec3(light + ".attenuationCoefficients", attenuationCoefficients[1]);
        }

        glm::mat4 model = glm::mat4(1.0f);


        // draw the loaded model
        modelShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        modelShader.setMat4("model", model);
        backpack.Draw(modelShader);


        // draw light cubes (point lights)
        lightCubeShader.use();
        perspective_projection(lightCubeShader);
        lightCubeShader.setMat4("view", view);
        glBindVertexArray(cubeVAO);
        for (int i = 0; i < sizeof(pointLightPositions) / sizeof(pointLightPositions[0]); i++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.1f)); // a smaller cube
            lightCubeShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); FOR EBO
        
        // glBindVertexArray(0);  

        //text();
        calibri.render_text("Kyle Krause OpenGL 3.3", 25.0f, 25.0f, 0.5f, glm::vec3(0.5, 0.5, 0.5));
        calibri.render_text(std::to_string(fps), (SCR_WIDTH - 60) + 0.0f, (SCR_HEIGHT - 30) + 0.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));

        // check and call events and swap the buffers: (double buffered) [stay at bottom]
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    clear_glfw();
    return 0;
}


bool ESC_pressed, LCTRL_pressed, LSHIFT_pressed;
bool W_pressed, A_pressed, S_pressed, D_pressed;
bool UP_pressed, DOWN_pressed;
bool SPACE_pressed;

void process_input(GLFWwindow* window) {
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

int glfw_configuration(GLFWwindow* window) {
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0); // vsync off
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return      -1;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    return 0;
}

void clear_glfw() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glDeleteBuffers(1, &EBO);
    glfwTerminate();
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

void gen_geometry_buffers(float *verts, float verts_size, float *dimensions, float dimensions_size, unsigned int &VAO, unsigned int& VBO, unsigned int &EBO) {
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

unsigned int load_texture(const char *path)
{
    unsigned int result;
    glGenTextures(1, &result);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, result);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
    }

    stbi_image_free(data); // free image memory
    return result;
}

void perspective_projection(Shader& shader) {
    shader.use();
    // projection matrix translates camera view space to clip space
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    shader.setMat4("projection", projection);
}

void do_transformations(Shader& ourShader, glm::mat4 &model) {
    // model matrix translates local object vertices into world space coordinates
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    ourShader.use();

    model = glm::translate(model, glm::vec3(0, 0, 0));

    // rotation over time
    model = glm::rotate(model, (float)currentTime, glm::vec3(0.5, 1, 0));

    //model = glm::scale(model, glm::vec3(0.5f, 1.0f, 0.3f));
    ourShader.setMat4("model", model);
}
