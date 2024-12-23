#include "..\glm\glm.hpp"
#include "..\glm\gtc\matrix_transform.hpp"
#include "..\glm\gtc\type_ptr.hpp"
#include "..\glm\gtc\matrix_inverse.hpp"

#include "..\GL\glew.h"

#include "..\shaders\Shader.h"

#include "..\3DStruct\threeDModel.h"

class FerrisWheel
{
private:
	float maxSpeed, speed, targetSpeed, acceleration, angle;
	int prevDelta;
	CThreeDModel stand, moving;
	std::string carriageNames[8] = { "Bcarriage.obj", "BLcarriage.obj", "BRcarriage.obj", "Lcarriage.obj", "Rcarriage.obj", "Tcarriage.obj", "TLcarriage.obj", "TRcarriage.obj" };
	glm::vec3 carriagePositions[8] = {
		glm::vec3(0, 3.25, 0),
		glm::vec3(-6.25, 5.85, 0),
		glm::vec3(6.25, 5.85, 0),
		glm::vec3(-8.75, 12, 0),
		glm::vec3(8.75, 12, 0),
		glm::vec3(0, 20.75, 0),
		glm::vec3(-6.25, 18.15, 0),
		glm::vec3(6.25, 18.15, 0)
	};
	CThreeDModel Bcarriage, BLcarriage, BRcarriage, Lcarriage, Rcarriage, Tcarriage, TLcarriage, TRcarriage;
	CThreeDModel carriages[8] = { Bcarriage, BLcarriage, BRcarriage, Lcarriage, Rcarriage, Tcarriage, TLcarriage, TRcarriage };
	glm::mat4 BcarriageMat;

public:
	FerrisWheel();
	FerrisWheel(COBJLoader objLoader, CShader* shader);
	void render(glm::mat4 viewingMatrix, CShader* shader, int delta);
	void adjustMaxSpeed(char dir);
	void on(int delta);
	void off();
	float getAngle()
	{
		return angle;
	}
	glm::mat4 getBcarriageMat()
	{
		return BcarriageMat;
	}
	glm::vec3* getCarriagePositions()
	{
		return carriagePositions;
	}
};