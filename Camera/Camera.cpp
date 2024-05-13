#include "Camera.h"
#include <iostream>

glm::mat4 Camera::calcMatrix()
{
	dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	dir.y = sin(glm::radians(pitch));
	dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(dir);
	return glm::lookAt(pos, pos + front, up);
}

void Camera::render(CShader* shader)
{
	glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramObjID(), "ViewMatrix"), 1, GL_FALSE, &calcMatrix()[0][0]);
}

void Camera::constructCollisionBoxes(CShader* shader, float ferrisAngle, glm::vec3 carriagePositions[])
{
	
	baseCollision.constructGeometry(shader, -11.5f, 0.0f, -6.5f, 11.5f, 2.5f, 6.5f); //Base collision
	standCollision.constructGeometry(shader, -6.5f, 1.0f, -2.5f, 6.5f, 14.5f, 2.5f); //Stand collision
	movingCollision.constructGeometry(shader, -10.5f, 2.0f, -0.25f, 10.5f, 22.5f, 4.8f); //Moving wheel collision

	//Carriage collisions
	for (int i = 0; i < 8; i++)
	{
		glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 12, 0));
		ModelMatrix = glm::rotate(ModelMatrix, ferrisAngle, glm::vec3(0, 0, 1.0));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(carriagePositions[i].x, carriagePositions[i].y - 12.0f, 5.0f));
		glm::vec3 carriagePos = glm::vec3(ModelMatrix[3][0], ModelMatrix[3][1], ModelMatrix[3][2]);

		carriageCollisions[i].constructGeometry(shader, carriagePos.x - 3.5f, carriagePos.y - 2.0, carriagePos.z - 3.5f, carriagePos.x + 3.5f, carriagePos.y + 5.0f, carriagePos.z + 4.5f);
	}

	//Light test object collision
	lightTestCollision.constructGeometry(shader, -27.5f, 0.0f, -6.5f, -22.5f, 21.5f, 16.5f);

}

void Camera::viewCollisionBoxes(CShader* shader, float ferrisAngle, glm::vec3 carriagePositions[])
{
	//Test collision for whole ride
		//ferrisCollision.constructGeometry(shader, -10.75f, 0.0f, -5.0f, 10.75f, 22.25f, 6.5f);
		//ferrisCollision.render();

	//Base collision
	baseCollision.render();

	//Stand collision
	standCollision.render();

	//Turning wheel collision
	movingCollision.render();

	//Carriage collisions
	for (int i = 0; i < 8; i++)
	{
		glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 12, 0));
		ModelMatrix = glm::rotate(ModelMatrix, ferrisAngle, glm::vec3(0, 0, 1.0));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(carriagePositions[i].x, carriagePositions[i].y - 12.0f, 5.0f));
		glm::vec3 carriagePos = glm::vec3(ModelMatrix[3][0], ModelMatrix[3][1], ModelMatrix[3][2]);

		//The line below causes memory leaks. The collisions for the carriages are still updated every frame in the move() method, however
		//carriageCollisions[i].constructGeometry(shader, carriagePos.x - 3.5f, carriagePos.y - 2.0, carriagePos.z - 3.5f, carriagePos.x + 3.5f, carriagePos.y + 5.0f, carriagePos.z + 4.5f);
		carriageCollisions[i].render();
	}

	//Light test object collision
	lightTestCollision.render();
}

void Camera::move(char dir, int delta, float ferrisAngle, glm::vec3 carriagePositions[])
{
	glm::vec3 oldPos = pos;
	switch (dir)
	{
	case 'w':
		pos += (speed * delta) * front;
		break;
	case 's':
		pos -= (speed * delta) * front;
		break;
	case 'a':
		pos -= glm::normalize(glm::cross(front, up)) * (speed * delta);
		break;
	case 'd':
		pos += glm::normalize(glm::cross(front, up)) * (speed * delta);
		break;
	}

	//Ground collision
	if (pos.y < 1.5f)
	{
		pos.y = 1.5f;
	}

	//Base collision
	if (
		pos.x > -11.5f &&
		pos.x < 11.5f &&
		pos.y > 0.0f &&
		pos.y < 2.5f &&
		pos.z > -6.5f &&
		pos.z < 6.5f
		)
		pos = oldPos;
	
	//Stand collision
	if (
		pos.x > -6.5f &&
		pos.x < 6.5f &&
		pos.y > 1.0f &&
		pos.y < 14.0f &&
		pos.z > -2.5f &&
		pos.z < 2.5f
		)
		pos = oldPos;

	//Turning wheel collision
	if (
		pos.x > -10.5f &&
		pos.x < 10.5f &&
		pos.y > 2.0f &&
		pos.y < 22.5f &&
		pos.z > -0.25f &&
		pos.z < 4.8f
		)
		pos = oldPos;

	//Carriage collisions
	for (int i = 0; i < 8; i++)
	{
		glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 12, 0));
		ModelMatrix = glm::rotate(ModelMatrix, ferrisAngle, glm::vec3(0, 0, 1.0));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(carriagePositions[i].x, carriagePositions[i].y - 12.0f, 5.0f));
		glm::vec3 carriagePos = glm::vec3(ModelMatrix[3][0], ModelMatrix[3][1], ModelMatrix[3][2]);

		if (
			pos.x > carriagePos.x - 3.5f &&
			pos.x < carriagePos.x + 3.5f &&
			pos.y > carriagePos.y - 2.0f &&
			pos.y < carriagePos.y + 5.0f &&
			pos.z > carriagePos.z - 3.5f &&
			pos.z < carriagePos.z + 4.5f
			)
			pos = oldPos;
	}

	//Light test object collision
	if (
		pos.x > -27.5f &&
		pos.x < -22.5f &&
		pos.y > 0.0f &&
		pos.y < 21.5f &&
		pos.z > -6.5f &&
		pos.z < 16.5f
		)
		pos = oldPos;
}

void Camera::rotate(char dir, int delta)
{
	switch (dir)
	{
	case 'u':
		pitch += speed * delta;
		break;
	case 'd':
		pitch -= speed * delta;
		break;
	case 'l':
		yaw -= speed * delta;
		break;
	case 'r':
		yaw += speed * delta;
		break;
	}
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
}

void Camera::setPitch(float setPitch)
{
	pitch = setPitch;
}

void Camera::setYaw(float setYaw)
{
	yaw = setYaw;
}