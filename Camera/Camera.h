#include "..\glm\glm.hpp"
#include "..\glm\gtc\matrix_transform.hpp"
#include "..\glm\gtc\type_ptr.hpp"
#include "..\glm\gtc\matrix_inverse.hpp"

#include "..\GL\glew.h"

#include "..\shaders\Shader.h"

#include "..\Box\Box.h"

class Camera
{
private:
	glm::vec3 pos, front, up, dir;
	float pitch, yaw, speed;
	CBox ferrisCollision;

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
		ferrisCollision = CBox();
	};

	glm::mat4 calcMatrix();
	void render(CShader* shader);
	void viewCollisionBoxes(CShader* shader, float ferrisAngle);
	void move(char dir, int delta);
	void rotate(char dir, int delta);
	void setPitch(float setPitch);
	void setYaw(float setYaw);
	void setPos(glm::vec3 setPos)
	{
		pos = setPos;
	}
	glm::vec3 getPos()
	{
		return pos;
	}
	glm::vec3 getFront()
	{
		return front;
	}
};