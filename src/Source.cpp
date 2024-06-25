#include <./src/Source.h>

// settings
const unsigned int SCR_WIDTH    = 800 * 2;
const unsigned int SCR_HEIGHT   = 800 * 2;
const bool DRAW_SPLINE            = true;


// timing
double currentTime   = 0.0;
float deltaTime     = 0.0f;
float lastFrame     = 0.0f;

unsigned int VAO, VBO, EBO, cubeVAO;

unsigned int texture1, texture2;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

bool flashlight_on = false;

bool W, A, S, D, SHIFT;

glm::vec3 pointLightPositions[] = {
        glm::vec3(1, -1, -1.3),
        //glm::vec3(1.5, .75, 1.5),
        glm::vec3(-1.2f, 0.8f, 0),
        glm::vec3(1.2f, 0.0f, 0),
};

std::vector<glm::vec3> initialControlPoints = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 2.0f, 1.0f),
        glm::vec3(4.0f, -2.0f, 2.0f),
        glm::vec3(6.0f, 2.0f, 3.0f),
        glm::vec3(8.0f, 4.0f, 4.0f),
        glm::vec3(10.0f, -4.0f, 5.0f),
};

bool use_mouse = true;

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
    texture1 = load_texture("./resources/container2.png");
    texture2 = load_texture("./resources/container2_specular.png");
    Shader lightCubeShader("./GLSL/cube_lighting.vert", "./GLSL/cube_lighting.frag");
    float dims[] = { 3, 0, 0};
    float dims2[] = { 3, 3, 2};
    gen_geometry_buffers(cube_vertices, sizeof(cube_vertices), dims, sizeof(dims), cubeVAO, VBO, EBO);
    gen_geometry_buffers(cube_vertices_with_normals_textures, sizeof(cube_vertices_with_normals_textures), dims2, sizeof(dims2), VAO, VBO, EBO);
    

    Text calibri("./fonts/calibri.ttf", SCR_WIDTH, SCR_HEIGHT);

    Shader modelShader("./GLSL/lighting.vert", "./GLSL/lighting_multiple.frag");

    Model backpack("../3dModels/backpack/backpack.obj");
    Model Neon("../3dModels/Neon/Neon.fbx");

    float* NeonDims = Neon.getDimensions();

    // x: 0.0158436 y: 0.0237116 z: 0.0158436
    std::cout << "x: " << NeonDims[0] << "\ty: " << NeonDims[1] << "\tz: " << NeonDims[0] << std::endl;
    
    // tell lighting frag shader that TEXTURE0 is diffuse, TEXTURE1 is ambient
    int cubeDiffLoc = 1;
    int cubeSpecLoc = 0;

    /*
    lightingShader.use();
    // tell lighting frag shader that TEXTURE0 is diffuse, TEXTURE1 is ambient
    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setInt("material.specular", 1);
    */

    

    
    Spline s(initialControlPoints, Spline::SplineType::CatmullRomSpline);
    //points[0] = glm::vec3(2, 0, 0);
    //points[1] = glm::vec3(4, 2, 1);
    //points[2] = glm::vec3(6, 1, 0);
    //points[3] = glm::vec3(8, 4, 0);

    //glm::vec3 p0(1.7f, 2, -0.3f);
    //glm::vec3 p1(2.0f, -.01f, 0.6f);
    //glm::vec3 p2(3.1f, 1.4f, 1.0f);
    //glm::vec3 p3(3.7f, 0.23f, 0);

    //glm::vec3 p0(0, -5, -1);
    //glm::vec3 p1(0, 3, 5);
    //glm::vec3 p2(5, 0, -5);
    //glm::vec3 p3(-5, 5, 5);

    int amount = 25 * (initialControlPoints.size() - 1);
    int initialAmount = amount;

    std::vector<glm::vec3> splinePoints;
    std::vector<glm::vec3> distanceVec;

    splineMeshes(s, splinePoints, distanceVec, amount);

    NeonDims[1] = 0.119543;

    glm::vec3 cubePositions[] = {
        glm::vec3(2.0f,  2.0f,  2.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, 5.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -4.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(2.6f, -2.0f, -2.5f),
        glm::vec3(3.0f,  2.0f, -2.5f),
        glm::vec3(3.0f,  0.2f, -1.5f),
        glm::vec3(-2.6f,  1.0f, -1.5f)
    };

    std::vector<glm::vec3> polePositions = {
        glm::vec3(5,  0,  5),
        glm::vec3(5,  NeonDims[1] + 0.01,  5),
        glm::vec3(5,  NeonDims[1] + NeonDims[1] + 0.02,  5),
    };
    


    float lastTime = 0.0f;
    std::vector<glm::vec3> drawingControlPoints = initialControlPoints;
    int addedDrawPoints = 0;
    std::cout << "Starting loop..." << std::endl;
    while (!glfwWindowShouldClose(window))
    {
        #pragma region head
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        int fps = (int)(1 / deltaTime);
        lastTime = currentTime;
        // input: [stay at top]
        process_input(window);

        //background color [stay at top]
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Add ImGui elements (e.g., windows, buttons, sliders)
        ImGui::Begin("Hello, world!");
        ImGui::Text("This is some useful text.");

        srand(currentTime);
        if (ImGui::Button("Add Spline Points") & DRAW_SPLINE) {
            int randx = rand() % 5 + 1;
            int randy = rand() % 5 + 1;
            int randz = rand() % 5 + 1;

            bool success = false;
            if (!splinePoints.empty()) {
                success = s.AddControlPoint(glm::vec3(randx, randy, randz));
            }
            else {
                success = s.AddControlPoint(glm::vec3(randx, randy, randz));
            }
            std::cout << randx << " " << randy << " " << randz << std::endl;
            if (success) {
                amount += initialAmount / 4;
                splinePoints.clear();
                distanceVec.clear();
                splineMeshes(s, splinePoints, distanceVec, amount);
            }
        }
        if (ImGui::Button("Evaluate Spline Points") & DRAW_SPLINE) {
			splinePoints.clear();
			distanceVec.clear();
			splineMeshes(s, splinePoints, distanceVec, amount);
        }


        modelShader.use();
        perspective_projection(modelShader);

        // camera [must go after shader activation]
        glm::mat4 view = camera.GetViewMatrix();
        modelShader.setMat4("view", view);

        modelShader.setVec3("viewPos", camera.Position);
        modelShader.setFloat("material.shininess", 64.0f);
        #pragma endregion

        

        //directional light
        modelShader.setVec3("dirLight.direction",   -1, -1, 1); // directional light
        modelShader.setVec3("dirLight.ambient",     glm::vec3(0.05f));
        modelShader.setVec3("dirLight.diffuse", glm::vec3(0.2f));
        modelShader.setVec3("dirLight.specular", 0.2f, 0.2f, 0.2f);

        // spotlight:
        if (flashlight_on) {
            modelShader.setVec3("spotLight.position", camera.Position);
            modelShader.setVec3("spotLight.direction", camera.Front);
            modelShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(3.5f)));
            modelShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(6.0f)));
            modelShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
            modelShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
            modelShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
            modelShader.setVec3("spotLight.attenuationCoefficients", attenuationCoefficients[5]);
        }
        else {
            modelShader.setVec3("spotLight.attenuationCoefficients", glm::vec3(0, 0, 0));
        }


        // pointlight:
        std::string light;
        for (int i = 0; i < sizeof(pointLightPositions) / sizeof(pointLightPositions[0]); i++) {
            light = "pointLights[" + std::to_string(i) + "]";
            modelShader.setVec3(light + ".position", pointLightPositions[i]);
            modelShader.setVec3(light + ".ambient", glm::vec3(0.1f));
            modelShader.setVec3(light + ".diffuse", glm::vec3(0.5f));
            modelShader.setVec3(light + ".specular", glm::vec3(1.0f));
            modelShader.setVec3(light + ".attenuationCoefficients", attenuationCoefficients[2]);
        }

        glm::mat4 model = glm::mat4(1.0f);


        // draw the loaded model
        modelShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        modelShader.setMat4("model", model);
        backpack.Draw(modelShader);
    
        // draw the Neons
        if (DRAW_SPLINE == true) {
            int alterIndex = 3;
            float alterSpeed = deltaTime;
            bool splineAltered = false;
            if(W && SHIFT) {
				s.ChangeControlPoint(alterIndex, WORLD_FORWARD, alterSpeed);
                splineAltered = true;
			}
            if (S && SHIFT) {
                s.ChangeControlPoint(alterIndex, WORLD_BACKWARD, alterSpeed);
                splineAltered = true;
            }
            if (A && SHIFT) {
				s.ChangeControlPoint(alterIndex, WORLD_LEFT, alterSpeed);
                splineAltered = true;
			}
            if (D && SHIFT) {
                s.ChangeControlPoint(alterIndex, WORLD_RIGHT, alterSpeed);
                splineAltered = true;
            }
            if (splineAltered) {
                splinePoints.clear();
                distanceVec.clear();
                splineMeshes(s, splinePoints, distanceVec, amount);
                splineAltered = false;
            }


            modelShader.use();
            modelShader.setVec4("customColor", glm::vec4(0.6666f, 0.4666f, 1.0f, 1.0f));
            for (int i = 0; i < splinePoints.size(); i++) {
                model = glm::mat4(1.0f);



                model = glm::translate(model, splinePoints[i]);
                //model = glm::rotate(model, PI / 2, glm::vec3(0, 0, 1));
                float x = distanceVec[i].x;
                float y = distanceVec[i].y;
                float z = distanceVec[i].z;

                //lastQuat = glm::radians(lastQuat);

                glm::vec3 toQuat(atan(y / z), atan(z / x), atan(y / x));
                toQuat = abs(toQuat);
                toQuat = glm::normalize(toQuat);
                glm::quat quat = LookAt(distanceVec[i], WORLD_UP);

                glm::mat4 RotationMatrix = glm::toMat4(quat);
                model = model * RotationMatrix;
                //model = glm::rotate(model, rots[i].y, glm::vec3(0, 0, 1));
                //model = glm::rotate(model, rots[i].z, glm::vec3(1, 0, 0));

                //model = glm::scale(model, glm::vec3(0.5f));	// it's a bit too big for our scene, so scale it down


                modelShader.setMat4("model", model);
                Neon.Draw(modelShader);

            }
            modelShader.setVec4("customColor", glm::vec4(0.6666f, 0.4666f, 1.0f, 0.0f));
        }
        
        

        // draw non model cubes
        modelShader.setInt("material.texture_diffuse1", cubeDiffLoc);
        modelShader.setInt("material.texture_specular1", cubeSpecLoc);
        // bind diffuse map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture1);
        // bind specular map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glBindVertexArray(VAO);

        for (int i = 0; i < sizeof(cubePositions) / sizeof(cubePositions[0]); i++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            //model = glm::rotate(model, deg2rad(angle), glm::vec3(1.0f, 0.3f, 0.5f));

            //do_transformations(lightingShader, model);

            modelShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

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

        // control points
        std::vector<glm::vec3> drawPoints = s.GetControlPoints();
        for (int i = 0; i < drawPoints.size(); i++) {
            float maxDim = glm::max(glm::max(drawPoints[i].x, drawPoints[i].y), drawPoints[i].z);
            lightCubeShader.setVec4("customColor", glm::vec4(drawPoints[i].x / maxDim, drawPoints[i].y / maxDim, drawPoints[i].z / maxDim, 1));

            model = glm::mat4(1.0f);
            model = glm::translate(model, drawPoints[i]);
            model = glm::scale(model, glm::vec3(0.1f)); // a smaller cube
            lightCubeShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        modelShader.use();
        modelShader.setVec4("customColor", glm::vec4(1, 1, 1.0f, 1.0f));
        for (int i = 0; i < polePositions.size(); i++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, polePositions[i]);

            model = glm::rotate(model, PI / 2, glm::vec3(1, 0, 0 ));

            modelShader.setMat4("model", model);

            Neon.Draw(modelShader);
        }
        
        lightCubeShader.setVec4("customColor", glm::vec4(0, 0, 1, 0));
        modelShader.setVec4("customColor", glm::vec4(0, 0, 0, 0));


        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); FOR EBO
        
        // glBindVertexArray(0);  

        //text();
        calibri.render_text("Kyle Krause OpenGL 3.3", 25.0f, 25.0f, 0.5f, glm::vec3(0.5, 0.5, 0.5));
        calibri.render_text(std::to_string(fps), (SCR_WIDTH - 60) + 0.0f, (SCR_HEIGHT - 30) + 0.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));

        //imgui [stay at bottom]
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // check and call events and swap the buffers: (double buffered) [stay at bottom]
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    clear_glfw();
    return 0;
}


