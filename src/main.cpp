#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>
#include <ctime>

#include "Texture.h"
#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Camera.h"
#include "transformations.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int mainWindow;
float windowWidth = 600.0;
float windowHeight = 600.0;

GLuint program;
GLuint programSunTexturing;
GLuint programTexturing;
GLuint programProceduralTexturing;	//  for spaceship
GLuint programNormalmapTexturing;

// Objects
obj::Model planeModel;
obj::Model shipModel;
obj::Model sphereModel;

// Textures
GLuint textureSun, textureMercury, textureVenus, textureEarth, textureComet;
GLuint textureTest, textureAsteroid, textureShip;

// Normal mapping
GLuint normalmapShip;
GLuint normalmapEarth;
GLuint normalmapAsteroid;
GLuint normalmapTest;

Core::Shader_Loader shaderLoader;
Core::RenderContext shipContext;
Core::RenderContext sphereContext;
Core::RenderContext saturnContext;
Core::RenderContext planeContext;

float cameraAngle = 0;
glm::vec3 cameraPos = glm::vec3(0, 0, 7);
glm::vec3 cameraDir; // Wektor "do przodu" kamery
glm::vec3 cameraSide; // Wektor "w bok" kamery
glm::mat4 cameraMatrix, perspectiveMatrix;

// grk7 - quaternions and camera movement
glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, -0.9f, -1.0f));
glm::quat rotation = glm::quat(1.f, 0.f, 0.f, 0.f);
float zOffset = 0.0;
float xOffset, yOffset;

// variables for fps check
int myframe;
long mytime, mytimebase;


void keyboard(unsigned char key, int x, int y)
{
	float angleSpeed = 2.0f;
	float moveSpeed = 0.1f;
	switch (key)
	{
	case 27: exit(0);	// ESC key to exit the window
	case 'z': zOffset -= angleSpeed; break;
	case 'x': zOffset += angleSpeed; break;
	case 'w': cameraPos += cameraDir * moveSpeed; break;
	case 's': cameraPos -= cameraDir * moveSpeed; break;
	case 'd': cameraPos += glm::cross(cameraDir, glm::vec3(0, 1, 0)) * moveSpeed; break;
	case 'a': cameraPos -= glm::cross(cameraDir, glm::vec3(0, 1, 0)) * moveSpeed; break;
	case 'e': cameraPos += glm::cross(cameraDir, glm::vec3(1, 0, 0)) * moveSpeed; break;
	case 'q': cameraPos -= glm::cross(cameraDir, glm::vec3(1, 0, 0)) * moveSpeed; break;
	}
}

void mouse(int x, int y)
{
	const float mouseSensitivity = 1.0f;
	xOffset = (x - (windowWidth / 2)) * mouseSensitivity;
	yOffset = (y - (windowHeight / 2)) * mouseSensitivity;
}

glm::mat4 createCameraMatrix(float xOffset, float yOffset)
{
	glm::quat xAxisQuaternion = glm::angleAxis(glm::radians(xOffset), glm::vec3(0, 1, 0));
	glm::quat yAxisQuaternion = glm::angleAxis(glm::radians(yOffset), glm::vec3(1, 0, 0));
	glm::quat rotationChange = yAxisQuaternion * xAxisQuaternion;

	rotation = glm::normalize(rotationChange * rotation);
	cameraDir = glm::inverse(rotation) * glm::vec3(0, 0, -1);
	cameraSide = glm::inverse(rotation) * glm::vec3(1, 0, 0);

	glutWarpPointer(windowWidth / 2, windowHeight / 2);	// locks cursor inside window

	return Core::createViewMatrixQuat(cameraPos, rotation);
}

