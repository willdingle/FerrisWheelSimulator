#include <iostream>
using namespace std;

//--- OpenGL ---
#include "GL\glew.h"
#include "GL\wglew.h"
#pragma comment(lib, "glew32.lib")
//--------------

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtc\matrix_inverse.hpp"

#include "GL\freeglut.h"

#include "Images\FreeImage.h"

#include "shaders\Shader.h"

//Shaders
CShader* myShader;
CShader* myBasicShader;

//MODEL LOADING
#include "3DStruct\threeDModel.h"
#include "Obj\OBJLoader.h"
#include "FerrisWheel\FerrisWheel.h"

float amount = 0;
float temp = 0.002f;

CThreeDModel ground, lightTestObj;
FerrisWheel ferrisWheel;
COBJLoader objLoader;

//Camera and view
#include "Camera/Camera.h"
Camera freeCam = Camera(glm::vec3(0.0f, 12.0f, 30.0f));
Camera groundCam = Camera(glm::vec3(10.0f, 1.5f, 30.0f));
Camera rideCam = Camera(glm::vec3(0.0f, 3.25f, 5.0f));
Camera *currentCam;
glm::mat4 ProjectionMatrix;

//Material properties
float Material_Ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
float Material_Diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float Material_Specular[4] = { 1.0f,1.0f,1.0f,1.0f };
float Material_Shininess = 50;

//Light Properties
float Light_Ambient_And_Diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float Light_Specular[4] = { 1.0f,1.0f,1.0f,1.0f };
float LightPos[4] = { 100.0f, 100.0f, 100.0f, 0.0f };
float SecondLightPos[4] = { -100.0f, 100.0f, -100.0f, 0.0f };

//Screen size
int screenWidth = 1280, screenHeight = 720;

//Booleans for key presses
bool Left = false;
bool Right = false;
bool Up = false;
bool Down = false;
bool a = false;
bool d = false;
bool w = false;
bool s = false;

//Boolean for viewing collision boxes
bool viewCollisions = false;

//Sphere
#include "Sphere\Sphere.h";
Sphere mySphere, staticSphere;

//delta time
int oldTimeSinceStart = 0;
int timeSinceStart = 0;
int deltaTime = 0;

//OPENGL FUNCTION PROTOTYPES
void display();				//called in winmain to draw everything to the screen
void reshape(int width, int height);				//called when the window is resized
void init();				//called in winmain when the program starts.
void processKeys();         //called in winmain to process keyboard input
void idle();		//idle function

/*************    START OF OPENGL FUNCTIONS   ****************/
void display()
{
	timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
	deltaTime = timeSinceStart - oldTimeSinceStart;
	oldTimeSinceStart = timeSinceStart;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(myShader->GetProgramObjID());  // use the shader

	//Part for displacement shader.
	amount += temp;
	if (amount > 1.0f || amount < -1.5f)
		temp = -temp;
	//amount = 0;
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "displacement"), amount);

	//translation and rotation for view
	glm::mat4 viewingMatrix = glm::mat4(1.0f);
	if (currentCam == &rideCam)
	{
		glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 12, 0));
		ModelMatrix = glm::rotate(ModelMatrix, ferrisWheel.getAngle(), glm::vec3(0, 0, 1.0));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0, -9.0f, 9.0f));
		glm::vec3 camPos = glm::vec3(ModelMatrix[3][0], ModelMatrix[3][1], ModelMatrix[3][2]);
		currentCam->setPos(camPos);
	}

	currentCam->render(myShader);
	viewingMatrix = currentCam->calcMatrix();
	if (viewCollisions)
		currentCam->viewCollisionBoxes(myShader, ferrisWheel.getAngle());

	//Set the projection matrix in the shader
	GLuint projMatLocation = glGetUniformLocation(myShader->GetProgramObjID(), "ProjectionMatrix");
	glUniformMatrix4fv(projMatLocation, 1, GL_FALSE, &ProjectionMatrix[0][0]);

	//Ferris wheel
	ferrisWheel.render(viewingMatrix, myShader, deltaTime);

	//Ground
	glm::mat4 ModelMatrix = glm::mat4(1.0f);
	glm::mat4 ModelViewMatrix = viewingMatrix * ModelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glm::mat4 NormalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	ground.DrawElementsUsingVBO(myShader);

	//Light Test Object
	ModelMatrix = glm::mat4(1.0f);
	ModelViewMatrix = viewingMatrix * ModelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	NormalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	lightTestObj.DrawElementsUsingVBO(myShader);

	//Light properties
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "LightPos"), 1, LightPos);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "SecondLightPos"), 1, SecondLightPos);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "light_ambient"), 1, Light_Ambient_And_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "light_diffuse"), 1, Light_Ambient_And_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "light_specular"), 1, Light_Specular);

	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "material_ambient"), 1, Material_Ambient);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "material_diffuse"), 1, Material_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "material_specular"), 1, Material_Specular);
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "material_shininess"), Material_Shininess);

	glFlush();
	glutSwapBuffers();
}

