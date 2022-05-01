#define glCheckError() glCheckError_(__FILE__, __LINE__) 

#include "Cube.h"


// The GLM library contains vector and matrix functions and classes for us to use
// They are designed to easily work with OpenGL!
#include <GLM/glm.hpp> // This is the main GLM header
#include <GLM/gtc/matrix_transform.hpp> // This one lets us use matrix transformations
#include <GLM/gtc/type_ptr.hpp> // This one gives us access to a utility function which makes sending data to OpenGL nice and easy
#include <iostream>
#include <vector>


class Shader;
class Scene
{
public:

	Scene();
	~Scene();

	void ChangeCameraAngleX(float value) { _cameraAngleX += value; }
	void ChangeCameraAngleY(float value) { _cameraAngleY += value; }

	void Update(float deltaTs);


	void Draw(Shader shader, int SHADOW_WIDTH, int SHADOW_HEIGHT = 640);


protected:

	//Cube Model
	Cube _cubeModel;

	// Model matrices for three cubes
	glm::mat4 _modelMatrixCube1;
	glm::mat4 _modelMatrixCube2;
	glm::mat4 _modelMatrixCube3;

	// All cubes share the same viewing matrix - this defines the camera's orientation and position
	glm::mat4 _viewMatrix;

	// The projection matrix defines the camera's view (like its lens)
	glm::mat4 _projMatrix;

	//Light position
	glm::vec3 lightPos;

	//Light Projection and view 
	glm::mat4 lightProjection;
	glm::mat4 lightView;

	//Light Model Matrix
	glm::mat4 lightModelMatrix;
	
	//Light space matrix that stores the light projection, view and model. 
	glm::mat4 lightSpaceMatrix;
	

	//Shadow projection
	glm::mat4 shadowProj;

	//Vector to store the shadow directions for the depth map.
	std::vector<glm::mat4> shadowTransforms;

	// Angle of rotation for our cube
	float _cube1Angle;
	float _cube2Angle;
	float _cameraAngleX, _cameraAngleY;
	float near_plane, far_plane;
	float aspect;


	// These are for storing the Uniform locations of shader variables
	// We need these so we can send Uniform data to them
	GLint _shaderModelMatLocation;
	GLint _shaderViewMatLocation;
	GLint _shaderProjMatLocation;
	GLint _lightSpaceMatrixLocation;
	GLint _textureLocation;
	GLint _nearplane;
	GLint _farplane;

	GLint _shaderDiffuseColLocation, _shaderEmissiveColLocation;
	GLint _shaderWSLightPosLocation;

};
