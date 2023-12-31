﻿#include <iostream>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "reactphysics3d/reactphysics3d.h"

#include <string>
#include <vector>

#include "MyContactListener.h"
#include "Shader.h"
#include "Camera.h"
#include "stb_image.h"
#include "Model.h"
#include "Debugger.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Objeto.h"


using namespace std;
using namespace ImGui;

const unsigned width = 1600;
const unsigned height = 900;

float deltaTime = 0.0f;
float lastTime = 0.0f;

//Camera camera(vec3(100.0f, 30.0f, 100.0f));

float lastX = width / 2.0f;
float lastY = height / 2.0f;
bool firstMouse = true;

mat4 projection;
mat4 view;

bool windowTest = true;
bool fpsTest = true;

bool enableCameraMovement = true;
bool isCameraMovementKeyPressed = false;

bool isDebugerModeKeyPressed = false;
bool debuggerMode = true;

bool isFlashOn = true;
bool isFlashKeyPressed = false;

bool isJumpKeyPressed = false;

bool blinn = false;
bool blinnKeyPressed = false;

//Variables para el uso del ImGui y manipulaciones de la luz
//Direction Light
vec3 directionD;
float ambientD = 0.0f, diffuseD = 0.0f, specularD = 0.0f;

//Point Light
float ambientP = 0.05f, diffuseP = 0.8f, specularP = 0.3f, linearP = 0.065f, cuadraticP = 0.01f;

//FlashLight
float ambientF = 0.0f, diffuseF = 1.0f, specularF = 0.0f, linearF = 0.0014f, cuadraticF = 0.000007f, cutOff = 15.0f, outerCutOff = 23.0f;

float alturaLamp = 16.0f;

vec3 colorL = vec3(1.0f, 1.0f, 1.0f);

float materialShines = 8.0f;


//La altura antes estaba en 8.084 xd
vec3 posCubeLight[] = { vec3(0.0f, alturaLamp, 5.16f),
						vec3(-15.0f, alturaLamp, 0.56f),
						vec3(0.0f, alturaLamp, -5.16f),
						vec3(15.0f, alturaLamp, 0.56f) };

float circleRadius = 100.0f;
float rotationSpeed = 45.0f;

Camera camera(vec3(6.0f, 40.0f, 6.0f));

void initGLFWVersion();
bool gladLoad();
void updateWindow(GLFWwindow* window, Shader ourShader, Model ourModel, Model ourLamp, Shader ourLight);

void framebuffer_size_callback(GLFWwindow* window, int w, int h);
void processInput(GLFWwindow* window, PhysicsWorld* world, RigidBody* body);
void CameraInput(GLFWwindow* window, PhysicsWorld* world, RigidBody* body);
void Mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

//SetUniforms Functions
void TransformModel(Shader ourShader);
void TransformCamera(Shader ourShader);
void CameraUniform(Shader shaderName);
void TransformLight(Shader ourLight, Model ourLamp);

//Light Functions
void flashLight(Shader ourShader);
void directionalLight(Shader ourShader);
void pointLight(Shader ourShader, int n);

// ImGui Functions
void iniGui(GLFWwindow* window);
void showFPS();
void destroyGui();
void showWindowTest();
void restartValues();

int main()
{
	initGLFWVersion();

	//Creacion de ventana
	GLFWwindow* window = glfwCreateWindow(width, height, "OpenGl", NULL, NULL);
	if (window == NULL)
	{
		cout << "Falla al cargar ventana" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoad())
	{
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, Mouse_callback);
	glfwSetScrollCallback(window, Scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_FRAMEBUFFER_SRGB);


	iniGui(window);

	stbi_set_flip_vertically_on_load(true);

	Shader ourShader("vertexShader.vs", "fragmentShaderTexture.fs");
	Model ourMaze("Modelos/Laberinto1/Laberinto1.obj");
	Shader ourLight("vertexLight.vs", "fragmentLight.fs");
	Model ourLamp("Modelos/Lampara/cuboLampara.obj");

	updateWindow(window, ourShader, ourMaze, ourLamp, ourLight);

	destroyGui();

	glfwTerminate();


	return 0;
}

void initGLFWVersion()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif // __APPLE__
}

bool gladLoad()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Falla al cargar glad" << endl;
		return false;
	}
	return true;
}

void framebuffer_size_callback(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, w, h);

}

