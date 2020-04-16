/*
 * Program 3 base code - includes modifications to shape and initGeom in preparation to load
 * multi shape objects
 * CPE 471 Cal Poly Z. Wood + S. Sueda + I. Dunn
 */

#include <iostream>
#include <glad/glad.h>

#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "WindowManager.h"
#include "Texture.h"
#include "stb_image.h"

#define _USE_MATH_DEFINES
#include <math.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>

 // value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

#define NUM_OBJ_FILES 4
#define INIT_HEIGHT 680
#define INIT_WIDTH 750
#define MAX_OBJ 20
#define MAX_SHAPES 30
#define CUBE_INDEX 0
#define TREE_INDEX 1
#define HOLD_INDEX 2
#define NUM_TREES 11

#define MOUSE_SENSITIVITY_X 1.5
#define MOUSE_SENSITIVITY_Y 1.5
#define SKYBOX_SCALE 100.0f

// pitch domain constrained to -80 to 80 degrees
#define MAX_PHI 1.396263
#define MIN_PHI -1.396263
#define SPEED 10.f
#define TARGET_FPS 60.f

#define MAX_MAT_I 3

using namespace std;
using namespace glm;
using namespace chrono;

class Application : public EventCallbacks
{

public:

	WindowManager* windowManager = nullptr;

	// Our shader programs
	std::shared_ptr<Program> prog, progT, progC;

	// Our Textures:
	shared_ptr<Texture> barkTexture, rockTexture,
			            stoneWallTexture, rockRoadTexture;

	// Shape to be used (from  file) - modify to support diff amount
	shared_ptr<Shape> mesh[MAX_OBJ][MAX_SHAPES];

	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our triangle to OpenGL
	GLuint VertexBufferID;

	//example data that might be useful when trying to compute bounds on multi-shape
	vec3 gMin;

	// material/shading information
	int matState = 0;
	vec3 lightPos = vec3(-2.0f, 4.0f, 2.0f);

	// camera information
	double oldMouseX = 0;
	double oldMouseY = 0;
	double phi = 0;
	double theta = 0;
	double xSens = INIT_WIDTH / MOUSE_SENSITIVITY_X;
	double ySens = INIT_HEIGHT / MOUSE_SENSITIVITY_Y;
	vec3 up = vec3(0.0f, 1.0f, 0.0f);
	vec3 eye = vec3(0.0f, 2.0f, 7.5f);
	vec3 lookAtPos = eye + vec3(0.0f, 0.0f, -1.0f);
	vec3 u, v, w;
	short int movingA, movingW, movingS, movingD, movingDown;
	double oldTime = glfwGetTime();

	// skybox info
	int skyTextureID;

	// Random Scale Array (see initRandomScale)
	float randScales[NUM_TREES];

