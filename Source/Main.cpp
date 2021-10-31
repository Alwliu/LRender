#include <iostream>
#include "Classes/Renderer.h"
#include "Classes/Camera.h"
#include "Classes/ResourceManager.h"

int main()
{
	Renderer* LRender = new Renderer("LRender", 800, 600);

	// Set render
	Rendersettings* renderSettings = new Rendersettings();
	renderSettings->bEnableResizeWindow = false;
	renderSettings->bEnableDepthTest = true;
	renderSettings->bEnableCullFace = true;
	LRender->Setsettings(renderSettings);
	delete renderSettings;

	LRender->Init();

	const char* Grid = "D:/C++App/LRender/Resource/testPlane.fbx";
	const char* fbxFile = "D:/C++App/LRender/Resource/testCube.fbx";
	const char* fbxFile2 = "D:/C++App/LRender/Resource/testSphere.fbx";

	ResourceManager::loadShaderFromFile("D:/C++App/LRender/Shaders/LightDepth.vert", "D:/C++App/LRender/Shaders/LightDepth.frag", NULL);
	ResourceManager::loadShaderFromFile("D:/C++App/LRender/Shaders/DebugLightDepth.vert", "D:/C++App/LRender/Shaders/DebugLightDepth.frag", NULL);

	ResourceManager::loadShaderFromFile("D:/C++App/LRender/Shaders/Unlit.vert", "D:/C++App/LRender/Shaders/Unlit.frag", NULL);
	ResourceManager::loadShaderFromFile("D:/C++App/LRender/Shaders/BlinnPhong.vert", "D:/C++App/LRender/Shaders/BlinnPhong.frag", NULL);

	ResourceManager::loadModelFromFile(Grid);
	ResourceManager::loadModelFromFile(fbxFile);
	ResourceManager::loadModelFromFile(fbxFile2);

	ResourceManager::CreateDirectionalLight("DirecLight");
	ResourceManager::Lighters["DirecLight"].Direction = glm::normalize(glm::vec3(2.0f, -4.0f, 1.0f));
	ResourceManager::Lighters["DirecLight"].Position = glm::vec3(-2.0f, 4.0f, -1.0f);

	LRender->SetRenderState();
	LRender->SetLightState(&ResourceManager::Lighters["DirecLight"]);

	while (!LRender->ShouldClose())
	{
		LRender->PollEvents();
		
		LRender->ClearScreen();

		LRender->DrawLightDepth(&ResourceManager::Lighters["DirecLight"], &ResourceManager::Shaders["LightDepth"]);

		LRender->Draw(&ResourceManager::Shaders["BlinnPhong"], &ResourceManager::Lighters["DirecLight"]);

		//LRender->DebugLightDepthMap(&ResourceManager::Shaders["DebugLightDepth"], &ResourceManager::Lighters["DirecLight"]);

		LRender->Update();
	}

	LRender->Close();
	ResourceManager::clear();
	delete LRender;
	return 0;
}