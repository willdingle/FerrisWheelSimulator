#include "FerrisWheel.h"

FerrisWheel::FerrisWheel()
{
	angle = 0.0f;
	maxSpeed = 0.0f;
	speed = 0.0f;
	targetSpeed = 0.0f;
	acceleration = 0.0f;
	starting = false;
	stopping = false;
}

FerrisWheel::FerrisWheel(COBJLoader objLoader, CShader* shader)
{
	maxSpeed = 0.004f;
	speed = 0.0f;
	angle = 0.0f;
	targetSpeed = 0.0f;
	acceleration = 0.000004f;
	starting = false;
	stopping = false;

	//Load stand
	if (objLoader.LoadModel("FerrisWheel/stand.obj"))//returns true if the model is loaded
	{
		stand.ConstructModelFromOBJLoader(objLoader);
		stand.InitVBO(shader);
	}
	else
	{
		std::cout << " model failed to load " << std::endl;
	}

	//Load moving
	if (objLoader.LoadModel("FerrisWheel/moving.obj"))//returns true if the model is loaded
	{
		moving.ConstructModelFromOBJLoader(objLoader);
		moving.InitVBO(shader);
	}
	else
	{
		std::cout << " model failed to load " << std::endl;
	}

	//Load carriages
	for (int i = 0; i < 8; i++)
	{
		if (objLoader.LoadModel("FerrisWheel/" + carriageNames[i]))//returns true if the model is loaded
		{
			carriages[i].ConstructModelFromOBJLoader(objLoader);
			carriages[i].InitVBO(shader);
		}
		else
		{
			std::cout << " model failed to load " << std:: endl;
		}
	}
}

void FerrisWheel::render(glm::mat4 viewingMatrix, CShader* shader)
{
	std::cout << "Speed:" << speed << ", Target:" << targetSpeed << std::endl;

	//Stand
	glm::mat4 ModelMatrix = glm::mat4(1.0f);
	glm::mat4 ModelViewMatrix = viewingMatrix * ModelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glm::mat3 NormalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(shader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	stand.DrawElementsUsingVBO(shader);

	//Moving
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 12, 0));
	ModelMatrix = glm::rotate(ModelMatrix, angle, glm::vec3(0, 0, 1.0));
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0, -12, 0));
	ModelViewMatrix = viewingMatrix * ModelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	NormalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(shader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
	moving.DrawElementsUsingVBO(shader);

	//Carriages
	glm::mat4 CarriageMatrix = glm::mat4(1.0f);
	glm::vec3 inverseCarPosition = glm::vec3(0, 0, 0);
	for (int i = 0; i < 8; i++)
	{
		CarriageMatrix = glm::translate(glm::mat4(1.0f), carriagePositions[i]);
		CarriageMatrix = glm::rotate(CarriageMatrix, -angle, glm::vec3(0, 0, 1.0));
		inverseCarPosition.x = -carriagePositions[i].x;
		inverseCarPosition.y = -carriagePositions[i].y;
		CarriageMatrix = glm::translate(CarriageMatrix, inverseCarPosition);
		ModelViewMatrix = viewingMatrix * ModelMatrix * CarriageMatrix;
		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
		NormalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
		glUniformMatrix3fv(glGetUniformLocation(shader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);
		carriages[i].DrawElementsUsingVBO(shader);
	}

	if (speed < targetSpeed)
	{
		speed += acceleration;
		if (speed > targetSpeed)
			speed = targetSpeed;
	}
	if (speed > targetSpeed)
	{
		speed -= acceleration;
		if (speed < targetSpeed)
			speed = targetSpeed;
	}

	angle += speed;
}

void FerrisWheel::adjustMaxSpeed(char dir)
{
	switch (dir)
	{
	case 'u':
		maxSpeed += 0.004f;
		break;
	case 'd':
		maxSpeed += 0.0f;
		break;
	}
}

void FerrisWheel::on()
{
	targetSpeed = maxSpeed;
}

void FerrisWheel::off()
{
	targetSpeed = 0.0f;
}