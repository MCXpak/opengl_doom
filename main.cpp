#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader_s.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Camera_c.h"
#include "Entity.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 400, lastY = 300;
bool firstMouse = true;
float fov = 45.0f;

glm::vec2 spriteFaceDirection = glm::vec2(0, 1);
float alpha;

float moveInc = 0.01;
float maxMove = 10;
float minMove = -10;
float currentPosX = 0;
float currentPosY = 0;

Camera Cam(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 1.0f, 0.0f), YAW, PITCH);

int main()
{

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float floor_vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    };

    int floor_coords[8][8] = {
        {1,1,1,0,0,0,1,1},
        {0,0,1,1,0,0,1,0},
        {0,0,0,1,1,1,1,0},
        {0,1,0,1,0,0,0,0},
        {0,1,1,1,1,1,1,1},
        {0,1,1,0,0,0,0,1},
        {0,0,0,0,1,1,1,1},
        {0,0,0,0,1,1,1,1},
    };

    //FLOOR

    unsigned int VAO_floor;
    glGenVertexArrays(1, &VAO_floor);
    glBindVertexArray(VAO_floor);

    unsigned int VBO_floor;
    glGenBuffers(1, &VBO_floor);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_floor);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floor_vertices), floor_vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    //generate and bind texture object
    unsigned int floor_texture;
    glGenTextures(1, &floor_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, floor_texture);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //load and generate texture

    stbi_set_flip_vertically_on_load(true);

    int width, height, nrChannels;
    unsigned char* data = stbi_load("./assets/metal_floor.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data); //free image memory

    //WALL

    unsigned int VAO_wall;
    glGenVertexArrays(1, &VAO_wall);
    glBindVertexArray(VAO_wall);

    unsigned int VBO_wall;
    glGenBuffers(1, &VBO_wall);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_wall);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floor_vertices), floor_vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //generate and bind texture object
    unsigned int wall_texture;
    glGenTextures(1, &wall_texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, wall_texture);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //load and generate texture

    stbi_set_flip_vertically_on_load(true);

    data = stbi_load("./assets/metal_wall.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data); //free image memory

    //ENEMY SPRITE
    float sprite_vertices[4][30] = {
        {
        -0.25f, -0.5f, 0.0f,  0.0f, 0.75f,
         0.25f, -0.5f, 0.0f,  1.0f, 0.75f,
         0.25f,  0.5f, 0.0f,  1.0f, 1.0f,
         0.25f,  0.5f, 0.0f,  1.0f, 1.0f,
        -0.25f,  0.5f, 0.0f,  0.0f, 1.0f,
        -0.25f, -0.5f, 0.0f,  0.0f, 0.75f,
        },
        {
        -0.25f, -0.5f, 0.0f,  0.0f, 0.50f,
         0.25f, -0.5f, 0.0f,  1.0f, 0.50f,
         0.25f,  0.5f, 0.0f,  1.0f, 0.75f,
         0.25f,  0.5f, 0.0f,  1.0f, 0.75f,
        -0.25f,  0.5f, 0.0f,  0.0f, 0.75f,
        -0.25f, -0.5f, 0.0f,  0.0f, 0.50f,
        },
        {
        -0.25f, -0.5f, 0.0f,  0.0f, 0.25f,
         0.25f, -0.5f, 0.0f,  1.0f, 0.25f,
         0.25f,  0.5f, 0.0f,  1.0f, 0.50f,
         0.25f,  0.5f, 0.0f,  1.0f, 0.50f,
        -0.25f,  0.5f, 0.0f,  0.0f, 0.50f,
        -0.25f, -0.5f, 0.0f,  0.0f, 0.25f,
        },
        {
        -0.25f, -0.5f, 0.0f,  0.0f, 0.0f,
         0.25f, -0.5f, 0.0f,  1.0f, 0.0f,
         0.25f,  0.5f, 0.0f,  1.0f, 0.25f,
         0.25f,  0.5f, 0.0f,  1.0f, 0.25f,
        -0.25f,  0.5f, 0.0f,  0.0f, 0.25f,
        -0.25f, -0.5f, 0.0f,  0.0f, 0.0f,
        }
    };

    GLuint VAO_sprites[4];
    glGenVertexArrays(4, VAO_sprites);

    GLuint VBO_sprites[4];
    glGenBuffers(4, VBO_sprites);

    for (int i = 0; i < 4; i++) {
        glBindVertexArray(VAO_sprites[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_sprites[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(sprite_vertices[i]), sprite_vertices[i], GL_STATIC_DRAW);
        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texture attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    //generate and bind texture object
    unsigned int sprite_texture;
    glGenTextures(1, &sprite_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sprite_texture);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //load and generate texture

    stbi_set_flip_vertically_on_load(true);

    data = stbi_load("./assets/monster_spritesheet.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data); //free image memory


    Shader ourShader("./shader.vs", "./shader.fs");

    unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
    unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
    unsigned int viewLoc = glGetUniformLocation(ourShader.ID, "view");
    unsigned int projectionLoc = glGetUniformLocation(ourShader.ID, "projection");

    Cam.MouseSensitivity = 0.1f;

    Entity monster(sprite_vertices, &Cam, &ourShader);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwSwapBuffers(window);
        glfwPollEvents();
        processInput(window);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(window, mouse_callback);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();
        ourShader.setInt("texture1", 0);
        //ourShader.setInt("texture2", 1);

        glm::mat4 projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);

        glm::mat4 view;
        view = Cam.GetViewMatrix(); //lookAt(Pos, Pos + Front, Up)

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
        glBindVertexArray(VAO_floor);

        ////generate floor
        //for (int i = 7; i >= 0; i--) {
        //    for (int j = 0; j < 8; j++) {
        //        if (floor_coords[i][j] == 1) {
        //            
        //            glm::mat4 model = glm::mat4(1.0f);
        //            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(-1.0, 0.0, 0.0));
        //            model = glm::translate(model, glm::vec3(j,i,0));
        //            glBindTexture(GL_TEXTURE_2D, floor_texture);
        //            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        //            glDrawArrays(GL_TRIANGLES, 0, 6);

        //            //WALL
        //            //use floor coordinates to check where to place wall e.g. if floor coord = 1, check surronding coords, if surrounding is 0, place wall. if no surrounding i.e. out of coord space, place wall
        //            //FRONT

        //            //glActiveTexture(GL_TEXTURE1); // activate the texture unit first before binding texture
        //            glBindTexture(GL_TEXTURE_2D, wall_texture);
        //            glBindVertexArray(VAO_wall);
        //            

        //            if ( i + 1 > 7 || floor_coords[i + 1][j] == 0) {
        //                glm::mat4 model = glm::mat4(1.0f);
        //                model = glm::rotate(model, glm::radians(90.0f), glm::vec3(-1.0, 0.0, 0.0));
        //                model = glm::translate(model, glm::vec3(j, i+1, 0));
        //                model = glm::rotate(model, glm::radians(90.0f), glm::vec3(-1.0, 0.0, 0.0));
        //                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        //                glDrawArrays(GL_TRIANGLES, 0, 6);
        //            }
        //            //BACK
        //            if (i - 1 < 0 || floor_coords[i - 1][j] == 0) {
        //                glm::mat4 model = glm::mat4(1.0f);
        //                model = glm::rotate(model, glm::radians(90.0f), glm::vec3(-1.0, 0.0, 0.0));
        //                model = glm::translate(model, glm::vec3(j, i - 1, 0));
        //                model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
        //                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        //                glDrawArrays(GL_TRIANGLES, 0, 6);
        //            }
        //            //LEFT
        //            if (j - 1 < 0 || floor_coords[i][j - 1] == 0) {
        //                glm::mat4 model = glm::mat4(1.0f);
        //                model = glm::rotate(model, glm::radians(90.0f), glm::vec3(-1.0, 0.0, 0.0));
        //                model = glm::translate(model, glm::vec3(j - 1, i, 0));
        //                model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, -1.0, 0.0));
        //                model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
        //                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        //                glDrawArrays(GL_TRIANGLES, 0, 6);
        //            }

        //            //RIGHT
        //            if (j + 1 > 7 || floor_coords[i][j + 1] == 0) {
        //                glm::mat4 model = glm::mat4(1.0f);
        //                model = glm::rotate(model, glm::radians(90.0f), glm::vec3(-1.0, 0.0, 0.0));
        //                model = glm::translate(model, glm::vec3(j + 1, i, 0));
        //                model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
        //                model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, 0.0, -1.0));
        //                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        //                glDrawArrays(GL_TRIANGLES, 0, 6);
        //            }
        //        }
        //    }
        //}

        //generate sprite
        monster.draw();
        monster.draw();
    }
    
    glfwTerminate();
	return 0;

};

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
};

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        Cam.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        Cam.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        Cam.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        Cam.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    Cam.ProcessMouseMovement(xoffset, yoffset, true);
}