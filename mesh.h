#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include "camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <memory>

class Mesh
{
public:
	GLuint vaoId;
	GLuint vboId;
	GLuint eboId;
	GLuint instanceVBO;

	Mesh(std::vector<float> vertexData, std::vector<int> indexData, const int vCount)
	{
		glGenVertexArrays(1, &vaoId);
		glGenBuffers(1, &vboId);
		glGenBuffers(1, &eboId);

		glBindVertexArray(vaoId);

		glBindBuffer(GL_ARRAY_BUFFER, vboId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexData.size(), &vertexData[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indexData.size(), &indexData[0], GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// Normal attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// texture attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		
	}
	Mesh(std::vector<float> vertexData, std::vector<glm::vec2> offsetData) 
	{
		glGenVertexArrays(1, &vaoId);
		glGenBuffers(1, &vboId);
		//glGenBuffers(1, &eboId);
		glGenBuffers(1, &instanceVBO);

		glBindVertexArray(vaoId);

		glBindBuffer(GL_ARRAY_BUFFER, vboId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexData.size(), &vertexData[0], GL_STATIC_DRAW);

		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboId);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indexData.size(), &indexData[0], GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// Colors attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);
		
		// instance offset attribute
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * offsetData.size(), &offsetData[0], GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glVertexAttribDivisor(2, 1);

	}

	void updateInstanceOffsets(const std::vector<glm::vec2>& newOffsets) {
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec2) * newOffsets.size(), &newOffsets[0]);
	}

};