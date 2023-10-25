#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
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
void checkCollision(Entity &a, Entity &b);

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
    
    int width, height, nrChannels;
    unsigned char* data = stbi_load("./assets/monster_spritesheet.png", &width, &height, &nrChannels, 0);
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
    unsigned int uniformColorLoc = glGetUniformLocation(ourShader.ID, "ourColor");
    unsigned int uniformTextureBoolLoc = glGetUniformLocation(ourShader.ID, "textureBool");

    Cam.MouseSensitivity = 0.1f;

    std::vector<Entity> floorArray;
    std::vector<Entity> wallArray;

    int wallArrayCount = 0;
    int floorArrayCount = 0;

    //generate floor
    /*for (int i = 7; i >= 0; i--) {
        for (int j = 0; j < 8; j++) {
            if (floor_coords[i][j] == 1) {
                floorArray.push_back(Entity(&Cam, &ourShader, &deltaTime, (int)glfwGetTime()));
                floorArray[floorArrayCount].addTexture("./assets/metal_floor.png");
                floorArray[floorArrayCount].rotate(90, -1, 0, 0);
                floorArray[floorArrayCount].move(j, i, 0);
                floorArrayCount += 1;

                //FRONT
                if (i + 1 > 7 || floor_coords[i + 1][j] == 0) {
                    wallArray.push_back(Entity(&Cam, &ourShader, &deltaTime, (int)glfwGetTime()));
                    wallArray[wallArrayCount].addTexture("./assets/metal_wall.png");
                    wallArray[wallArrayCount].rotate(90, -1, 0, 0);
                    wallArray[wallArrayCount].move(j, i + 1, 0);
                    wallArray[wallArrayCount].rotate(90, -1, 0, 0);
                    wallArrayCount += 1;
                }
                //BACK
                if (i - 1 < 0 || floor_coords[i - 1][j] == 0) {
                    wallArray.push_back(Entity(&Cam, &ourShader, &deltaTime, (int)glfwGetTime()));
                    wallArray[wallArrayCount].addTexture("./assets/metal_wall.png");
                    wallArray[wallArrayCount].rotate(90, -1, 0, 0);
                    wallArray[wallArrayCount].move(j, i - 1, 0);
                    wallArray[wallArrayCount].rotate(90, 1, 0, 0);
                    wallArrayCount += 1;
                }
                //LEFT
                if (j - 1 < 0 || floor_coords[i][j - 1] == 0) {
                    wallArray.push_back(Entity(&Cam, &ourShader, &deltaTime, (int)glfwGetTime()));
                    wallArray[wallArrayCount].addTexture("./assets/metal_wall.png");
                    wallArray[wallArrayCount].rotate(90, -1, 0, 0);
                    wallArray[wallArrayCount].move(j - 1, i, 0);
                    wallArray[wallArrayCount].rotate(90, 0, -1, 0);
                    wallArray[wallArrayCount].rotate(90, 0, 0, 1);
                    wallArrayCount += 1;
                }

                //RIGHT
                if (j + 1 > 7 || floor_coords[i][j + 1] == 0) {
                    wallArray.push_back(Entity(&Cam, &ourShader, &deltaTime, (int)glfwGetTime()));
                    wallArray[wallArrayCount].addTexture("./assets/metal_wall.png");
                    wallArray[wallArrayCount].rotate(90, -1, 0, 0);
                    wallArray[wallArrayCount].move(j, i, 0);
                    wallArray[wallArrayCount].rotate(90, 0, -1, 0);
                    wallArray[wallArrayCount].rotate(90, 0, 0, -1);
                    wallArrayCount += 1;
                }
            }
        }
    }*/

    Entity testWall(&Cam, &ourShader, &deltaTime, (int)glfwGetTime());
    testWall.addTexture("./assets/metal_wall.png");
    testWall.move(0.5, 0, 0);

    Entity testWall1(&Cam, &ourShader, &deltaTime, (int)glfwGetTime());
    testWall1.addTexture("./assets/metal_wall.png");
    testWall1.move(2, 0, 0);

    testWall.setVelocity(0.7, -0.3, 0.0);
    testWall1.setVelocity(-0.1, 0.3, 0);
    testWall1.mass = 5;

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
        
        /*for (int i = 0; i < floorArray.size(); i++) {
            floorArray[i].draw();
        }

        for (int i = 0; i < wallArray.size(); i++) {
            wallArray[i].draw();
        }*/

        //testFloor.draw();
        //glActiveTexture(0);

        //generate sprite
        
        //monster.move(2 * sin(glfwGetTime()) , 2 * sin(glfwGetTime()));
        //monster.setVelocity(1,0);

        testWall.draw();
        testWall1.draw();
        

        checkCollision(testWall, testWall1);
    }
    
    glfwTerminate();
	return 0;

};

void checkCollision(Entity &a, Entity &b) {

    float leftA = a.x - a.sizeX / 2;
    float rightA = a.x + a.sizeX / 2;
    float topA = a.y - a.sizeY / 2;
    float bottomA = a.y + a.sizeY / 2;
    float frontA = a.z - a.sizeZ / 2;
    float backA = a.z + a.sizeZ / 2;

    float leftB = b.x - b.sizeX / 2;
    float rightB = b.x + b.sizeX / 2;
    float topB = b.y - b.sizeY / 2;
    float bottomB = b.y + b.sizeY / 2;
    float frontB = b.z - b.sizeZ / 2;
    float backB = b.z + b.sizeZ / 2;

    if (rightA >= leftB && leftA <= rightB &&
        bottomA >= topB && topA <= bottomB &&
        backA >= frontB && frontA <= backB) {
        std::cout << "Collided!" << std::endl;

        // Coefficient of restitution
        float e = 1.0f;

        // Calculate relative velocities in all three dimensions
        float VrelX = b.velX - a.velX;
        float VrelY = b.velY - a.velY;
        float VrelZ = b.velZ - a.velZ;

        std::cout << "VrelX: " << VrelX << ", VrelY:" << VrelY << std::endl;

        // Calculate the magnitude of the relative velocity vector
        float VrelMagnitude = std::sqrt(VrelX * VrelX + VrelY * VrelY + VrelZ * VrelZ);

        if (VrelMagnitude > 0.0f) {
            // Calculate the impulse magnitude
            float J = -(1.0f + e) * VrelMagnitude / (1.0f / a.mass + 1.0f / b.mass);

            // Calculate the normalized collision direction
            float collisionDirectionX = VrelX / VrelMagnitude;
            float collisionDirectionY = VrelY / VrelMagnitude;
            float collisionDirectionZ = VrelZ / VrelMagnitude;

            std::cout << "collisionDirectionX: " << collisionDirectionX << ", collisionDirectionY:" << collisionDirectionY << std::endl;

            // Apply the impulse along the collision direction
            a.velX -= J * collisionDirectionX / a.mass;
            a.velY -= J * collisionDirectionY / a.mass;
            a.velZ -= J * collisionDirectionZ / a.mass;

            b.velX += J * collisionDirectionX / b.mass;
            b.velY += J * collisionDirectionY / b.mass;
            b.velZ += J * collisionDirectionZ / b.mass;
        }

        std::cout << "VelX: " << a.velX << ", VelY: " << a.velY << ", VelZ: " << a.velZ << std::endl;
    }



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