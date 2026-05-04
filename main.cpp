#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Camera_c.h"
#include "entity.h"
#include "mesh.h"
#include "resource_manager.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processMouseInput(GLFWwindow* window, Shader* shader, ResourceManager* manager, Camera* camera, std::vector<glm::vec3>* lightPositions);

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 400, lastY = 300;
bool firstMouse = true;
float fov = 45.0f;

glm::vec2 spriteFaceDirection = glm::vec2(0, 1);
float alpha = 0;
float tempAlpha = 0;
float prevCamDir; 
float prevAngle;
float angle;
float rotDirection = 1.0;

float moveInc = 0.01;
float maxMove = 10;
float minMove = -10;
float currentPosX = 0;
float currentPosY = 0;

Camera camera(glm::vec3(0.0f, 2.0f, 0.0f));

std::vector<float> cubeVertices = {
    // Position (3)  | Normal (3)     | Tex Coords (2)    
    // --- Face 1: -Z (Front) ---
    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   0.0f, 0.0f,

    // --- Face 2: +Z (Back) ---
    -0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,

    // --- Face 3: -X (Left) ---
    -0.5f,  0.5f,  0.5f,   -1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,   -1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,   -1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,   -1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,   -1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,   -1.0f, 0.0f, 0.0f,   1.0f, 0.0f,

    // --- Face 4: +X (Right) ---
     0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,

     // --- Face 5: -Y (Bottom) ---
     -0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,   0.0f, 1.0f,
      0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,   1.0f, 1.0f,
      0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,   1.0f, 0.0f,
      0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,   1.0f, 0.0f,
     -0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,   0.0f, 0.0f,
     -0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,   0.0f, 1.0f,

     // --- Face 6: +Y (Top) ---
     -0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
      0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
      0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
     -0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
     -0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f
};

std::vector<Entity> lasers = {};