void reshape(int width, int height)		// Resize the OpenGL window
{
	screenWidth = width; screenHeight = height;           // to ensure the mouse coordinates match 
	// we will use these values to set the coordinate system

	glViewport(0, 0, width, height);						// Reset The Current Viewport

	//Set the projection matrix
	ProjectionMatrix = glm::perspective(glm::radians(60.0f), (GLfloat)screenWidth / (GLfloat)screenHeight, 1.0f, 200.0f);
}
void init()
{
	glClearColor(0.529, 0.808, 0.922, 0.0); //sets the clear colour to sky blue
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

	myShader = new CShader();
	//if(!myShader->CreateShaderProgram("BasicView", "glslfiles/basicTransformationsWithDisplacement.vert", "glslfiles/basicTransformationsWithDisplacement.frag"))
	if (!myShader->CreateShaderProgram("BasicView", "glslfiles/basicTransformations.vert", "glslfiles/basicTransformations.frag"))
	{
		cout << "failed to load shader" << endl;
	}

	myBasicShader = new CShader();
	if (!myBasicShader->CreateShaderProgram("Basic", "glslfiles/basic.vert", "glslfiles/basic.frag"))
	{
		cout << "failed to load shader" << endl;
	}

	glUseProgram(myShader->GetProgramObjID());  // use the shader

	glEnable(GL_TEXTURE_2D);

	//Initialise ferris wheel
	ferrisWheel = FerrisWheel(objLoader, myShader);

	//Load ground
	if (objLoader.LoadModel("FerrisWheel/ground.obj"))//returns true if the model is loaded
	{
		ground.ConstructModelFromOBJLoader(objLoader);
		ground.InitVBO(myShader);
	}
	else
	{
		std::cout << " model failed to load " << std::endl;
	}

	//Load lightTestObj
	if (objLoader.LoadModel("FerrisWheel/lightTestObj.obj"))//returns true if the model is loaded
	{
		lightTestObj.ConstructModelFromOBJLoader(objLoader);
		lightTestObj.InitVBO(myShader);
	}
	else
	{
		std::cout << " model failed to load " << std::endl;
	}

	currentCam = &freeCam;
	groundCam.setPitch(20.0f);
	groundCam.setYaw(-100.0f);
	rideCam.setYaw(180.0f);

	/*
	mySphere.setCentre(0, 0, 0);
	mySphere.setRadius(8);
	mySphere.constructGeometry(myBasicShader, 16);

	staticSphere.setCentre(20, 5, 0);
	staticSphere.setRadius(8);
	staticSphere.constructGeometry(myBasicShader, 16);
	*/
}

void special(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		Left = true;
		break;
	case GLUT_KEY_RIGHT:
		Right = true;
		break;
	case GLUT_KEY_UP:
		Up = true;
		break;
	case GLUT_KEY_DOWN:
		Down = true;
		break;
	case GLUT_KEY_HOME:
		ferrisWheel.on(deltaTime);
		break;
	case GLUT_KEY_END:
		ferrisWheel.off();
		break;
	case GLUT_KEY_INSERT:
		if (viewCollisions)
			viewCollisions = false;
		else
			viewCollisions = true;
		break;
	}
}

void specialUp(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		Left = false;
		break;
	case GLUT_KEY_RIGHT:
		Right = false;
		break;
	case GLUT_KEY_UP:
		Up = false;
		break;
	case GLUT_KEY_DOWN:
		Down = false;
		break;
	}
}

void keyFunc(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 97: //a = left
		a = true;
		break;
	case 100: //d = right
		d = true;
		break;
	case 119: //w = move away from camera
		w = true;
		break;
	case 115: //s = move towards camera
		s = true;
		break;
	case 49: //1 = free camera
		currentCam = &freeCam;
		break;
	case 50: //2 = ground camera
		currentCam = &groundCam;
		break;
	case 51: //3 = ride camera
		currentCam = &rideCam;
		break;
	case 45:
		ferrisWheel.adjustMaxSpeed('d');
		break;
	case 61:
		ferrisWheel.adjustMaxSpeed('u');
		break;
	}
}

void keyFuncUp(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 97: //a = left
		a = false;
		break;
	case 100: //d = right
		d = false;
		break;
	case 119: //w = move away from camera
		w = false;
		break;
	case 115: //s = move towards camera
		s = false;
		break;
	}
}

void processKeys()
{
	//Camera rotations
	if (Left)
	{
		currentCam->rotate('l', deltaTime);
	}
	if (Right)
	{
		currentCam->rotate('r', deltaTime);
	}
	if (Up)
	{
		currentCam->rotate('u', deltaTime);
	}
	if (Down)
	{
		currentCam->rotate('d', deltaTime);
	}

	//Camera strafing (free camera only)
	if (currentCam == &freeCam && a)
	{
		freeCam.move('a', deltaTime);
	}
	if (currentCam == &freeCam && d)
	{
		freeCam.move('d', deltaTime);
	}
	if (currentCam == &freeCam && w)
	{
		freeCam.move('w', deltaTime);
	}
	if (currentCam == &freeCam && s)
	{
		freeCam.move('s', deltaTime);
	}
}

void idle()
{
	processKeys();
	glutPostRedisplay();
}
/**************** END OPENGL FUNCTIONS *************************/

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(screenWidth, screenHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Ferris Wheel Simulator");

	//Initialise glew
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cout << " GLEW ERROR" << endl;
	}

	//OpenGL version
	int OpenGLVersion[2];
	glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);
	cout << OpenGLVersion[0] << " " << OpenGLVersion[1] << endl;

	//Initialise function
	init();

	//Glut functions
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutSpecialFunc(special);
	glutSpecialUpFunc(specialUp);
	glutKeyboardFunc(keyFunc);
	glutKeyboardUpFunc(keyFuncUp);
	glutIdleFunc(idle);

	//Main loop
	glutMainLoop();

	return 0;
}
