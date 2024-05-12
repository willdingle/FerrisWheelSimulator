#version 400

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ViewMatrix;

in  vec3 in_Position;  // Position coming in
in  vec2 in_TexCoord;  // texture coordinate coming in
in  vec3 in_Normal;    // vertex normal used for lighting

uniform vec4 LightPos;  // light position
uniform vec4 SecondLightPos; //2nd light pos
uniform vec4 ThirdLightPos;
uniform vec4 FourthLightPos;

out vec2 ex_TexCoord;  // exiting texture coord
out vec3 ex_Normal;    // exiting normal transformed by the normal matrix
out vec3 ex_PositionEye; 
out vec3 ex_LightDir; 
out vec3 ex_Light2Pos;
out vec3 ex_Light3Pos;
out vec3 ex_Light4Pos;

void main(void)
{
	gl_Position = ProjectionMatrix * ModelViewMatrix * vec4(in_Position, 1.0);
	
	ex_TexCoord = in_TexCoord;
		
	ex_Normal = NormalMatrix*in_Normal; 

	ex_PositionEye = vec3((ModelViewMatrix * vec4(in_Position, 1.0)));

	ex_LightDir = vec3(ViewMatrix * LightPos);
	ex_Light2Pos = vec3(ViewMatrix * SecondLightPos);
	ex_Light3Pos = vec3(ViewMatrix * ThirdLightPos);
	ex_Light4Pos = vec3(ViewMatrix * FourthLightPos);
}