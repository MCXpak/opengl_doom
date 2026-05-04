#include <iostream>
#include <map>
#include <string>
#include <memory>
#include <set>
#include "mesh.h"

class ResourceManager
{
public:

	std::map<std::string, std::shared_ptr<Mesh>> meshCache{};

	ResourceManager()
	{

	};


	std::shared_ptr<Mesh> getMesh(const std::string& name)
	{
		if (meshCache.find(name) != meshCache.end()) {
			return meshCache[name];
		};

		return NULL;
	};

	std::shared_ptr<Mesh> createMesh(const std::string& name, const std::vector<float> vertexData, const int vertexCount)
	{
		if (getMesh(name) != NULL) {
			return meshCache[name];
		}

		std::cout << "Creating " << name << " mesh with " << vertexCount << " vertex count." << '\n';
		meshCache[name] = std::make_shared<Mesh>(vertexData, vertexCount);
		return meshCache[name];

	};

	std::shared_ptr<Mesh> createMesh(const std::string& name, const std::vector<float> vertexData, std::string texture_file_path)
	{
		if (getMesh(name) != NULL) {
			return meshCache[name];
		}

		std::cout << "Creating " << name << " mesh..." << '\n';
		meshCache[name] = std::make_shared<Mesh>(vertexData, texture_file_path);
		return meshCache[name];

	};
	std::shared_ptr<Mesh> createMesh2D(const std::string& name, const std::vector<float> vertexData, std::string texture_file_path)
	{
		if (getMesh(name) != NULL) {
			return meshCache[name];
		}

		std::cout << "Creating " << name << " mesh..." << '\n';
		meshCache[name] = std::make_shared<Mesh>(vertexData, texture_file_path, true);
		return meshCache[name];

	};

	std::shared_ptr<Mesh> createMesh(const std::string& name, const std::vector<float> vertexData, const std::vector<glm::vec2> offsetData)
	{
		if (getMesh(name) != NULL) {
			return meshCache[name];
		}

		std::cout << "Creating " << name << " mesh..." << '\n';
		meshCache[name] = std::make_shared<Mesh>(vertexData, offsetData);
		std::cout << "Created " << name << " mesh." << '\n';
		return meshCache[name];

	};
};