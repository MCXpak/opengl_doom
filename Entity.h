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
	float rotAngle = 0;
	float x = 0;
	float y = 0;
	int numOfFrames = 4;
	std::string name;
	GLuint entityVAOs[4];
	GLuint entityVBOs[4];
	float entityVertices[4][30];
	unsigned int entityTexture;
	const char* textureLocation;
	Camera* camera;
	float camX = 0;
	float camY = 0;
	unsigned int uniformModelLoc;

	Entity(float entityVertices[4][30], Camera* cam, Shader* shader)
	{
		camera = cam;

		uniformModelLoc = glGetUniformLocation((*shader).ID, "model");

		glGenVertexArrays(numOfFrames, entityVAOs);
		glGenBuffers(numOfFrames, entityVBOs);

		for (int i = 0; i < 4; i++) {
			glBindVertexArray(entityVAOs[i]);
			glBindBuffer(GL_ARRAY_BUFFER, entityVBOs[i]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(entityVertices[i]), entityVertices[i], GL_STATIC_DRAW);
			// position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			// texture attribute
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);
		}

		glGenTextures(1, &entityTexture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, entityTexture);

		// set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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
	};

	void draw()
	{
		glBindTexture(GL_TEXTURE_2D, entityTexture);
		int entityFrame = (int)(glfwGetTime() * 2) % 3;
		glBindVertexArray(entityVAOs[entityFrame]);
		glm::mat4 model = glm::mat4(1.0f);

		camX = (*camera).Position[0];
		camY = (*camera).Position[2];

		glm::vec2 camYZ = glm::vec2(camX, camY);
		glm::vec2 camYZminusCurrentPos = glm::vec2(camYZ[0] - x, camYZ[1] - y);
		std::cout << camYZminusCurrentPos[0] << ", " << camYZminusCurrentPos[1] << std::endl;
		x = 2 * sin(glfwGetTime());
		y = 2 * sin(glfwGetTime());

		model = glm::translate(model, glm::vec3(x, 0.0, y));
		rotAngle = glm::acos(glm::dot(spriteFaceDirection, camYZminusCurrentPos) / (glm::length(spriteFaceDirection) * glm::length(camYZminusCurrentPos)));
		if (camYZminusCurrentPos[0] < 0) {
			rotAngle = -rotAngle;
		}
		float testAngle = 50 * 3.1415 / 180;
		model = glm::rotate(model, rotAngle, glm::vec3(0.0, 1.0, 0.0));

		glUniformMatrix4fv(uniformModelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 6);

	};

	void move(float moveX, float moveY) {

	}
};

#endif