std::vector<float> base_sprite_vertices = {
    // Position              // Normal            // Tex Coords (Base frame)
    -0.25f, -0.5f, 0.0f,     0.0f, 0.0f, -1.0f,   0.0f, 0.0f,
     0.25f, -0.5f, 0.0f,     0.0f, 0.0f, -1.0f,   1.0f, 0.0f,
     0.25f,  0.5f, 0.0f,     0.0f, 0.0f, -1.0f,   1.0f, 0.25f,
     0.25f,  0.5f, 0.0f,     0.0f, 0.0f, -1.0f,   1.0f, 0.25f,
    -0.25f,  0.5f, 0.0f,     0.0f, 0.0f, -1.0f,   0.0f, 0.25f,
    -0.25f, -0.5f, 0.0f,     0.0f, 0.0f, -1.0f,   0.0f, 0.0f
};

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

    Shader ourShader("./shader.vs", "./shader.fs");
    Shader lightObjectShader("./lightObjectShader.vs", "./lightObjectShader.fs");

    ResourceManager manager;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    std::vector<float> floor_vertices = {
		// positions        // Normals   // texture coords
        -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   0.0f, 0.0f,
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

	Entity floorEntity(manager.createMesh("floor", floor_vertices, (std::string) "./assets/metal_floor.png"), &ourShader, &camera);
	floorEntity.entityTexture = 0;

	Entity wallEntity(manager.createMesh("wall", floor_vertices, (std::string)"./assets/metal_wall.png"), &ourShader, &camera);
	wallEntity.entityTexture = 0;

    //ENEMY SPRITE
    float sprite_vertices[4][48] = {
        {
        -0.25f, -0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.75f,
         0.25f, -0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.75f,
         0.25f,  0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
         0.25f,  0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -0.25f,  0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        -0.25f, -0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.75f,
        },
        {
        -0.25f, -0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.50f,
         0.25f, -0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.50f,
         0.25f,  0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.75f,
         0.25f,  0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.75f,
        -0.25f,  0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.75f,
        -0.25f, -0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.50f,
        },
        {
        -0.25f, -0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.25f,
         0.25f, -0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.25f,
         0.25f,  0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.50f,
         0.25f,  0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.50f,
        -0.25f,  0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.50f,
        -0.25f, -0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.25f,
        },
        {
        -0.25f, -0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
         0.25f, -0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
         0.25f,  0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.25f,
         0.25f,  0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.25f,
        -0.25f,  0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.25f,
        -0.25f, -0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        }
    };



    Entity enemyEntity(manager.createMesh("enemy_sprite", base_sprite_vertices, "./assets/monster_spritesheet.png"), &ourShader, &camera);
    enemyEntity.useUVAnimation = true;
    enemyEntity.numFrames = 4;
    enemyEntity.uvFrameHeight = 0.25f;  // 1.0 / 4 frames
    enemyEntity.animationSpeed = 5.0f;
	enemyEntity.setPos(0, -0.1, 0);

    //Cam.MouseSensitivity = 0.1f;

    Entity lightCube(manager.createMesh("cube", cubeVertices, 36), &lightObjectShader, &camera, 0.0f, 0.6f, 0.0f);
	lightCube.scale(0.2f, 0.1f, 0.2);

    std::vector<glm::vec3> pointLightPositions = {
        glm::vec3(lightCube.x, lightCube.y, lightCube.z),
        glm::vec3(5.0f, 5.0f, 5.0f),
    };

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwSwapBuffers(window);
        glfwPollEvents();
        processInput(window);
		processMouseInput(window, &lightObjectShader, &manager, &camera, &pointLightPositions);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(window, mouse_callback);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();
        ourShader.setInt("material.diffuse", 0);
		ourShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
		ourShader.setFloat("material.shininess", 0.0f);
        //ourShader.setInt("texture2", 1);

        ourShader.setVec3("dirLight.ambient", 0.1f, 0.1f, 0.1f);
        ourShader.setVec3("dirLight.diffuse", 0.2f, 0.2f, 0.2f);
        ourShader.setVec3("dirLight.specular", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);

        for (int i = 0; i < pointLightPositions.size(); i++) {
			std::cout << "Number of point lights: " << pointLightPositions.size() << std::endl;
            std::stringstream ss;
            ss << "pointLights[" << i << "]";
            std::string currPointLight = ss.str();
			ourShader.setInt("numPointLights", (int)pointLightPositions.size());
            ourShader.setVec3(currPointLight + ".position", pointLightPositions[i]);
            ourShader.setFloat(currPointLight + ".constant", 1.0f);
            ourShader.setFloat(currPointLight + ".linear", 0.09f);
            ourShader.setFloat(currPointLight + ".quadratic", 0.032f);
            ourShader.setVec3(currPointLight + ".ambient", glm::vec3(0.05f));
            ourShader.setVec3(currPointLight + ".diffuse", glm::vec3(0.5f));
            ourShader.setVec3(currPointLight + ".specular", glm::vec3(0.5f));
        }

        for (int i = 0; i < lasers.size(); i++) {
            std::cout << "Number of lasers: " << lasers.size() << std::endl;
            std::stringstream ss;
            ss << "lasers[" << i << "]";
            std::string currLaser = ss.str();
			ourShader.setInt("numLasers", (int)lasers.size());
            ourShader.setVec3(currLaser + ".position", glm::vec3(lasers[i].x, lasers[i].y, lasers[i].z));
            ourShader.setFloat(currLaser + ".constant", 1.0f);
            ourShader.setFloat(currLaser + ".linear", 0.09f);
            ourShader.setFloat(currLaser + ".quadratic", 0.032f);
            ourShader.setVec3(currLaser + ".ambient", glm::vec3(0.01f, 0.0f, 0.0f));
            ourShader.setVec3(currLaser + ".diffuse", glm::vec3(0.1f, 0.0f, 0.0f));
            ourShader.setVec3(currLaser + ".specular", glm::vec3(0.0002f));
        }



        glm::mat4 projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);

        glm::mat4 view;
        view = camera.GetViewMatrix(); //lookAt(Pos, Pos + Front, Up)

        lightObjectShader.use();
        lightCube.Draw();

        ourShader.use();

        // Generate floor
        for (int i = 7; i >= 0; i--) {
            for (int j = 0; j < 8; j++) {
                if (floor_coords[i][j] == 1) {

                    // FLOOR
					floorEntity.rotate(90.0f, -1.0f, 0.0f, 0.0f);
					floorEntity.setPos(i, 0, j);
					floorEntity.Draw();

                    // WALL
                    // use floor coordinates to check where to place wall e.g. if floor coord = 1, check surronding coords, if surrounding is 0, place wall. if no surrounding i.e. out of coord space, place wall
                    // FRONT

                    if ( i + 1 > 7 || floor_coords[i + 1][j] == 0) {
					    wallEntity.rotate(90.0f, 0.0f, 1.0f, 0.0f);
					    wallEntity.setPos(i + 1, 0, j);
					    wallEntity.Draw();
                    }
                    // BACK
                    if (i - 1 < 0 || floor_coords[i - 1][j] == 0) {
					    wallEntity.rotate(90.0f, 0.0f, -1.0f, 0.0f);
					    wallEntity.setPos(i - 1, 0, j);
					    wallEntity.Draw();
                    }
                    // LEFT
                    if (j - 1 < 0 || floor_coords[i][j - 1] == 0) {
                        wallEntity.rotate(180.0f, 1.0f, 0.0f, 0.0f);
					    wallEntity.setPos(i, 0, j - 1);
					    wallEntity.Draw();
                    }

                    // RIGHT
                    if (j + 1 > 7 || floor_coords[i][j + 1] == 0) {
                        wallEntity.rotate(0.0f, -1.0f, 0.0f, 0.0f);
					    wallEntity.setPos(i, 0, j + 1);
					    wallEntity.Draw();
                    }
                }
            }
        }

        // Draw sprites 
        enemyEntity.Draw();

		// Draw lasers
        for (Entity& laser : lasers) {
            laser.Draw();
		}

    }
    
    glfwTerminate();
	return 0;

};

