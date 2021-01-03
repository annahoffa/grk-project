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

#include "Box.cpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint program;
GLuint programSun;
GLuint programSunTex;
GLuint programTex;
GLuint texSun, texMercury, texVenus, texEarth, texMars;
GLuint statekProc;
Core::Shader_Loader shaderLoader;

obj::Model shipModel;
obj::Model sphereModel;
Core::RenderContext shipContext;
Core::RenderContext sphereContext;
Core::RenderContext saturnContext;

float cameraAngle = 0;
glm::vec3 cameraPos = glm::vec3(-6, 0, 0);
glm::vec3 cameraDir;

glm::mat4 cameraMatrix, perspectiveMatrix;


void keyboard(unsigned char key, int x, int y)
{
	float angleSpeed = 0.1f;
	float moveSpeed = 0.1f;
	switch (key)
	{
	case 'z': cameraAngle -= angleSpeed; break;
	case 'x': cameraAngle += angleSpeed; break;
	case 'w': cameraPos += cameraDir * moveSpeed; break;
	case 's': cameraPos -= cameraDir * moveSpeed; break;
	case 'd': cameraPos += glm::cross(cameraDir, glm::vec3(0, 1, 0)) * moveSpeed; break;
	case 'a': cameraPos -= glm::cross(cameraDir, glm::vec3(0, 1, 0)) * moveSpeed; break;
	case 'e': cameraPos += glm::cross(cameraDir, glm::vec3(1, 0, 0)) * moveSpeed; break;
	case 'q': cameraPos -= glm::cross(cameraDir, glm::vec3(1, 0, 0)) * moveSpeed; break;
	}
}

glm::mat4 createCameraMatrix()
{
	// Obliczanie kierunku patrzenia kamery (w plaszczyznie x-z) przy uzyciu zmiennej cameraAngle kontrolowanej przez klawisze.
	cameraDir = glm::vec3(cosf(cameraAngle), 0.0f, sinf(cameraAngle));
	glm::vec3 up = glm::vec3(0, 1, 0);

	return Core::createViewMatrix(cameraPos, cameraDir, up);
}

void drawObject(Core::RenderContext context, glm::mat4 modelMatrix, glm::vec3 color, GLuint program)
{
	glUseProgram(program);
	glUniform3f(glGetUniformLocation(program, "objectColor"), color.x, color.y, color.z);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

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

	Core::DrawContext(context);
	glUseProgram(0);
}

glm::mat4 orbitalSpeed(float angle)
{
	float time = glutGet(GLUT_ELAPSED_TIME) / 10000.0f;

	glm::mat4 rotation;
	rotation = glm::rotate(rotation, time * glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));

	return rotation;
}

glm::mat4 scaling(float size)
{
	glm::mat4 scal = glm::scale(glm::vec3(size, size, size));
	return scal;
}

glm::mat4 moonRotation(float angle, float deviation)
{
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	glm::mat4 rotation;
	rotation = glm::rotate(rotation, time * glm::radians(angle), glm::vec3(0.0f, 0.1f, deviation));

	return rotation;
}

void renderScene()
{
	// Aktualizacja macierzy widoku i rzutowania. Macierze sa przechowywane w zmiennych globalnych, bo uzywa ich funkcja drawObject.
	// (Bardziej elegancko byloby przekazac je jako argumenty do funkcji, ale robimy tak dla uproszczenia kodu.
	//  Jest to mozliwe dzieki temu, ze macierze widoku i rzutowania sa takie same dla wszystkich obiektow!)
	cameraMatrix = createCameraMatrix();
	perspectiveMatrix = Core::createPerspectiveMatrix();
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.f;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);

	// Macierz statku "przyczpeia" go do kamery.
	glm::mat4 shipModelMatrix = glm::translate(cameraPos + cameraDir * 0.5f + glm::vec3(0, -0.25f, 0)) * glm::rotate(-cameraAngle + glm::radians(90.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::vec3(0.25f));

	glUseProgram(program);
	glUniform3f(glGetUniformLocation(program, "lightPos"), 0, 0, 0);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	glUseProgram(programTex);
	glUniform3f(glGetUniformLocation(programTex, "lightPos"), 0, 0, 0);
	glUniform3f(glGetUniformLocation(programTex, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	glUseProgram(statekProc);
	glUniform3f(glGetUniformLocation(statekProc, "lightPos"), 0, 0, 0);
	glUniform3f(glGetUniformLocation(statekProc, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	glUseProgram(programSun);
	glUniform3f(glGetUniformLocation(programSun, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	drawObject(shipContext, shipModelMatrix, glm::vec3(0.6f), statekProc);

	// Sun
	drawObject(sphereContext, glm::translate(glm::vec3(0, 0, 0)) * glm::scale(glm::vec3(0.95, 0.95, 0.95)), glm::vec3(0.9, 0.7, 0.1), programSun);
	// Mercury
	drawObjectTexture(sphereContext, orbitalSpeed(300) * glm::translate(glm::vec3(1.5f, 0.f, 0.f)) * scaling(0.20), texMercury, programTex);
	// Venus
	drawObjectTexture(sphereContext, orbitalSpeed(150) * glm::translate(glm::vec3(2.f, 0.f, 0.f)) * scaling(0.30), texVenus, programTex);
	// Earth
	drawObjectTexture(sphereContext, orbitalSpeed(120) * glm::translate(glm::vec3(3.0f, 0.f, 0.f)) * scaling(0.35), texEarth, programTex);
	// Moon
	drawObject(sphereContext, orbitalSpeed(120) * glm::translate(glm::vec3(3.0f, 0.f, 0.f)) * moonRotation(65, 0.005) * glm::translate(glm::vec3(0.5f, 0.f, 0.f)) * scaling(0.05), glm::vec3(0.3), program);

	glutSwapBuffers();
}

void init()
{
	glEnable(GL_DEPTH_TEST);
	program = shaderLoader.CreateProgram("shaders/shader.vert", "shaders/shader.frag");
	programSun = shaderLoader.CreateProgram("shaders/shader_sun.vert", "shaders/shader_sun.frag");
	programTex = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_tex.frag");
	statekProc = shaderLoader.CreateProgram("shaders/shader_proc_tex.vert", "shaders/shader_proc_tex.frag");
	texEarth = Core::LoadTexture("textures/earth2.png");
	texMercury = Core::LoadTexture("textures/mercury.png");
	texVenus = Core::LoadTexture("textures/venus.png");
	sphereModel = obj::loadModelFromFile("models/sphere.obj");
	shipModel = obj::loadModelFromFile("models/spaceship.obj");
	shipContext.initFromOBJ(shipModel);
	sphereContext.initFromOBJ(sphereModel);
}

void shutdown()
{
	shaderLoader.DeleteProgram(program);
	shaderLoader.DeleteProgram(programSunTex);
	shaderLoader.DeleteProgram(programTex);
	shaderLoader.DeleteProgram(statekProc);
}

void idle()
{
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(600, 600);
	glutCreateWindow("OpenGL project");
	glewInit();

	init();
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(renderScene);
	glutIdleFunc(idle);

	glutMainLoop();

	shutdown();

	return 0;
}
