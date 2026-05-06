#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "Shader.h"
#include "camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include "mesh.h"

class Entity
{
public:
	int id = rand() % 10000;
	int type = 1;
	float x = 0;
	float y = 0;
	float z = 0;
	int gridPos[3] = { 0, 0, 0 };
	float sizeX = 1;
	float sizeY = 1;
	float sizeZ = 1;
	float minX = x - sizeX;
	float maxX = x + sizeX;
	float minY = y - sizeY;
	float maxY = y + sizeY;
	float minZ = z - sizeZ;
	float maxZ = z + sizeZ;
	float boundBoxPaddingX = 0.0f;
	float boundBoxPaddingY = 0.0f;
	float boundBoxPaddingZ = 0.0f;
	float velX = 0;
	float velY = 0;
	float velZ = 0;
	float angle = 0;
	float rotX = 0;
	float rotY = 1.0f;
	float rotZ = 0;
	int entityTexture = -1;
	int instances = 1;
	std::vector<glm::vec2> instanceOffsets;
	std::vector<glm::vec2> instanceVels;
	bool isCube = true;
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	float colorAlpha = 1.0f;
	std::shared_ptr<Mesh> mesh;
	Shader* shader;
	Camera* camera;
	std::vector<std::shared_ptr<Mesh>> animationFrames;
	float animationSpeed = 5.0f; // Frames per second
	bool useUVAnimation = false;
	int numFrames = 1;
	float uvFrameHeight = 1.0f; // Height of one frame in UV space (e.g., 0.25 for 4 frames)
	glm::vec2 uvOffset = glm::vec2(0.0f);
	int health = 4;
	bool isEnemy = false;


	Entity(std::shared_ptr<Mesh> meshP, Shader* s, Camera* c, float xCoord = 0, float yCoord = 0, float zCoord = 0)
	{
		mesh = meshP;
		shader = s;
		camera = c;
		x = xCoord;
		y = yCoord;
		z = zCoord;

		updateBoundingBox();
	};

	Entity(std::shared_ptr<Mesh> meshP, Shader* s, Camera* c, float xCoord, float yCoord, float zCoord, int ints)
	{
		mesh = meshP;
		shader = s;
		camera = c;
		x = xCoord;
		y = yCoord;
		z = zCoord;
		instances = ints;
	};

	void setPos(float xCoord, float yCoord, float zCoord)
	{
		x = xCoord;
		y = yCoord;
		z = zCoord;
		updateBoundingBox();
	}

	void update() {
		//Update entity with it's velocity properties
		translate(velX, velY, velZ);
	}

	void translate(float xCoord, float yCoord, float zCoord)
	{
		x = x + xCoord;
		y = y + yCoord;
		z = z + zCoord;

		updateBoundingBox();
	}

	void scale(float xScale, float yScale, float zScale)
	{
		sizeX *= xScale;
		sizeY *= yScale;
		sizeZ *= zScale;

		updateBoundingBox();
	}

	void scale(float scale)
	{
		sizeX *= scale;
		sizeY *= scale;
		sizeZ *= scale;

		updateBoundingBox();
	}

	void setVel(float xVel, float yVel, float zVel)
	{
		velX = xVel;
		velY = yVel;
		velZ = zVel;
	}

	void setVel(float vel)
	{
		velX = vel;
		velY = vel;
		velZ = vel;
	}

	void rotate(float ang, float xAxis, float yAxis, float zAxis)
	{
		angle = ang;
		rotX = xAxis;
		rotY = yAxis;
		rotZ = zAxis;
	}

	void updateBoundingBox()
	{
		minX = x - sizeX/2 - boundBoxPaddingX;
		maxX = x + sizeX/2 + boundBoxPaddingX;
		minY = y - sizeY/2 - boundBoxPaddingY;
		maxY = y + sizeY/2 + boundBoxPaddingY;
		minZ = z - sizeZ/2 - boundBoxPaddingZ;
		maxZ = z + sizeZ/2 + boundBoxPaddingZ;
	}

	void moveHorizontal(float x_movement) {
		translate(x_movement, 0, 0);
	}

	void Draw() {
		update();
		shader->use();

		glm::mat4 view = camera->GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

		// Animation
		if (useUVAnimation) {
			int frameIndex = (int)(glfwGetTime() * animationSpeed) % numFrames;
			uvOffset.y = (numFrames - 1 - frameIndex) * uvFrameHeight;
		}
		else {
			uvOffset = glm::vec2(0.0f, 0.0f);
		}
		shader->setVec2("uvOffset", uvOffset);
		
		shader->setMat4("view", view);
		shader->setMat4("projection", projection);
		shader->setVec3("viewPos", camera->Position);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(x, y, z));

