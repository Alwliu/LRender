#include "Renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

Camera Renderer::MainCamera;
float Renderer::lastX;
float Renderer::lastY;

void Renderer::BindVAO(Mesh* mesh, unsigned int index)
{
	unsigned int vbo, ebo;
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(this->RenderStates[index].VAO);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh->Vertices.size() * sizeof(Vertex), &mesh->Vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->Indices.size() * sizeof(unsigned int), &mesh->Indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TextureCoord));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Renderer::Setsettings(Rendersettings* settings)
{
	this->Settings = *settings;
}

void Renderer::Init()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (!this->Settings.bEnableResizeWindow)
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	glfwWindowHint(GLFW_SAMPLES, this->Settings.Multisamples);

	this->Window = glfwCreateWindow(this->ScreenWidth, this->ScreenHeight, Title, NULL, NULL);
	glfwMakeContextCurrent(this->Window);

	glfwSetCursorPosCallback(this->Window, &Renderer::Mouse_Callback);

	LoadGlad();

	if (this->Settings.bEnableDepthTest)
		glEnable(GL_DEPTH_TEST);
	if (this->Settings.bEnableCullFace)
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	if (this->Settings.bEnableMultisample)
		glEnable(GL_MULTISAMPLE);

	this->Projection = glm::perspective(glm::radians(this->Settings.fov), (float)this->ScreenWidth / (float)this->ScreenHeight, 0.1f, 100.0f);
	this->MainCamera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));

	SetUniformParameters();
	BindShaderUniform();
}

bool Renderer::LoadGlad()
{
	return gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
}

void Renderer::PollEvents()
{
	glfwPollEvents();
	ProcessInput();
}

void Renderer::ClearScreen()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	if (this->Settings.bEnableDepthTest)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	else
		glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::SetRenderState()
{
	this->RenderStates.resize(ResourceManager::MeshCount);
	std::vector<unsigned> vaos;
	vaos.resize(ResourceManager::MeshCount);
	glGenVertexArrays(this->RenderStates.size(), &vaos[0]);

	unsigned int Index = 0;
	for (auto iter : ResourceManager::Models)
	{
		for (auto mesh : iter.second.Meshes)
		{
			RenderStates[Index].VAO = vaos[Index];
			RenderStates[Index].IndicesSize = mesh.Indices.size();
			BindVAO(&mesh, Index);
			Index++;
		}
	}
}