void processInput(GLFWwindow* window, PhysicsWorld* world, RigidBody* body)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !isFlashKeyPressed)
	{
		isFlashOn = !isFlashOn;
		isFlashKeyPressed = true;
		if (isFlashOn)
		{
			diffuseF = 1.0f;
		}
		else
		{
			diffuseF = 0.0f;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !blinnKeyPressed)
	{
		blinn = !blinn;
		blinnKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
	{
		blinnKeyPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE)
	{
		isFlashKeyPressed = false;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		camera.MovementSpeed = 9.0f;
	}
	else
	{
		camera.MovementSpeed = 5.0f;
	}

	CameraInput(window, world, body);
}

void CameraInput(GLFWwindow* window, PhysicsWorld* world, RigidBody* body)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && camera.isCameraDebugger)
	{
		camera.ProcessKeyboard(DOWN, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && camera.isCameraDebugger)
	{
		camera.ProcessKeyboard(UP, deltaTime);
	}
	/*
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !camera.isCameraDebugger && !isJumpKeyPressed)
	{
		isJumpKeyPressed = true;
		if (camera.isInFloor) {
			camera.isFall = true;
			camera.isJump = true;
			camera.isInFloor = false;
			camera.jumpVelocity = 5.0f;
		}
	}
	if (camera.isFall)
	{
		camera.jumpVelocity -= 9.8f * deltaTime;
		camera.Position.y += camera.jumpVelocity * deltaTime;
		if (camera.Position.y <= 10.0f)
		{
			camera.Position.y = 10.0f;
			camera.isJump = false;
			camera.isInFloor = true;
			camera.isFall = false;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE && !camera.isCameraDebugger)
	{
		isJumpKeyPressed = false;
	}*/
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && !isCameraMovementKeyPressed)
	{
		enableCameraMovement = !enableCameraMovement;
		isCameraMovementKeyPressed = true;
		if (enableCameraMovement == false)
		{
			firstMouse = true;
		}

		if (enableCameraMovement)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}

	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE)
	{
		isCameraMovementKeyPressed = false;

	}
	if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS && !isDebugerModeKeyPressed)
	{
		camera.isCameraDebugger = !(camera.isCameraDebugger);
		isDebugerModeKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_RELEASE)
	{
		isDebugerModeKeyPressed = false;
	}
}

void ProcessBodyMovement(GLFWwindow* window, PhysicsWorld* world, RigidBody* body, float deltaTime)
{
	// Obtener la transformaci�n actual del cuerpo
	Transform currentTransform = body->getTransform();
	bool isJump = false;
	// Velocidad de movimiento
	float movementSpeed = 5.0f;
	float jumpForce = 13.0f;

	// Obtener la orientaci�n actual del cuerpo
	Quaternion bodyOrientation = currentTransform.getOrientation();

	// Calcular el desplazamiento en funci�n de las teclas presionadas
	Vector3 displacement(0.0f, 0.0f, 0.0f);
	
	Quaternion newOrientation = Quaternion::identity();
	Vector3 angle(0.0f, -radians(camera.Yaw), 0.0f);
	newOrientation = Quaternion::fromEulerAngles(angle);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		displacement += newOrientation * Vector3(movementSpeed, 0.0f, 0.0f);
		//displacement += horizontalFront * velocity;

	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		displacement += newOrientation * Vector3(-movementSpeed, 0.0f, 0.0f);
		
		//displacement -= horizontalFront * velocity;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		displacement += newOrientation * Vector3(0.0f, 0.0f, -movementSpeed);
		//displacement -= camera.Right * velocity;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		displacement += newOrientation * Vector3(0.0f, 0.0f, movementSpeed);
		//displacement -= camera.Right * velocity;
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS || isJump == false)
	{
		isJump = true;
		displacement += newOrientation * Vector3(0.0f, jumpForce, 0.0f);
	}




	//Calcular la nueva posici�n
	Vector3 newPosition = currentTransform.getPosition() + displacement * deltaTime;

	//cout << newPosition.x << ", " << newPosition.y << ", " << newPosition.z << endl;

	Transform newTransform(newPosition, newOrientation);

	
	//newTransform = Transform::identity();

	body->setTransform(newTransform); 
	world->update(deltaTime);

}

void Mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	if (enableCameraMovement)
	{
		float xpos = xposIn;
		float ypos = yposIn;
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

		camera.ProcessMouseMovement(xoffset, yoffset);
	}

}

void Scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void updateWindow(GLFWwindow* window, Shader ourShader, Model ourModel, Model ourLamp, Shader ourLight)
{
	//Simular physicas
	float timeStep = 1.0f / 60.0f;
	double accumulator = 0.0f;
	Transform prevTransform;
	Transform currTransform;
	
	//Crear mundo
	PhysicsCommon physicsCommon;
	PhysicsWorld* world = physicsCommon.createPhysicsWorld();

	//Creacion del laberinto
	Vector3 pisoPos(0.0, 0.0, 0.0);
	Quaternion pisoOrient = Quaternion::identity();
	Transform pisoTransf(pisoPos, pisoOrient);
	RigidBody* piso = world->createRigidBody(pisoTransf);

	piso->setType(BodyType::STATIC);

	pisoTransf = Transform::identity();
	// Crear el TriangleMesh
	TriangleMesh* triangleMesh = physicsCommon.createTriangleMesh();
	

	int numberMeshes = ourModel.meshes.size();


	for (int i = 0; i < numberMeshes; i++) {
		
		const vector<Vertex>& vertices = ourModel.meshes[i].vertices;
		const vector<unsigned int>& indices = ourModel.meshes[i].indices;
		const int nbVertices = vertices.size();
		const int nbTriangles = indices.size() / 3;

		float* vertexArray = new float[3 * nbVertices];
		int* indexArray = new int[3 * nbTriangles];

		for (int j = 0; j < nbVertices; j++) {
			vertexArray[3 * j] = vertices[j].Position.x;
			vertexArray[3 * j + 1] = vertices[j].Position.y;
			vertexArray[3 * j + 2] = vertices[j].Position.z;
		}

		for (int j = 0; j < nbTriangles; j++) {
			indexArray[3 * j] = indices[3 * j];
			indexArray[3 * j + 1] = indices[3 * j + 1];
			indexArray[3 * j + 2] = indices[3 * j + 2];
		}

		TriangleVertexArray* triangleArray =
			new TriangleVertexArray(nbVertices, vertexArray, 3 * sizeof(float), nbTriangles,
				indexArray, 3 * sizeof(int),
				TriangleVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
				TriangleVertexArray::IndexDataType::INDEX_INTEGER_TYPE);
		

		triangleMesh->addSubpart(triangleArray);
	}

	// Crear el ConcaveMeshShape
	ConcaveMeshShape* concaveMesh = physicsCommon.createConcaveMeshShape(triangleMesh);
	Collider* pisoCollider = piso->addCollider(concaveMesh, pisoTransf);

	//Creacion de un collider para la camara
	Vector3 positionCameraCollider(camera.Position.x, camera.Position.y, camera.Position.z);
	Quaternion orientation = Quaternion::identity();
	Vector3 anguloInicial(0.0f, -radians(camera.Yaw), 0.0f);
	orientation = Quaternion::fromEulerAngles(anguloInicial);
	Transform transform(positionCameraCollider, orientation);
	RigidBody* body = world->createRigidBody(transform);
	body->setAngularLockAxisFactor(Vector3(0, 1, 0));


	body->setType(BodyType::DYNAMIC);
	body->setLinearDamping(1.25);
	body->setAngularDamping(1.25);

	const Vector3 halfExtents(0.5, 1.5, 0.5);
	BoxShape* boxShape = physicsCommon.createBoxShape(halfExtents);
	//CapsuleShape* capsuleShape = physicsCommon.createCapsuleShape(1.0, 2.0);
	prevTransform = transform;
	transform = Transform::identity();
	//Collider* collider = body->addCollider(capsuleShape, transform);
	Collider* collider = body->addCollider(boxShape, transform);

	Vector3 piso2Pos(0.0, 0.0, 0.0);
	Quaternion piso2Orient = Quaternion::identity();
	Transform piso2Transf(piso2Pos, pisoOrient);
	RigidBody* piso2 = world->createRigidBody(piso2Transf);

	piso2->setType(BodyType::STATIC);
	Vector3 pisoHalfExt(80.0, 8.0, 80.0);
	BoxShape* pisoBox = physicsCommon.createBoxShape(pisoHalfExt);
	Collider* piso2Collider = piso2->addCollider(pisoBox, piso2Transf);


	Shader debugShader("debugVertexShader.vs", "debugFragmentShader.fs");
	Debugger deb(world);
	deb.enableDebugRendering();

	MyContactListener listener;
	listener.setBodies(body, piso2);
	world->setEventListener(&listener);
	

	while (!glfwWindowShouldClose(window))
	{
		float currenTime = glfwGetTime();
		deltaTime = currenTime - lastTime;
		lastTime = currenTime;
		accumulator += deltaTime;

		processInput(window, world, body);
		camera.updateCamera(window, body, world, deltaTime, listener.isInContact);
		//ProcessBodyMovement(window, world, body, deltaTime);
		if (accumulator >= timeStep)
		{

			glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
			//glClearColor(0.12f, 0.46f, 0.76f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//Calculos de las f�sicas
			while (accumulator >= timeStep)
			{
				world->update(timeStep);
				accumulator -= timeStep;
			}
			decimal factor = accumulator / timeStep;
			currTransform = body->getTransform();
			Transform interTransform = Transform::interpolateTransforms(prevTransform, currTransform, factor);
			prevTransform = currTransform;



			ourShader.use();
			ourShader.setInt("material.diffuse", 0);
			ourShader.setInt("material.specular", 1);
			ourShader.setFloat("material.shininess", materialShines);
			TransformCamera(ourShader);
			directionalLight(ourShader);
			pointLight(ourShader, 4);
			flashLight(ourShader);
			TransformModel(ourShader);
			ourModel.Draw(ourShader);

			ourLight.use();
			ourLight.setVec3("aColor", colorL);
			ourLight.setFloat("material.shininess", materialShines);
			ourLight.setInt("texture_diffuse1", 0);
			TransformCamera(ourLight);
			TransformLight(ourLight, ourLamp);

			debugShader.use();
			TransformCamera(debugShader);
			deb.drawColliders();

			glfwSwapBuffers(window);
		}

		glfwPollEvents();
	
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		showFPS();

		//showWindowTest();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	}

	//world->destroyCollisionBody(body);
	world->destroyRigidBody(body);
	//world->destroyCollisionBody(piso);
	world->destroyRigidBody(piso);
	world->destroyRigidBody(piso2);
	
	physicsCommon.destroyConcaveMeshShape(concaveMesh);
	//physicsCommon.destroyConvexMeshShape(convexMeshS)
	physicsCommon.destroyBoxShape(boxShape);
	//physicsCommon.destroyCapsuleShape(capsuleShape);
	physicsCommon.destroyTriangleMesh(triangleMesh);
	physicsCommon.destroyPhysicsWorld(world);
	
}

void TransformCamera(Shader ourShader)
{

	projection = perspective(radians(camera.Zoom), (float)width / (float)height, 0.1f, 100000000.0f);
	view = camera.GetViewMatrix();
	CameraUniform(ourShader);
}

void CameraUniform(Shader shaderName)
{
	shaderName.setMat4("projection", projection);
	shaderName.setMat4("view", view);
}

void TransformModel(Shader ourShader)
{
	mat4 modelo = mat4(1.0f);
	modelo = translate(modelo, vec3(0.0f, 0.1f, 0.0f));


	ourShader.setMat4("model", modelo);
}

void TransformLight(Shader ourLight, Model ourLamp)
{
	int tam = sizeof(posCubeLight) / sizeof(posCubeLight[0]);
	for (int i = 0; i < tam; i++)
	{
		mat4 modelo = mat4(1.0f);
		modelo = translate(modelo, posCubeLight[i]);
		ourLight.setMat4("model", modelo);
		ourLamp.Draw(ourLight);
	}
}

//Light Functions
void flashLight(Shader ourShader)
{
	ourShader.setVec3("spotLight.position", camera.Position);
	ourShader.setVec3("spotLight.direction", camera.Front);
	ourShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	ourShader.setVec3("spotLight.diffuse", diffuseF, diffuseF, diffuseF);
	ourShader.setVec3("spotLight.specular", specularF, specularF, specularF);
	ourShader.setFloat("spotLight.constant", 1.0f);
	ourShader.setFloat("spotLight.linear", linearF);
	ourShader.setFloat("spotLight.quadratic", cuadraticF);
	ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(cutOff)));
	ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(outerCutOff)));
}

