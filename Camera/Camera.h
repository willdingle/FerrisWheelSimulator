#include "..\glm\glm.hpp"
#include "..\glm\gtc\matrix_transform.hpp"
#include "..\glm\gtc\type_ptr.hpp"
#include "..\glm\gtc\matrix_inverse.hpp"

#include "..\GL\glew.h"

#include "..\shaders\Shader.h"

class Camera
{
private:
	glm::vec3 pos, front, up, dir;
	float pitch, yaw, speed;

public:
	Camera(glm::vec3 setPos)
	{
		pos = setPos;
		front = glm::vec3(0.0f, 0.0f, -1.0f);
		up = glm::vec3(0.0f, 1.0f, 0.0f);
		dir = glm::vec3(0.0f, 0.0f, 0.0f);
		pitch = 0.0f;
		yaw = -90.0f;
		speed = 0.08f;
	};

	glm::mat4 calcMatrix();
	void render(CShader* shader);
	void render(CShader* shader, glm::mat4 matrix);
	void move(char dir);
	void rotate(char dir);
	void setPitch(float setPitch);
	void setYaw(float setYaw);
	void setPos(glm::vec3 setPos)
	{
		pos = setPos;
	}
};