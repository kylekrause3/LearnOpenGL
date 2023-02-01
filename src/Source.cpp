#include <iostream>

#include <glad/glad.h> //glad first be sure to include GLAD before other header files that require OpenGL (ex. glfw)
#include <GLFW/glfw3.h>

#include "../Shader.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    glfwInit(); //initialize glfw
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); 
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif


    // create glfw window
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader ourShader("default.vert", "default.frag");

    //HELLO TRIANGLE:

    float vertices[] = {
        //  x      y      z      R      G      B
        // positions
         0.5f,  0.5f,  0.0f,  0.0f,  0.0f,  1.0f, // top right
         0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, // bottom right
        -0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  0.0f, // bottom left
        -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f  // top left
    };

    unsigned int indices[]{
        0, 1, 3, //triangle 1
        1, 2, 3  //triangle 2
    };

    //VBO vertex buffer object, deals with verticies
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    //EBO element buffer object, deals with indices
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    unsigned int VAO; //vertex array object, contains VBO
    glGenVertexArrays(1, &VAO);
    // 1. bind Vertex Array Object
    glBindVertexArray(VAO);
    // 2. copy our vertices array in a vertex buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 3. copy our index array in a element buffer for OpenGL to use
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // 4. then set the vertex attributes pointers
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);



    /* RENDER LOOP */
    while (!glfwWindowShouldClose(window))
    {
        // input:
        processInput(window);

        // rendering commands here:
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // activate shader
        ourShader.use();

        // update the uniform color
        float timeValue = glfwGetTime();
        float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
        int vertexColorLocation = glGetUniformLocation(ourShader.ID, "ourColor");
        glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f); // actually set the uniform at location to the color we made


        // draw shapes
        glBindVertexArray(VAO);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0);

        // check and call events and swap the buffers: (double buffered)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// used for making sure that the viewport dimensions change with the window size, 
// called by glfw on framesize change
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) 
        glfwSetWindowShouldClose(window, true);
}

//ONTO: Shaders