void pointLight(Shader ourShader, int n)
{
	for (int i = 0; i < n; i++)
	{
		ourShader.setVec3("pointLights[" + to_string(i) + "].position", posCubeLight[i]);
		ourShader.setVec3("pointLights[" + to_string(i) + "].ambient", ambientP, ambientP, ambientP);
		//ourShader.setVec3("pointLights[" + to_string(i) + "].diffuse", diffuseP, diffuseP, diffuseP);
		ourShader.setVec3("pointLights[" + to_string(i) + "].diffuse", colorL);
		ourShader.setVec3("pointLights[" + to_string(i) + "].specular", specularP, specularP, specularP);
		ourShader.setFloat("pointLights[" + to_string(i) + "].constant", 1.0f);
		ourShader.setFloat("pointLights[" + to_string(i) + "].linear", linearP);
		ourShader.setFloat("pointLights[" + to_string(i) + "].quadratic", cuadraticP);
	}

}

void directionalLight(Shader ourShader)
{
	ourShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
	//ourShader.setVec3("dirLight.direction", directionD.x, directionD.y, directionD.z);
	ourShader.setVec3("dirLight.ambient", ambientD, ambientD, ambientD);
	ourShader.setVec3("dirLight.diffuse", diffuseD, diffuseD, diffuseD);
	ourShader.setVec3("dirLight.specular", specularD, diffuseD, diffuseD);
}