void shoot(Shader& shader, ResourceManager& manager, Camera& camera, std::vector<glm::vec3>& lightPositions) {
	std::cout << "Pew pew!" << std::endl;
	Entity laser(manager.createMesh("laser", cubeVertices, 36), &shader, &camera);
	laser.setPos(camera.Position.x, camera.Position.y - 0.1f, camera.Position.z);

	// Direction and velocity
	glm::vec3 dir = glm::normalize(camera.Front);
	laser.setVel(dir.x * 0.1f, dir.y * 0.1f, dir.z * 0.1f);

	// Orient the laser so its model forward (-Z) points along camera.Front
	glm::vec3 modelForward = glm::vec3(0.0f, 0.0f, -1.0f);
	float d = glm::clamp(glm::dot(modelForward, dir), -1.0f, 1.0f);
	float angle = glm::degrees(acos(d));
	glm::vec3 axis = glm::cross(modelForward, dir);

	if (glm::length(axis) < 1e-6f) {
		if (d < 0.0f) {
			// Opposite: rotate 180 degrees around up axis
			axis = glm::vec3(0.0f, 1.0f, 0.0f);
			angle = 180.0f;
		} else {
			// Same direction: no rotation needed
			axis = glm::vec3(1.0f, 0.0f, 0.0f);
			angle = 0.0f;
		}
	} else {
		axis = glm::normalize(axis);
	}

	laser.rotate(angle, axis.x, axis.y, axis.z);
	laser.scale(0.1f, 0.1f, 0.5f);
	laser.color = glm::vec3(1.0f, 0.0f, 0.0f);
    laser.id = lightPositions.size();
	lightPositions.push_back(glm::vec3(laser.x, laser.y, laser.z));
	lasers.push_back(laser);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
};

void processMouseInput(GLFWwindow* window, Shader* shader, ResourceManager* manager, Camera* camera, std::vector<glm::vec3>* lightPositions) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        shoot(*shader, *manager, *camera, *lightPositions);
    }
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

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
    camera.ProcessMouseMovement(xoffset, yoffset, true);
}