		if (isEnemy) {
			// Compute camera axes in world space by inverting the view matrix.
			// Use those axes to construct a rotation matrix that aligns the quad to face the camera.
			glm::mat4 invView = glm::inverse(view);
			glm::vec3 camRight = glm::normalize(glm::vec3(invView[0]));   // first column
			glm::vec3 camUp = glm::normalize(glm::vec3(invView[1]));      // second column
			glm::vec3 camForward = glm::normalize(glm::vec3(invView[2])); // third column

			// Build rotation matrix from camera axes and apply scaling along each axis.
			glm::mat4 billboard = glm::mat4(1.0f);
			// Columns are the world-space axes for the quad; scale them to apply size.
			billboard[0] = glm::vec4(camRight * sizeX, 0.0f);
			billboard[1] = glm::vec4(camUp * sizeY, 0.0f);
			// Use -camForward so the quad faces the camera (normal points toward camera).
			billboard[2] = glm::vec4(-camForward * sizeZ, 0.0f);

			model *= billboard;
		}
		else {
			if (angle != 0.0f) {
				model = glm::rotate(model, glm::radians(angle), glm::vec3(rotX, rotY, rotZ));
			}
			model = glm::scale(model, glm::vec3(sizeX, sizeY, sizeZ));
		}

		shader->setMat4("model", model);

		shader->setVec3("objectColor", color);
		shader->setBool("isCube", isCube);

		glBindVertexArray(mesh->vaoId);
		if (mesh->textureId != 0) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mesh->textureId);
		}
		glDrawArrays(GL_TRIANGLES, 0, mesh->vertexCount);
	
		glBindVertexArray(0);
	}

	void Draw2D() {
		update();
		shader->use();
		glDisable(GL_DEPTH_TEST);

		// Ortho for 2D rendering
		glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

		shader->setVec2("uvOffset", uvOffset);

		shader->setMat4("projection", projection);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(x, y, z));
		if (angle != 0.0f) {
			model = glm::rotate(model, glm::radians(angle), glm::vec3(rotX, rotY, rotZ));
		}
		model = glm::scale(model, glm::vec3(sizeX, sizeY, sizeZ));
		shader->setMat4("model", model);

		//shader->setVec3("objectColor", color);
		//shader->setBool("isCube", isCube);

		glBindVertexArray(mesh->vaoId);
		if (mesh->textureId != 0) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mesh->textureId);
		}
		glDrawArrays(GL_TRIANGLES, 0, mesh->vertexCount);

		glBindVertexArray(0);
		glEnable(GL_DEPTH_TEST);
	}

	void DrawInstanced() {
		shader->use();
		glm::mat4 view = camera->GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);
		
		updateParticleVelocities();
		updateInstanceOffsets();
		shader->setMat4("view", view);
		shader->setMat4("projection", projection);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(x, y, z));

		glm::mat4 invView = glm::inverse(view);
		glm::vec3 camRight = glm::normalize(glm::vec3(invView[0]));   // first column
		glm::vec3 camUp = glm::normalize(glm::vec3(invView[1]));      // second column
		glm::vec3 camForward = glm::normalize(glm::vec3(invView[2])); // third column

		// Build rotation matrix from camera axes and apply scaling along each axis.
		glm::mat4 billboard = glm::mat4(1.0f);
		// Columns are the world-space axes for the quad; scale them to apply size.
		billboard[0] = glm::vec4(camRight * sizeX, 0.0f);
		billboard[1] = glm::vec4(camUp * sizeY, 0.0f);
		// Use -camForward so the quad faces the camera (normal points toward camera).
		billboard[2] = glm::vec4(-camForward * sizeZ, 0.0f);

		model *= billboard;

		//model = glm::scale(model, glm::vec3(sizeX, sizeY, sizeZ));
		shader->setMat4("model", model);
		shader->setVec3("objectColor", color);
		shader->setFloat("colorAlpha", colorAlpha);
		glBindVertexArray(mesh->vaoId);
		glDrawArraysInstanced(GL_TRIANGLES, 0, mesh->vertexCount, instances);
		glBindVertexArray(0);
	}

	void updateInstanceOffsets() {
		mesh->updateInstanceOffsets(instanceOffsets);
	}

	void updateParticleVelocities() {
		for (int i = 0; i < static_cast<int>(instanceOffsets.size()); i++) {
			instanceOffsets[i] += instanceVels[i];
		}
	}
	
};