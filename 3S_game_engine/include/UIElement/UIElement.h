#pragma once

#include "Shader/Shader.h"
#include "Texture/Texture.h"

// Class responsible for 2D ui elements rendering
class UIElement
{
private:
	Shader uiShader;
	Texture uiTexture;
	unsigned int uiVAO, uiVBO, uiEBO; // Vertex array, vertex buffer and element buffer
	float uiVertices[20]; // table for vertices data
	int uiIndices[6]; // table for indices data
public:
	// Constructor with paths and border parameters
	UIElement(std::string vertexShaderPath, std::string fragmentShaderPath, std::string textureDirectory, std::string texturePath, float leftPos, float rightPos, float bottomPos, float topPos);
	~UIElement();
	// Render method draws the UI Element
	void render();
};