//ImGui Functions
void restartValues()
{
	ambientD = 0.05f;
	diffuseD = 0.4f;
	specularD = 0.5f;

	//Point Light
	ambientP = 0.05f;
	diffuseP = 0.8f;
	specularP = 1.0f;
	linearP = 0.09f;
	cuadraticP = 0.032f;
	colorL = vec3(1.0f, 1.0f, 1.0f);

	//FlashLight
	ambientF = 0.0f;
	diffuseF = 1.0f;
	specularF = 1.0f;
	linearF = 0.09f;
	cuadraticF = 0.032f;
	cutOff = 12.5f;
	outerCutOff = 15.0f;


}

void showFPS()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(200, 100));
	ImGui::Begin("FPS", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground);
	ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
	ImGui::End();

}

void showWindowTest()
{
	if (windowTest)
	{
		ImGui::Begin("Window Test", &windowTest);

		ImGui::Text("Character Position");
		ImGui::InputFloat("x", &camera.Position.x);
		ImGui::InputFloat("y", &camera.Position.y);
		ImGui::InputFloat("z", &camera.Position.z);
		ImGui::Text("Material Shiness");
		ImGui::SliderFloat("Material", &materialShines, 0.0f, 200.0f);

		ImGui::Text("Directional Light");
		ImGui::SliderFloat("Ambient Directional", &ambientD, 0.0f, 1.0f);
		ImGui::SliderFloat("Diffuse Directional", &diffuseD, 0.0f, 1.0f);
		ImGui::SliderFloat("Specular Directional", &specularD, 0.0f, 1.0f);

		ImGui::Text("\nLight");
		ImGui::ColorEdit4("Light Color", &colorL.x);
		ImGui::SliderFloat("Speed", &rotationSpeed, 45.0f, 200.0f);
		ImGui::SliderFloat("Radius", &circleRadius, 40.0f, 100.0f);

		ImGui::Text("\nPoint Light: ");
		ImGui::SliderFloat("Altura Light", &posCubeLight[0].y, 0.1f, 10.0f);
		ImGui::SliderFloat("Light x", &posCubeLight[0].x, 0.1f, 10.0f);
		ImGui::SliderFloat("Light z", &posCubeLight[0].z, 0.1f, 10.0f);
		ImGui::SliderFloat("Ambient Point", &ambientP, 0.0f, 1.0f);
		ImGui::SliderFloat("Diffuse Point", &diffuseP, 0.0f, 1.0f);
		ImGui::SliderFloat("Specular Point", &specularP, 0.0f, 1.0f);
		ImGui::SliderFloat("Linear Point", &linearP, 0.0014f, 0.7f);
		ImGui::SliderFloat("Quadratic Point", &cuadraticP, 0.000007f, 1.8f);

		ImGui::Text("\nFlashLight: ");
		ImGui::SliderFloat("Diffuse Flash", &diffuseF, 0.0f, 1.0f);
		ImGui::SliderFloat("Specular Flash", &specularF, 0.0f, 1.0f);
		ImGui::SliderFloat("Linear Flash", &linearF, 0.0014f, 0.7f);
		ImGui::SliderFloat("Quadratic Flash", &cuadraticF, 0.000007f, 1.8f);
		ImGui::SliderFloat("cutOff", &cutOff, 0.0f, 50.0f);
		ImGui::SliderFloat("outerCutOff", &outerCutOff, 0.0f, 50.0f);

		if (ImGui::Button("RestartValues"))
			restartValues();

		if (ImGui::Button("Close"))
			windowTest = false;



		ImGui::End();
	}
}

void iniGui(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGuiStyle& style = ImGui::GetStyle();

	style.WindowRounding = 5.0f;  // Bordes redondeados para ventanas
	style.FrameRounding = 4.0f;   // Bordes redondeados para elementos
	style.GrabRounding = 4.0f;    // Bordes redondeados para barras de desplazamiento
	style.AntiAliasedLines = true;
	style.AntiAliasedFill = true;

	// Colores de estilo
	style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // Texto en blanco
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.2f, 0.2f, 0.2f, 0.7f); // Fondo de las ventanas
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.4f, 0.4f, 0.4f, 0.6f); // Fondo de los elementos

	ImGui::StyleColorsDark();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

void destroyGui()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}