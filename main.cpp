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

CShader* myShader;  ///shader object 
CShader* myBasicShader;

//MODEL LOADING
#include "3DStruct\threeDModel.h"
#include "Obj\OBJLoader.h"

float amount = 0;
float temp = 0.002f;

CThreeDModel stand, moving, ground;
CThreeDModel models[] = { stand, moving, ground };
CThreeDModel Bcarriage, BLcarriage, BRcarriage, Lcarriage, Rcarriage, Tcarriage, TLcarriage, TRcarriage;
CThreeDModel carriages[] = {Bcarriage, BLcarriage, BRcarriage, Lcarriage, Rcarriage, Tcarriage, TLcarriage, TRcarriage};
string names[] = { "stand.obj", "moving.obj", "ground.obj"};
string carriageNames[] = { "Bcarriage.obj", "BLcarriage.obj", "BRcarriage.obj", "Lcarriage.obj", "Rcarriage.obj", "Tcarriage.obj", "TLcarriage.obj", "TRcarriage.obj" };
glm::vec3 carriagePositions[] = {
	glm::vec3(0, 6.25, 0),
	glm::vec3(-4, 8, 0),
	glm::vec3(4, 8, 0),
	glm::vec3(-5.75, 12, 0),
	glm::vec3(5.75, 12, 0),
	glm::vec3(0, 17.75, 0),
	glm::vec3(-4, 16, 0),
	glm::vec3(4, 16, 0)
};
COBJLoader objLoader;	//this object is used to load the 3d models.
///END MODEL LOADING

//Camera and view
#include "Camera/Camera.h"
Camera freeCam = Camera(glm::vec3(0.0f, 12.0f, 25.0f));
Camera groundCam = Camera(glm::vec3(5.0f, 1.0f, 20.0f));
Camera *currentCam;
glm::mat4 ProjectionMatrix; // matrix for the orthographic projection

//Material properties
float Material_Ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
float Material_Diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float Material_Specular[4] = { 1.0f,1.0f,1.0f,1.0f };
float Material_Shininess = 50;

//Light Properties
float Light_Ambient_And_Diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float Light_Specular[4] = { 1.0f,1.0f,1.0f,1.0f };
float LightPos[4] = { 50.0f, 50.0f, 50.0f, 0.0f };

//Screen size
int screenWidth = 1280, screenHeight = 720;

//Booleans for key presses
bool Left = false;
bool Right = false;
bool Up = false;
bool Down = false;
bool Home = false;
bool End = false;
bool o = false;
bool p = false;
bool a = false;
bool d = false;
bool w = false;
bool s = false;

//Movement speed
float ferrisSpeed = 0.0004f;

//Sphere
#include "Sphere\Sphere.h";
Sphere mySphere, staticSphere;

//OPENGL FUNCTION PROTOTYPES
void display();				//called in winmain to draw everything to the screen
void reshape(int width, int height);				//called when the window is resized
void init();				//called in winmain when the program starts.
void processKeys();         //called in winmain to process keyboard input
void idle();		//idle function
void switchCam(unsigned char key);