void Renderer::SetLightState(DirLight* light)
{
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	Texture2D depthMap(2048, 2048);
	depthMap.Internal_Format = GL_DEPTH_COMPONENT;
	depthMap.Image_Format = GL_DEPTH_COMPONENT;
	glGenTextures(1, &depthMap.ID);
	depthMap.Bind();
	glTexImage2D(GL_TEXTURE_2D, 0, depthMap.Internal_Format, depthMap.Width, depthMap.Height, 0, depthMap.Image_Format, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap.ID, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	ResourceManager::Textures[light->name] = depthMap;
	this->FrameBuffer[light->name] = depthMapFBO;
}

void Renderer::DrawLightDepth(DirLight* light, ShadingModel shadingModel)
{
	Shader* shader = nullptr;

	if (shadingModel == ShadingModel::LightDepth)
		shader = &ResourceManager::Shaders["LightDepth"];

	if (shader)
	{
		Texture2D& depthMap = ResourceManager::Textures[light->name];
		unsigned int depthMapFBO = this->FrameBuffer[light->name];

		glViewport(0, 0, depthMap.Width, depthMap.Height);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		shader->Use();
		shader->SetMat4("LightSpaceMatrix", light->GetLightSpaceMatrix());

		for (auto iter : this->RenderStates)
		{
			glBindVertexArray(iter.VAO);
			glDrawElements(GL_TRIANGLES, iter.IndicesSize, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, this->ScreenWidth, this->ScreenHeight);
	}
}

void Renderer::SetUniformParameters()
{
	glGenBuffers(1, &this->UniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, this->UniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(GL_UNIFORM_BUFFER, 0, this->UniformBuffer, 0, 2 * sizeof(glm::mat4));

	glBindBuffer(GL_UNIFORM_BUFFER, this->UniformBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &this->Projection[0][0]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Renderer::BindShaderUniform()
{
	for (auto iter : ResourceManager::Shaders)
	{
		unsigned int uniformBlockID = glGetUniformBlockIndex(iter.second.ID, "Matrices");
		glUniformBlockBinding(iter.second.ID, uniformBlockID, 0);
	}
}

void Renderer::UpdateUniformParameters(Shader* shader, DirLight* shadowLight)
{
	shader->Use();
	glBindBuffer(GL_UNIFORM_BUFFER, this->UniformBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &this->MainCamera.GetViewMatrix()[0][0]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	shader->setVec4("LightColor", ResourceManager::Lighters["DirecLight"].Color);
	shader->setVec3("LightDirection", -ResourceManager::Lighters["DirecLight"].Direction);
	shader->SetMat4("LightSpaceMatrix", shadowLight->GetLightSpaceMatrix());
	shader->setVec3("ViewPos", this->MainCamera.Position);
	shader->setInt("ShadowMap", 0);
	
	glActiveTexture(GL_TEXTURE0);
	ResourceManager::Textures[shadowLight->name].Bind();
}

void Renderer::Draw(Shader* shader, DirLight* shadowLight)
{
	UpdateUniformParameters(shader, shadowLight);

	for (auto iter : this->RenderStates)
	{
		glBindVertexArray(iter.VAO);
		glDrawElements(GL_TRIANGLES, iter.IndicesSize, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}

void Renderer::Update()
{
	float currentFrame = glfwGetTime();
	this->detaTime = currentFrame - this->lastFram;
	this->lastFram = currentFrame;

	glfwSwapBuffers(this->Window);
}

bool Renderer::ShouldClose()
{
	return glfwWindowShouldClose(this->Window);
}

void Renderer::Close()
{
	glfwTerminate();
}

void Renderer::ProcessInput()
{
	if (glfwGetKey(this->Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(this->Window, true);
	}
	if (glfwGetKey(this->Window, GLFW_KEY_W) == GLFW_PRESS)
	{
		MainCamera.ProcessKeyboard(Camera_Movement::FORWARD, this->detaTime);
	}
	if (glfwGetKey(this->Window, GLFW_KEY_S) == GLFW_PRESS)
	{
		MainCamera.ProcessKeyboard(Camera_Movement::BACKWARD, this->detaTime);
	}
	if (glfwGetKey(this->Window, GLFW_KEY_A) == GLFW_PRESS)
	{
		MainCamera.ProcessKeyboard(Camera_Movement::LEFT, this->detaTime);
	}
	if (glfwGetKey(this->Window, GLFW_KEY_D) == GLFW_PRESS)
	{
		MainCamera.ProcessKeyboard(Camera_Movement::RIGHT, this->detaTime);
	}
	if (glfwGetKey(this->Window, GLFW_KEY_E) == GLFW_PRESS)
	{
		MainCamera.ProcessKeyboard(Camera_Movement::UP, this->detaTime);
	}
	if (glfwGetKey(this->Window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		MainCamera.ProcessKeyboard(Camera_Movement::DOWN, this->detaTime);
	}
}

void Renderer::Mouse_Callback(GLFWwindow* window, double xpos, double ypos)
{
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		MainCamera.ProcessMouseMovement(xoffset, yoffset);
	}
	else
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

void Renderer::DrawTestGrid(Shader* shader)
{
	unsigned int planeVAO, planeVBO;
	float planeVertices[] = {
		// positions            // normals         // texcoords
		-1000.0f, -0.0f, -1000.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
		-1000.0f, -0.0f,  1000.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
		1000.0f, -0.0f,  1000.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,

		 1000.0f, -0.0f, 1000.0f,  0.0f, 1.0f, 0.0f,  25.0f, 0.0f,
		 1000.0f, -0.0f, -1000.0f,  0.0f, 1.0f, 0.0f,   25.0f, 25.0f,
		 -1000.0f, -0.0f, -1000.0f,  0.0f, 1.0f, 0.0f,  0.0f,  25.0f
	};

	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);

	shader->Use();
	shader->SetMat4("Projection", this->Projection);
	shader->SetMat4("View", this->MainCamera.GetViewMatrix());

	glUseProgram(shader->ID);
	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void Renderer::DebugLightDepthMap(Shader* debugShader, DirLight* light)
{
	GLuint quadVAO, quadVBO;
	GLfloat quadVertices[] = {
		// Positions        // Texture Coords
		-1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
	};
	// Setup plane VAO
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

	debugShader->Use();
	debugShader->setFloat("near_plane", light->NearPlane);
	debugShader->setFloat("far_plane", light->NearPlane);
	glActiveTexture(GL_TEXTURE0);
	ResourceManager::Textures[light->name].Bind();

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}