void drawObject(Core::RenderContext context, glm::mat4 modelMatrix, glm::vec3 color, GLuint program)
{
	glUseProgram(program);
	glUniform3f(glGetUniformLocation(program, "objectColor"), color.x, color.y, color.z);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniform3f(glGetUniformLocation(program, "spotlightPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	glUniform3f(glGetUniformLocation(program, "spotlightDir"), cameraDir.x, cameraDir.y, cameraDir.z);
	glUniform1f(glGetUniformLocation(program, "spotlightCutOff"), glm::cos(glm::radians(12.5f)));
	glUniform1f(glGetUniformLocation(program, "spotlightOuterCutOff"), glm::cos(glm::radians(17.5f)));

	Core::DrawContext(context);
	glUseProgram(0);
}

void drawObjectTexture(Core::RenderContext context, glm::mat4 modelMatrix, GLuint textureID, GLuint program)
{
	glUseProgram(program);
	Core::SetActiveTexture(textureID, "colorTexture", program, 0);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniform3f(glGetUniformLocation(program, "lightPos"), 0, 0, 0);
	glUniform3f(glGetUniformLocation(program, "spotlightPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	glUniform3f(glGetUniformLocation(program, "spotlightDir"), cameraDir.x, cameraDir.y, cameraDir.z);
	glUniform1f(glGetUniformLocation(program, "spotlightCutOff"), glm::cos(glm::radians(12.5f)));
	glUniform1f(glGetUniformLocation(program, "spotlightOuterCutOff"), glm::cos(glm::radians(17.5f)));

	Core::DrawContext(context);
	glUseProgram(0);
}

void setUpUniforms(GLuint program, glm::mat4 modelMatrix)
{
	//glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	glUniform3f(glGetUniformLocation(program, "lightPos"), 0, 0, 0); // aka LightPos ( (0,0,0) czyli pozycja Slonca w naszym ukladzie) ale z pozostawiona nazwa lightDir zeby za duzo nie zmieniac w shaderach
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	glUniform3f(glGetUniformLocation(program, "spotlightPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	glUniform3f(glGetUniformLocation(program, "spotlightDir"), cameraDir.x, cameraDir.y, cameraDir.z);
	glUniform1f(glGetUniformLocation(program, "spotlightCutOff"), glm::cos(glm::radians(12.5f)));
	glUniform1f(glGetUniformLocation(program, "spotlightOuterCutOff"), glm::cos(glm::radians(17.5f)));

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
}

void drawObjectTextureWithNormalmap(Core::RenderContext context, glm::mat4 modelMatrix, GLuint textureId, GLuint normalmapId, GLuint program)
{
	glUseProgram(program);

	setUpUniforms(program, modelMatrix);
	Core::SetActiveTexture(textureId, "textureSampler", program, 0);
	Core::SetActiveTexture(normalmapId, "normalSampler", program, 1);

	Core::DrawContext(context);
	glUseProgram(0);
}

void renderScene()
{
	cameraMatrix = createCameraMatrix(xOffset, yOffset);
	perspectiveMatrix = Core::createPerspectiveMatrix();

	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.f;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);

	// Macierz statku "przyczepia" go do kamery.
	//glm::mat4 shipModelMatrix = glm::translate(cameraPos + cameraDir * 0.5f + glm::vec3(0, -0.25f, 0)) * glm::rotate(-cameraAngle + glm::radians(90.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::vec3(0.25f));
	glm::mat4 shipInitialTransformation = glm::translate(glm::vec3(0, -0.25f, 0)) * glm::rotate(glm::radians(180.0f), glm::vec3(0, 1, 0)) * glm::rotate(glm::radians(zOffset), glm::vec3(0, 0, 1)) * glm::scale(glm::vec3(0.25f));
	glm::mat4 shipModelMatrix = glm::translate(cameraPos + cameraDir * 0.5f) * glm::mat4_cast(glm::inverse(rotation)) * shipInitialTransformation;

	// W sumie to po co to?
	glUseProgram(program);
	glUniform3f(glGetUniformLocation(program, "lightPos"), 0, 0, 0);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	drawObject(shipContext, shipModelMatrix, glm::vec3(0.6f), programProceduralTexturing);

	// Sun
	drawObjectTexture(sphereContext, glm::translate(glm::vec3(0, 0, 0)) * glm::scale(glm::vec3(0.95, 0.95, 0.95)), textureSun, programSunTexturing);
	// Mercury
	drawObjectTexture(sphereContext, T::orbitalSpeed(300) * glm::translate(glm::vec3(1.5f, 0.f, 0.f)) * T::scaling(0.20), textureMercury, programTexturing);
	// Venus
	drawObjectTexture(sphereContext, T::orbitalSpeed(150) * glm::translate(glm::vec3(2.f, 0.f, 0.f)) * T::scaling(0.30), textureVenus, programTexturing);
	// Earth
	drawObjectTextureWithNormalmap(sphereContext, glm::translate(glm::vec3(3.f, 0.f, 0.f)), textureEarth, normalmapEarth, programNormalmapTexturing);
	// Moon
	//drawObject(sphereContext, T::orbitalSpeed(120) * glm::translate(glm::vec3(3.f, 0.f, 0.f)) * T::moonRotation(65, 0.005) * glm::translate(glm::vec3(0.5f, 0.f, 0.f)) * T::scaling(0.05), glm::vec3(0.3), program);
	// Comet
	//drawObjectTexture(sphereContext, T::cometRotation(200, glm::vec3(1.f, -0.5f, 0.7f)) * glm::translate(glm::vec3(0.f, 4.f, 0.f)) * T::scaling(0.20), textureComet, programTexturing);

	
	//drawObjectTextureWithNormalmap(&planeModel, glm::translate(glm::vec3(-3, 0, 0)) * glm::scale(glm::vec3(1, 0.5, 0.5)), textureTest, normalmapTest);
	drawObjectTextureWithNormalmap(planeContext, glm::translate(glm::vec3(0, 0, -3)) * glm::scale(glm::vec3(1, 0.5, 0.5)), textureTest, normalmapTest, programNormalmapTexturing);

	/*
	// Code to check fps (simply uncomment to use)
	myframe++;
	time = glutGet(GLUT_ELAPSED_TIME);
	if (time - mytimebase > 1000) {
		printf("FPS:%4.2f\n", myframe * 1000.0 / (time - mytimebase));
		mytimebase = time;
		myframe = 0;
	}
	*/

	glutSwapBuffers();
}

void init()
{
	glEnable(GL_DEPTH_TEST);

	program = shaderLoader.CreateProgram("shaders/shader.vert", "shaders/shader.frag");
	programSunTexturing = shaderLoader.CreateProgram("shaders/sun.vert", "shaders/sun.frag");
	programTexturing = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_tex.frag");
	programProceduralTexturing = shaderLoader.CreateProgram("shaders/shader_proc_tex.vert", "shaders/shader_proc_tex.frag");
	programNormalmapTexturing = shaderLoader.CreateProgram("shaders/normalmap.vert", "shaders/normalmap.frag");

	sphereModel = obj::loadModelFromFile("models/sphere.obj");
	shipModel = obj::loadModelFromFile("models/spaceship.obj");
	planeModel = obj::loadModelFromFile("models/plane.obj");

	textureSun = Core::LoadTexture("textures/sun.png");
	textureMercury = Core::LoadTexture("textures/mercury.png");
	textureVenus = Core::LoadTexture("textures/venus.png");
	textureEarth = Core::LoadTexture("textures/earth.png");
	textureComet = Core::LoadTexture("textures/comet.png");

	normalmapShip = Core::LoadTexture("textures/spaceship_normals.png");
	normalmapEarth = Core::LoadTexture("textures/earth_normals.png");
	normalmapAsteroid = Core::LoadTexture("textures/asteroid_normals.png");
	normalmapTest = Core::LoadTexture("textures/test_normals.png");

	shipContext.initFromOBJ(shipModel);
	sphereContext.initFromOBJ(sphereModel);
	planeContext.initFromOBJ(planeModel);
}

void shutdown()
{
	shaderLoader.DeleteProgram(program);
	shaderLoader.DeleteProgram(programSunTexturing);
	shaderLoader.DeleteProgram(programTexturing);
	shaderLoader.DeleteProgram(programProceduralTexturing);
}

/*
void idle()
{
	glutPostRedisplay();
}
*/

void timer(int) {
	glutPostRedisplay();
	glutTimerFunc(1000 / 60, timer, 0);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(windowWidth, windowHeight);
	glutCreateWindow("OpenGL project");
	glewInit();

	init();
	glutKeyboardFunc(keyboard);
	glutPassiveMotionFunc(mouse);
	glutSetCursor(GLUT_CURSOR_NONE);
	glutDisplayFunc(renderScene);
	//glutIdleFunc(idle);	// CPU usage goes up to ~99% with this; new solution below
	timer(0);				// restricts program to ~60 fps

	glutMainLoop();

	shutdown();

	return 0;
}
