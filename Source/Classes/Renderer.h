#pragma once
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Classes/Model.h"
#include "Classes/Shader.h"
#include "Classes/Camera.h"
#include "Classes/ResourceManager.h"

struct Rendersettings
{
	float fov = 45.0f;
	bool bEnableResizeWindow = true;
	bool bEnableDepthTest = false;
	bool bEnableCullFace = false;
	bool bEnableMultisample = false;
	unsigned int Multisamples = 4;
};

struct RenderState
{
	unsigned int VAO;
	unsigned int IndicesSize;
};

class Renderer
{
public:
	// Screen size
	GLuint ScreenWidth;
	GLuint ScreenHeight;

	static Camera MainCamera;

	float detaTime;
	float lastFram;

	static float lastX, lastY;

	glm::mat4 Projection;

	unsigned int UniformBuffer;

	std::map<std::string, unsigned int> FrameBuffer;

	GLFWwindow* Window;

	std::vector<RenderState> RenderStates;

private:
	// Window title
	const char* Title;

	Rendersettings Settings;

	void BindVAO(Mesh* mesh, unsigned int index);

public:
	// Constructor
	Renderer(const char* title, GLuint screenWidth, GLuint screenHeight) : ScreenWidth(screenWidth), ScreenHeight(screenHeight), Title(title)
	{ 
		Window = nullptr;
		detaTime = 0.0f;
		lastFram = 0.0;
		lastX = screenWidth / 2.0f;
		lastY = screenHeight / 2.0f;
		UniformBuffer = 0;
	};

	void Setsettings(Rendersettings* settings);

	// Initialize render
	void Init();

	void PollEvents();

	void ClearScreen();

	// Create VAO list
	void SetRenderState();

	void SetLightState(DirLight* light);

	void DrawLightDepth(DirLight* light, ShadingModel shadingModel = ShadingModel::LightDepth);

	void Draw(Shader* shader, DirLight* shadowLight);

	// Glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	void Update();

	bool ShouldClose();

	// Close render
	void Close();

	// An Example
	void DrawTestGrid(Shader* shader);

	// Debug light depth map
	void DebugLightDepthMap(Shader* debugShader, DirLight* light);

private:
	// Glad: load all OpenGL function pointers
	bool LoadGlad();

	// Set Uniform Buffer
	void SetUniformParameters();

	// Bind Uniform Buffer to shaders
	void BindShaderUniform();

	// Update some Uniform Parameters
	void UpdateUniformParameters(Shader* shader, DirLight* shadowLight);

	// Input
	void ProcessInput();

public:
	// Mouse control
	static void Mouse_Callback(GLFWwindow* window, double xpos, double ypos);
};