	// create array of random floats [0, 1.0] for scaling trees in the forest
	void initRandomScale()
	{
		for (int i = 0; i < NUM_TREES; i++)
		{
			randScales[i] = rand() * 1.0f / RAND_MAX;
		}
	}

	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if (key == GLFW_KEY_A && action == GLFW_PRESS) {
			movingA = 1;
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
			movingA = 0;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS) {
			movingD = 1;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
			movingD = 0;
		}
		if (key == GLFW_KEY_W && action == GLFW_PRESS) {
			movingW = 1;
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
			movingW = 0;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS) {
			movingS = 1;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
			movingS = 0;
		}
		if ((key == GLFW_KEY_RIGHT_SHIFT || key == GLFW_KEY_LEFT_SHIFT) && 
			(action == GLFW_PRESS || action == GLFW_REPEAT)) {
			eye -= vec3(v.x * SPEED, v.y * SPEED, v.z * SPEED);
			lookAtPos -= vec3(v.x * SPEED, v.y * SPEED, v.z * SPEED);
		}
		if (key == GLFW_KEY_SPACE && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			eye += vec3(v.x * SPEED, v.y * SPEED, v.z * SPEED);
			lookAtPos += vec3(v.x * SPEED, v.y * SPEED, v.z * SPEED);
		}
		if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		if (key == GLFW_KEY_Z && action == GLFW_RELEASE) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		if (key == GLFW_KEY_Q)
		{
			// if pressed key, considerable jump in light position
			if (action == GLFW_PRESS)
			{
				lightPos = vec3(lightPos.x - 0.5f, lightPos.y, lightPos.z);
			}
			// if holding key, smoother transition
			else if (action == GLFW_REPEAT)
			{
				lightPos = vec3(lightPos.x - 0.25f, lightPos.y, lightPos.z);
			}

		}
		if (key == GLFW_KEY_E)
		{
			// if pressed key, considerable jump in light position
			if (action == GLFW_PRESS)
			{
				lightPos = vec3(lightPos.x + 0.5f, lightPos.y, lightPos.z);
			}
			// if holding key, smoother transition
			else if (action == GLFW_REPEAT)
			{
				lightPos = vec3(lightPos.x + 0.25f, lightPos.y, lightPos.z);
			}

		}
	}

	// empty function to satisfy WindowManager.h
	void scrollCallback(GLFWwindow* window, double deltaX, double deltaY)
	{
	}

	void mouseCallback(GLFWwindow* window, int button, int action, int mods)
	{
		double posX, posY;
		// oldMouseX, oldMouseY

		if (action == GLFW_PRESS)
		{
			glfwGetCursorPos(window, &posX, &posY);
			cout << "Pos X " << posX << " Pos Y " << posY << endl;
		}
	}

	void cursorCallback(GLFWwindow* window, double xpos, double ypos)
	{
		double posX, posY;

		glfwGetCursorPos(window, &posX, &posY);
		double deltaX = oldMouseX - posX;
		double deltaY = oldMouseY - posY;
		oldMouseX = posX;
		oldMouseY = posY;

		theta += deltaX / xSens;
		phi = max(min(phi + deltaY / ySens, MAX_PHI), MIN_PHI);

		lookAtPos = vec3(-1.0 * cos(phi) * cos(theta), sin(phi), cos(phi) * cos(M_PI / 2.0 - theta))
			+ eye;
		
		w = normalize(eye - lookAtPos);
		u = normalize(cross(up, w));
		v = cross(w, u);
	}

	// checks moving flags and moves view accordingly
	void moveCamera(float seconds)
	{
		/* code to move game camera around */
		float timeStep = seconds;
		
		vec3 toMove;
		if (movingA) {
			toMove = -1.0f * vec3(u.x, 0, u.z) * SPEED * timeStep;
			eye += toMove;
			lookAtPos += toMove;
		}
		if (movingD) {
			toMove = vec3(u.x, 0, u.z) * SPEED * timeStep;
			eye += toMove;
			lookAtPos += toMove;
		}
		if (movingW) {
			toMove = -1.0f * vec3(w.x, 0, w.z) * SPEED * timeStep;
			eye += toMove;
			lookAtPos += toMove;
		}
		if (movingS) {
			toMove = vec3(w.x, 0, w.z) * SPEED * timeStep;
			eye += toMove;
			lookAtPos += toMove;
		}
	}

	void resizeCallback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
		xSens = width / MOUSE_SENSITIVITY_X;
		ySens = height / MOUSE_SENSITIVITY_Y;
	}

	// Code to load in skybox texture initially (provided)
	unsigned int createSky(string dir, vector<string> faces) {
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(false);
		for (GLuint i = 0; i < faces.size(); i++) {
			unsigned char* data =
				stbi_load((dir + faces[i]).c_str(), &width, &height, &nrChannels, 0);
			if (data) {
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			}
			else {
				cout << "failed to load: " << (dir + faces[i]).c_str() << endl;
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		if (glGetError() > 0)
		{
			cout << " creating cube map any errors : " << glGetError() << endl;
		}
		return textureID;
	}

	// Code to load in the textures initially
	void initTex(const std::string& resourceDirectory) {
		barkTexture = make_shared<Texture>();
		barkTexture->setFilename(resourceDirectory + "/bark.jpg");
		barkTexture->init();
		barkTexture->setUnit(0);
		barkTexture->setWrapModes(GL_REPEAT, GL_REPEAT);

		rockTexture = make_shared<Texture>();
		rockTexture->setFilename(resourceDirectory + "/purpleRock1.jpg");
		rockTexture->init();
		rockTexture->setUnit(2);
		rockTexture->setWrapModes(GL_REPEAT, GL_REPEAT);

		stoneWallTexture = make_shared<Texture>();
		stoneWallTexture->setFilename(resourceDirectory + "/stoneWall.jpg");
		stoneWallTexture->init();
		stoneWallTexture->setUnit(2);
		stoneWallTexture->setWrapModes(GL_REPEAT, GL_REPEAT);

		rockRoadTexture = make_shared<Texture>();
		rockRoadTexture->setFilename(resourceDirectory + "/redRoad.jpg");
		rockRoadTexture->init();
		rockRoadTexture->setUnit(2);
		rockRoadTexture->setWrapModes(GL_REPEAT, GL_REPEAT);
	}

	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(.12f, .34f, .56f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		// Initialize the GLSL program with no textures
		prog = make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/complex_vert.glsl", resourceDirectory + "/complex_frag.glsl");
		prog->init();
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addUniform("lightPos");
		prog->addUniform("shine");
		prog->addUniform("ambient");
		prog->addUniform("specColor");
		prog->addUniform("difColor");
		prog->addUniform("ambient");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertNor");
		prog->addAttribute("vertTex");

		// Initialize the second GLSL program with textures
		progT = make_shared<Program>();
		progT->setVerbose(true);
		progT->setShaderNames(resourceDirectory + "/tex_vert.glsl", resourceDirectory + "/tex_fragTile.glsl");
		progT->init();
		progT->addUniform("P");
		progT->addUniform("V");
		progT->addUniform("M");
		progT->addUniform("repeatCoeff");
		progT->addUniform("Texture0");
		progT->addUniform("ambient");
		progT->addUniform("shine");
		progT->addUniform("lightPos");
		progT->addAttribute("vertPos");
		progT->addAttribute("vertNor");
		progT->addAttribute("vertTex");

		initTex(resourceDirectory);

		// Initialize the third GLSL program with cube textures
		progC = make_shared<Program>();
		progC->setVerbose(true);
		progC->setShaderNames(resourceDirectory + "/cube_vert.glsl", resourceDirectory + "/cube_frag.glsl");
		progC->init();
		progC->addUniform("P");
		progC->addUniform("V");
		progC->addUniform("M");
		progC->addUniform("skybox");
		progC->addAttribute("vertPos");
		progC->addAttribute("vertNor");
		progC->addAttribute("vertTex");

		vector<std::string> faces{
						"vc_rt.tga",
						"vc_lf.tga",
						"vc_up.tga",
						"vc_dn.tga",
						"vc_ft.tga",
						"vc_bk.tga"
						};
		skyTextureID = createSky(resourceDirectory + "/cracks/", faces);

		initRandomScale();
	}

	void initGeom(const std::string & resourceDirectory, string* objFileName, int objc)
	{
		// Initialize mesh
		// Load geometry
		vector<tinyobj::shape_t> TOshapes;
		vector<tinyobj::material_t> objMaterials;
		string errStr;
		for (int j = 0; j < objc; j++)
		{
			//load in the mesh and make the shape(s)
			bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + objFileName[j]).c_str());
			if (!rc)
			{
				cerr << errStr << endl;
			}
			else
			{
				for (int i = 0; i < TOshapes.size(); i++)
				{
					mesh[j][i] = make_shared<Shape>();
					mesh[j][i]->createShape(TOshapes[i]);
					mesh[j][i]->measure();
					mesh[j][i]->init();
				}
			}
		}
	}

	void setModel(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack>M) {
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, glm::value_ptr(M->topMatrix()));
	}
	
	// draw the grass ground for the scene
	void drawGround(std::shared_ptr<MatrixStack> Model)
	{
		progT->bind();
		glUniform1f(progT->getUniform("repeatCoeff"), 10.0f);
		rockRoadTexture->bind(progT->getUniform("Texture0"));
		// draw mesh 
		Model->pushMatrix();
		Model->loadIdentity();
		/* draw ground */
		Model->pushMatrix();
		Model->translate(vec3(0, -0.5, 0));
		Model->scale(vec3(60.0, 1, 60.0));
		setModel(progT, Model);
		mesh[CUBE_INDEX][0]->draw(progT);
		Model->popMatrix();
		rockRoadTexture->unbind();
		progT->unbind();
	}

	void drawSkyBox(std::shared_ptr<MatrixStack> Model,
		mat4* View, std::shared_ptr<MatrixStack> Projection)
	{
		progC->bind();
		glUniformMatrix4fv(progC->getUniform("P"), 1, GL_FALSE, glm::value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(progC->getUniform("V"), 1, GL_FALSE, glm::value_ptr(*View));
		//set the depth function to always draw the box!
		glDepthFunc(GL_LEQUAL);
		//bind the cube map texture
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyTextureID);

		// draw box 
		Model->pushMatrix();
		Model->loadIdentity();
		Model->pushMatrix();
		Model->scale(vec3(SKYBOX_SCALE));
		setModel(progC, Model);

		mesh[CUBE_INDEX][0]->draw(progC);

		Model->popMatrix();
		Model->popMatrix();

		//set the depth test back to normal!
		glDepthFunc(GL_LESS);
		progC->unbind();
	}

	void render() {
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Use the matrix stack for Lab 6
		float aspect = width / (float)height;

		// Create the matrix stacks - please leave these alone for now
		auto Projection = make_shared<MatrixStack>();
		auto Model = make_shared<MatrixStack>();

		// Apply perspective projection.
		Projection->pushMatrix();
		Projection->perspective(45.0f, aspect, 0.01f, 300.0f);

		// Calculate time passed since last render
		float seconds;
		seconds = glfwGetTime() - oldTime;
		printf("%f\n", seconds);
		oldTime = glfwGetTime();

		// Camera orientation
		moveCamera(seconds);
		mat4 View = glm::lookAt(eye, lookAtPos, up);
		

		// Draw a stack of cubes with indiviudal transforms
		progT->bind();
		glUniformMatrix4fv(progT->getUniform("P"), 1, GL_FALSE, glm::value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(progT->getUniform("V"), 1, GL_FALSE, glm::value_ptr(View));

		glUniform3f(progT->getUniform("lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform1f(progT->getUniform("shine"), 1.0f);
		glUniform1f(progT->getUniform("ambient"), 0.3f);

		// draw scene elements
		drawGround(Model);

		//draw skybox scenery
		drawSkyBox(Model, &View, Projection);

		// Pop matrix stacks.
		Projection->popMatrix();

		progT->unbind();

	}
};

int main(int argc, char* argv[])
{
	// Where the resources are loaded from
	std::string resourceDir = "../resources";

	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application* application = new Application();

	// Your main will always include a similar set up to establish your window
	// and GL context, etc.

	WindowManager* windowManager = new WindowManager();
	windowManager->init(INIT_WIDTH, INIT_HEIGHT);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	// This is the code that will likely change program to program as you
	// may need to initialize or set up different data and state

	// array of object file names
	string objFiles[] = { "/cube.obj",
						  "/Tree.obj",
						  "/Rock1.obj",
						  "/dummy.obj"};

	application->init(resourceDir);
	application->initGeom(resourceDir, objFiles, NUM_OBJ_FILES);

	// Loop until the user closes the window.
	while (!glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}