bool ESC_pressed, LCTRL_pressed, LSHIFT_pressed;
bool W_pressed, A_pressed, S_pressed, D_pressed;
bool F_pressed;
bool UP_pressed, DOWN_pressed;
bool SPACE_pressed;
bool I_pressed, J_pressed, K_pressed, L_pressed;
bool P_pressed, SEMICOLON_pressed;
bool C_pressed;



void process_input(GLFWwindow* window) {
    ESC_pressed         = glfwGetKey(window, GLFW_KEY_ESCAPE)       == GLFW_PRESS;
    W_pressed           = glfwGetKey(window, GLFW_KEY_W)            == GLFW_PRESS;
    S_pressed           = glfwGetKey(window, GLFW_KEY_S)            == GLFW_PRESS;
    A_pressed           = glfwGetKey(window, GLFW_KEY_A)            == GLFW_PRESS;
    D_pressed           = glfwGetKey(window, GLFW_KEY_D)            == GLFW_PRESS;
    UP_pressed          = glfwGetKey(window, GLFW_KEY_UP)           == GLFW_PRESS;
    DOWN_pressed        = glfwGetKey(window, GLFW_KEY_DOWN)         == GLFW_PRESS;
    SPACE_pressed       = glfwGetKey(window, GLFW_KEY_SPACE)        == GLFW_PRESS;
    LCTRL_pressed       = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;
    LSHIFT_pressed      = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)   == GLFW_PRESS;
    F_pressed           = glfwGetKey(window, GLFW_KEY_F)            == GLFW_PRESS;
    I_pressed           = glfwGetKey(window, GLFW_KEY_I)            == GLFW_PRESS;
    J_pressed           = glfwGetKey(window, GLFW_KEY_J)            == GLFW_PRESS;
    K_pressed           = glfwGetKey(window, GLFW_KEY_K)            == GLFW_PRESS;
    L_pressed           = glfwGetKey(window, GLFW_KEY_L)            == GLFW_PRESS;
    P_pressed           = glfwGetKey(window, GLFW_KEY_P)            == GLFW_PRESS;
    SEMICOLON_pressed   = glfwGetKey(window, GLFW_KEY_SEMICOLON)    == GLFW_PRESS;
    C_pressed           = glfwGetKey(window, GLFW_KEY_C)            == GLFW_PRESS;

    static bool F_down = true;
    static bool C_down = true;

    int lightCubeIndex = 0;
    float lightCubeMoveSpeed = 1.0f * deltaTime;

    if (ESC_pressed) {
        glfwSetWindowShouldClose(window, true);
    }

    if (W_pressed) {
        W = true;
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    else {
		W = false;
	}
    if (S_pressed) {
        S = true;
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    else {
        S = false;
    }
    if (A_pressed) {
        A = true;
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    else {
        A = false;
    }
    if (D_pressed) {
        D = true;
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    else {
        D = false;
    }
    if(LSHIFT_pressed) {
        SHIFT = true;
        camera.ProcessKeyboard(MODIFIER, deltaTime);
    }
    else{
        SHIFT = false;
	}
    
    if (UP_pressed || SPACE_pressed) {
        camera.ProcessKeyboard(UP, deltaTime);
    }
    if (DOWN_pressed || LCTRL_pressed) {
        camera.ProcessKeyboard(DOWN, deltaTime);
    }

    if (F_pressed) {
        if (F_down) {
            flashlight_on = !flashlight_on;
        }
        F_down = false;
    }
    else {
        F_down = true;
    }

    if (I_pressed) {
        pointLightPositions[lightCubeIndex] += WORLD_FORWARD * lightCubeMoveSpeed;
    }
    if (K_pressed) {
        pointLightPositions[lightCubeIndex] += WORLD_BACKWARD * lightCubeMoveSpeed;
    }
    if (J_pressed) {
        pointLightPositions[lightCubeIndex] += WORLD_LEFT * lightCubeMoveSpeed;
    }
    if (L_pressed) {
        pointLightPositions[lightCubeIndex] += WORLD_RIGHT * lightCubeMoveSpeed;
    }
    if (P_pressed) {
        pointLightPositions[lightCubeIndex] += WORLD_UP * lightCubeMoveSpeed;
    }
    if (SEMICOLON_pressed) {
        pointLightPositions[lightCubeIndex] += WORLD_DOWN * lightCubeMoveSpeed;
    }
    if (C_pressed) {
        if (C_down) {
            use_mouse = !use_mouse; // toggle mouse in window

            if (use_mouse) {
                glfwSetCursorPos(window, SCR_WIDTH / 2, SCR_HEIGHT / 2);
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            else {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }

            C_down = false;
        }
    }
    else {
        C_down = true;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    float xoffset = 0;
    float yoffset = 0;
    if (use_mouse) {
        glfwSetCursorPos(window, SCR_WIDTH / 2, SCR_HEIGHT / 2);

        xoffset = xpos - (SCR_WIDTH / 2);
        yoffset = (SCR_HEIGHT / 2) - ypos;
        /*float lastX = xpos;
        float lastY = ypos;*/

        float sensitivity = 0.5f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;
        
        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
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

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    return 0;
}

void clear_glfw() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glDeleteBuffers(1, &EBO);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    glfwTerminate();
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

float magnitude(glm::vec3 vector) {
    float result = 0.0f;

    result += vector.x * vector.x;
    result += vector.y * vector.y;
    result += vector.z * vector.z;

    return sqrt(result);
}

// adapted from
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-17-quaternions/
// Returns a quaternion such that q*start = dest
glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 dest) {
    start = normalize(start);
    dest = normalize(dest);

    float cosTheta = glm::dot(start, dest);
    glm::vec3 rotationAxis;

    if (cosTheta < -1 + 0.001f) {
        // special case when vectors in opposite directions :
        // there is no "ideal" rotation axis
        // So guess one; any will do as long as it's perpendicular to start
        // This implementation favors a rotation around the Up axis,
        // since it's often what you want to do.
        rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), start);
        if (glm::length2(rotationAxis) < 0.01) // bad luck, they were parallel, try again!
            rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);

        rotationAxis = normalize(rotationAxis);
        return glm::angleAxis(glm::radians(180.0f), rotationAxis);
    }

    // Implementation from Stan Melax's Game Programming Gems 1 article
    rotationAxis = glm::cross(start, dest);

    float s = sqrt((1 + cosTheta) * 2);
    float invs = 1 / s;

    return glm::quat(
        s * 0.5f,
        rotationAxis.x * invs,
        rotationAxis.y * invs,
        rotationAxis.z * invs
    );


}


glm::quat LookAt(glm::vec3 direction, glm::vec3 desiredUp) {

    if (glm::length2(direction) < 0.0001f)
        return glm::quat();

    // Recompute desiredUp so that it's perpendicular to the direction
    // You can skip that part if you really want to force desiredUp
    glm::vec3 right = glm::cross(direction, desiredUp);
    desiredUp = cross(right, direction);

    // Find the rotation between the front of the object (that we assume towards +Z,
    // but this depends on your model) and the desired direction
    glm::quat rot1 = RotationBetweenVectors(glm::vec3(0.0f, 0.0f, 1.0f), direction);
    // Because of the 1rst rotation, the up is probably completely screwed up. 
    // Find the rotation between the "up" of the rotated object, and the desired up
    glm::vec3 newUp = rot1 * glm::vec3(0.0f, 1.0f, 0.0f);
    glm::quat rot2 = RotationBetweenVectors(newUp, desiredUp);

    // Apply them
    return rot2 * rot1; // remember, in reverse order.
}

// TODO: Change this to return vector<vec3> for spline points
void splineMeshes(Spline &s, std::vector<glm::vec3>& splinePoints, std::vector<glm::vec3>& distanceVec, int amount) {

    for (int i = 0; i < amount; i++) {

        //glm::vec3 SplinePoint = Spline::Hermite(calcPoints[3], calcPoints[0], calcPoints[1], calcPoints[2], i / (float) amount);
        glm::vec3 SplinePoint = s.Evaluate(i / (float)amount);

        splinePoints.push_back(SplinePoint);
    }

    
    int i;
    for (i = 0; i < amount - 1; i++) {
        distanceVec.push_back(glm::normalize(splinePoints[i + 1] - splinePoints[i]));
    }
    distanceVec.push_back(-1.0f * distanceVec[amount - 2]);
}