/*************    START OF OPENGL FUNCTIONS   ****************/
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(myShader->GetProgramObjID());  // use the shader

	//Part for displacement shader.
	amount += temp;
	if (amount > 1.0f || amount < -1.5f)
		temp = -temp;
	//amount = 0;
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "displacement"), amount);

	//translation and rotation for view
	glm::mat4 viewingMatrix = currentCam->calcMatrix();
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ViewMatrix"), 1, GL_FALSE, &viewingMatrix[0][0]);

	//Set the projection matrix in the shader
	GLuint projMatLocation = glGetUniformLocation(myShader->GetProgramObjID(), "ProjectionMatrix");
	glUniformMatrix4fv(projMatLocation, 1, GL_FALSE, &ProjectionMatrix[0][0]);

	//Light properties
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "LightPos"), 1, LightPos);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "light_ambient"), 1, Light_Ambient_And_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "light_diffuse"), 1, Light_Ambient_And_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "light_specular"), 1, Light_Specular);

	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "material_ambient"), 1, Material_Ambient);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "material_diffuse"), 1, Material_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "material_specular"), 1, Material_Specular);
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "material_shininess"), Material_Shininess);

	//Stand
	glm::mat4 ModelMatrix = glm::mat4(1.0f);
	glm::mat4 ModelViewMatrix = viewingMatrix * ModelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glm::mat3 NormalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	models[0].DrawElementsUsingVBO(myShader);

	//Moving
	ferrisSpeed += 0.0004f;
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 12, 0));
	ModelMatrix = glm::rotate(ModelMatrix, ferrisSpeed, glm::vec3(0, 0, 1.0));
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0, -12, 0));
	ModelViewMatrix = viewingMatrix * ModelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	NormalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	models[1].DrawElementsUsingVBO(myShader);

	//Carriages
	glm::mat4 CarriageMatrix = glm::mat4(1.0f);
	glm::vec3 inverseCarPosition = glm::vec3(0, 0, 0);
	for (int i = 0; i < 8; i++)
	{
		CarriageMatrix = glm::translate(glm::mat4(1.0f), carriagePositions[i]);
		CarriageMatrix = glm::rotate(CarriageMatrix, -ferrisSpeed, glm::vec3(0, 0, 1.0));
		inverseCarPosition.x = -carriagePositions[i].x;
		inverseCarPosition.y = -carriagePositions[i].y;
		CarriageMatrix = glm::translate(CarriageMatrix, inverseCarPosition);
		ModelViewMatrix = viewingMatrix * ModelMatrix * CarriageMatrix;
		glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
		NormalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
		glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
		carriages[i].DrawElementsUsingVBO(myShader);
	}

	//Ground
	ModelMatrix = glm::mat4(1.0f);
	ModelViewMatrix = viewingMatrix * ModelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	NormalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	models[2].DrawElementsUsingVBO(myShader);

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
	glClearColor(0.529, 0.808, 0.922, 0.0);						//sets the clear colour to sky blue
	//glClear(GL_COLOR_BUFFER_BIT) in the display function
	//will clear the buffer to this colour
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

	//Load all models except carriages
	for (int i = 0; i < 3; i++)
	{
		if (objLoader.LoadModel("FerrisWheel/" + names[i]))//returns true if the model is loaded
		{
			models[i].ConstructModelFromOBJLoader(objLoader);
			models[i].InitVBO(myShader);
		}
		else
		{
			cout << " model failed to load " << endl;
		}
	}

	//Load carriages
	for (int i = 0; i < 8; i++)
	{
		if (objLoader.LoadModel("FerrisWheel/" + carriageNames[i]))//returns true if the model is loaded
		{
			carriages[i].ConstructModelFromOBJLoader(objLoader);
			carriages[i].InitVBO(myShader);
		}
		else
		{
			cout << " model failed to load " << endl;
		}
	}

	currentCam = &freeCam;

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
		Home = true;
		break;
	case GLUT_KEY_END:
		End = true;
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
	case GLUT_KEY_HOME:
		Home = false;
		break;
	case GLUT_KEY_END:
		End = false;
		break;
	}
}

void keyFunc(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 111: //o = y up
		o = true;
		break;
	case 112: //p = y down
		p = true;
		break;
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
	case 49:
		break;
	}
}

void keyFuncUp(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 111: //o = y up
		o = false;
		break;
	case 112: //p = y down
		p = false;
		break;
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
	case 49:
		currentCam = &freeCam;
		break;
	case 50:
		currentCam = &groundCam;
		break;
	}
}

void processKeys()
{
	//Camera rotations
	if (Left)
	{
		freeCam.rotate('l');
	}
	if (Right)
	{
		freeCam.rotate('r');
	}
	if (Up)
	{
		freeCam.rotate('u');
	}
	if (Down)
	{
		freeCam.rotate('d');
	}

	//Camera strafing
	if (a)
	{
		freeCam.move('a');
	}
	if (d)
	{
		freeCam.move('d');
	}
	if (w)
	{
		freeCam.move('w');
	}
	if (s)
	{
		freeCam.move('s');
	}
}

void switchCam(unsigned char key)
{
	switch (key)
	{
		
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

	//This initialises glew - it must be called after the window is created.
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cout << " GLEW ERROR" << endl;
	}

	//Check the OpenGL version being used
	int OpenGLVersion[2];
	glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);
	cout << OpenGLVersion[0] << " " << OpenGLVersion[1] << endl;

	//initialise the objects for rendering
	init();

	//Specify glut functions
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutSpecialFunc(special);
	glutSpecialUpFunc(specialUp);
	glutKeyboardFunc(keyFunc);
	glutKeyboardUpFunc(keyFuncUp);
	glutIdleFunc(idle);

	//starts the main loop. Program loops and calls callback functions as appropriate.
	glutMainLoop();

	return 0;
}
