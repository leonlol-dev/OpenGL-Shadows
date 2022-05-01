
#include <iostream>
#include <fstream>
#include <string>

#include "Scene.h"
#include "Shader.h"

Scene::Scene()
{


	_cube1Angle = 0.0f;
	_cube2Angle = 0.0f;

	_cameraAngleX = 0.0f, _cameraAngleY = 0.0f;


	//_modelMatrixCube1;
	_modelMatrixCube2 = glm::scale(glm::translate(glm::mat4(1.0f),glm::vec3(1.0f,0.0f,0.0f)),glm::vec3(0.1f,0.1f,0.1f));
	_modelMatrixCube3 = glm::scale(glm::translate(glm::mat4(1.0f),glm::vec3(0.0f,-1.0f,0.0f)),glm::vec3(2.0f,0.1f,2.0f));
	
	// Set up the viewing matrix
	// This represents the camera's orientation and position
	_viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0,0,-3.5f) );
	

	// Set up a projection matrix
	_projMatrix = glm::perspective(45.0f, 1.0f, 0.1f, 10.0f);


	//Setting up light matrix
	near_plane = 0.1f, far_plane = 10.0f;
	lightPos = glm::vec3(-1.0f, 4.0f, 1.0f);
	lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	lightView = glm::lookAt
				(lightPos,                           
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f));
	lightModelMatrix = glm::mat4(1.0f);

	//The Light Space Matrix.
	lightSpaceMatrix = lightModelMatrix * lightProjection * lightView;


}

Scene::~Scene()
{
}

void Scene::Update( float deltaTs )
{
	_cube1Angle += deltaTs * 0.5f;
	// Limit the loop to 360 degrees
	// This will prevent numerical inaccuracies
	while( _cube1Angle > (3.14159265358979323846 * 2.0) )
	{
		_cube1Angle -= (3.14159265358979323846 * 2.0);
	}

	_cube2Angle += deltaTs * 2.0f;
	while( _cube2Angle > (3.14159265358979323846 * 2.0) )
	{
		_cube2Angle -= (3.14159265358979323846 * 2.0);
	}




	// Update the model matrix with the rotation of the object
	_modelMatrixCube1 = glm::rotate( glm::mat4(1.0f), _cube1Angle, glm::vec3(0,1,0) );
	_modelMatrixCube2 = glm::scale(glm::translate(glm::rotate( glm::mat4(1.0f),-_cube2Angle, glm::vec3(0,1,0) ),glm::vec3(1.0f,0.0f,0.0f)),glm::vec3(0.1f,0.1f,0.1f));

	_viewMatrix = glm::rotate( glm::rotate( glm::translate( glm::mat4(1.0f), glm::vec3(0,0,-3.5f) ), _cameraAngleX, glm::vec3(1,0,0) ), _cameraAngleY, glm::vec3(0,1,0) );
}

void Scene::Draw(Shader shader)
{
		// Activate the shader program
		//glUseProgram( _shaderProgram );

		shader.use();
	
		// We use the small cube's model matrix to transform the light position
		// This means the light will have the position of the small cube
		shader.setVec4("worldSpaceLightPos", _modelMatrixCube2 * glm::vec4(0, 0, 0, 1) );


		// Send view and projection matrices to OpenGL
		shader.setMat4("viewMat", _viewMatrix);
		shader.setMat4("projMat", _projMatrix);
		shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		shader.setFloat("far_plane", far_plane);
		shader.setFloat("near_plane", near_plane);
		shader.setFloat("depthMap", 0);


		/* Draw Cube 1 */

			// Set emissive colour component for cube 1
			shader.setVec3("emissiveColour", 0.0f, 0.0f, 0.0f);
			shader.setMat4("modelMat", _modelMatrixCube1);
			// Set diffuse colour for cube 1
			shader.setVec3("diffuseColour", 1.0f, 0.3f, 0.3f);
				_cubeModel.Draw( );

		

		/* Draw Cube 2 */

			
			shader.setMat4("modelMat", _modelMatrixCube2);
			shader.setVec3("diffuseColour", 0.0f, 0.0f, 0.0f);
			// Set emissive colour component for cubes 2 to be bright so it looks like a light
			shader.setVec3("emissiveColour", 1.0f, 1.0f, 1.0f );
				_cubeModel.Draw( );
		


		/* Draw Cube 3 */

			// Set emissive colour component for cubes 3 to be dark
			shader.setVec3("emissiveColour", 0.0f, 0.0f, 0.0f );
			shader.setMat4("modelMat", _modelMatrixCube3);
			// Set diffuse colour for cube 3
			shader.setVec3("diffuseColour", 0.3f, 0.3f, 1.0f );
				_cubeModel.Draw( );


			//glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Technically we can do this, but it makes no real sense because we must always have a valid shader program to draw geometry
	glUseProgram( 0 );
}


