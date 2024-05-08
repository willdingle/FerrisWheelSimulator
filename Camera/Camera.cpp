#include "Camera.h"

glm::mat4 Camera::calcMatrix()
{
	dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	dir.y = sin(glm::radians(pitch));
	dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(dir);
	return glm::lookAt(pos, pos + front, up);
}

void Camera::move(char dir, int delta)
{
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
}

void Camera::render(CShader* shader)
{
	glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramObjID(), "ViewMatrix"), 1, GL_FALSE, &calcMatrix()[0][0]);
}

void Camera::render(CShader* shader, glm::mat4 matrix)
{
	glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramObjID(), "ViewMatrix"), 1, GL_FALSE, &matrix[0][0]);
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