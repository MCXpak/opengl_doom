#ifndef ENTITY_H
#define ENTITY_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader_s.h"
#include "Camera_c.h"
//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"

class Entity
{
private:
	glm::vec2 spriteFaceDirection = glm::vec2(0, 1);

public:
	int id = 0;
	float rotAngle = 0;
	float x = 0;
	float y = 0;
	float z = 0;
	float rotation = 0;
	glm::vec3 rotationAxisVec;
	float velX = 0;
	float velY = 0;
	float accel = 0;
	std::string name;
	GLuint entityVAO;
	GLuint entityVBO;
	float entityVertices[4][30];
	unsigned int entityTexture;
	const char* textureLocation;
	Camera* camera;
	float camX = 0;
	float camY = 0;
	unsigned int uniformModelLoc;
	unsigned int uniformColorLoc;
	unsigned int uniformTextureModeLoc;
	int uniformTextureMode = 0;
	float* deltaTime;
	bool billboard = false;
	glm::mat4 model = glm::mat4(1.0f);

	float vertices[30] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	};

	Entity(Camera* cam, Shader* shader, float* dTime, int identifier)
	{
		camera = cam;
		deltaTime = dTime;
		id = identifier;

		uniformModelLoc = glGetUniformLocation((*shader).ID, "model");
		uniformColorLoc = glGetUniformLocation((*shader).ID, "ourColor");
		uniformTextureModeLoc = glGetUniformLocation((*shader).ID, "textureMode");

		glUniform1i(uniformTextureModeLoc, uniformTextureMode);

		glGenVertexArrays(1, &entityVAO);
		glGenBuffers(1, &entityVBO);

		glBindVertexArray(entityVAO);
		glBindBuffer(GL_ARRAY_BUFFER, entityVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// texture attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		//glGenTextures(1, &entityTexture);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, entityTexture);
	
	};

	void draw()
	{

		glBindVertexArray(entityVAO);

		//Check if there is a texture, if not, send a solid color
		if (uniformTextureMode) {
			glUniform1i(uniformTextureModeLoc, uniformTextureMode);
		}
		else {
			glUniform1i(uniformTextureModeLoc, uniformTextureMode);
			glUniform4f(uniformColorLoc, 0.5f, 0.0f, 0.3f, 1.0f);

		}

		glBindTexture(GL_TEXTURE_2D, entityTexture);

		//int entityFrame = (int)(glfwGetTime() * 2) % 3;

		camX = (*camera).Position[0];
		camY = (*camera).Position[2];

		//v = u + at  - Kinematics
		velX = velX + accel * (*deltaTime);
		velY = velY + accel * glfwGetTime();

		// s = ut
		x = x + velX * (*deltaTime);
		y = y + velX * (*deltaTime);

		if (billboard) {
			glm::vec2 camYZ = glm::vec2(camX, camY);
			glm::vec2 camYZminusCurrentPos = glm::vec2(camYZ[0] - x, camYZ[1] - y);
			std::cout << camYZminusCurrentPos[0] << ", " << camYZminusCurrentPos[1] << std::endl;
			rotAngle = glm::acos(glm::dot(spriteFaceDirection, camYZminusCurrentPos) / (glm::length(spriteFaceDirection) * glm::length(camYZminusCurrentPos)));
			if (camYZminusCurrentPos[0] < 0) {
				rotAngle = -rotAngle;
			}
			model = glm::rotate(model, rotAngle, glm::vec3(0.0, 1.0, 0.0));
		}
		//else {
		//	//model = glm::rotate(model, glm::radians(rotation), rotationAxisVec);
		//}
		
		//model = glm::translate(model, glm::vec3(x, y, z));
	
		glUniformMatrix4fv(uniformModelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
	};

	void move(float moveX, float moveY, float moveZ) {
		model = glm::translate(model, glm::vec3(moveX, moveY, moveZ));
		//moves to 
		x = moveX;
		y = moveY;
		z = moveZ;
	}

	void rotate(float angle, float x, float y, float z) {
		model = glm::rotate(model, glm::radians(angle), glm::vec3(x, y, z));
		//rotation = angle;
		//rotationAxisVec = glm::vec3(x, y, z);
	}

	void setVelocity(float velocityX, float velocityY) {
		velX = velocityX;
		velY = velocityY;
	}

	void setAcceleration(float acceleration) {
		accel = acceleration;
	}

	void navMap(int floorCoords[8][8]) {
		//FORWARD

		//TRY LEFT
			
	}

	void addTexture(const char textureLoc[]) {

		uniformTextureMode = 1;

		glGenTextures(1, &entityTexture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, entityTexture);

		// set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int width, height, nrChannels;
		unsigned char* data = stbi_load(textureLoc, &width, &height, &nrChannels, 0);
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

	}

	void addSpritesheet() {
		/*for (int i = 0; i < 4; i++) {
			glBindVertexArray(entityVAOs[i]);
			glBindBuffer(GL_ARRAY_BUFFER, entityVBOs[i]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(entityVertices[i]), entityVertices[i], GL_STATIC_DRAW);
			 position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			 texture attribute
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);
		}*/
	}
};

#endif