#include "Camera.h"

glm::mat4 Camera::calcMatrix()
{
	dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	dir.y = sin(glm::radians(pitch));
	dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(dir);
	return glm::lookAt(pos, pos + front, up);
}

void Camera::move(char dir)
{
	switch (dir)
	{
	case 'w':
		pos += speed * front;
		break;
	case 's':
		pos -= speed * front;
		break;
	case 'a':
		pos -= glm::normalize(glm::cross(front, up)) * speed;
		break;
	case 'd':
		pos += glm::normalize(glm::cross(front, up)) * speed;
		break;
	}
}

void Camera::render(CShader* shader)
{
	glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramObjID(), "ViewMatrix"), 1, GL_FALSE, &calcMatrix()[0][0]);
}

void Camera::render(CShader* shader, glm::mat4 matrix)
{
	glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramObjID(), "ViewMatrix"), 1, GL_FALSE, &matrix[0][0]);
}

void Camera::rotate(char dir)
{
	switch (dir)
	{
	case 'u':
		pitch += speed;
		break;
	case 'd':
		pitch -= speed;
		break;
	case 'l':
		yaw -= speed;
		break;
	case 'r':
		yaw += speed;
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