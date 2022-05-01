
#include <GLM/glm.hpp> // This is the main GLM header
#include <GLM/gtc/matrix_transform.hpp> // This one lets us use matrix transformations
#include <GLM/gtc/type_ptr.hpp> // This one gives us access to a utility function which makes sending data to OpenGL nice and easy
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <SDL/SDL.h>
#include "glew.h"

class Shader
{
public:
	unsigned int id;
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr)
	{
		std::string vertexCode(vertexPath);
		std::string fragmentCode(fragmentPath);


		//Upload Vertex Shader
		std::ifstream vertFile(vertexCode);
		char* vShaderText = NULL;

		if (vertFile.is_open())
		{
			// Find out how many characters are in the file
			vertFile.seekg(0, vertFile.end);
			int length = (int)vertFile.tellg();
			vertFile.seekg(0, vertFile.beg);

			// Create our buffer
			vShaderText = new char[length];

			// Transfer data from file to buffer
			vertFile.read(vShaderText, length);

			// Check it reached the end of the file
			if (!vertFile.eof())
			{
				vertFile.close();
				std::cerr << "WARNING: could not read vertex shader from file: " << vertexCode << std::endl;
				return;
			}

			// Find out how many characters were actually read
			length = (int)vertFile.gcount();

			// Needs to be NULL-terminated
			vShaderText[length - 1] = 0;

			vertFile.close();
		}
		else
		{
			std::cerr << "WARNING: could not open vertex shader from file: " << vertexCode << std::endl;
			return;
		}

		//Upload Fragment Shader
		std::ifstream fragFile(fragmentCode);
		char* fShaderText = NULL;

		if (fragFile.is_open())
		{
			// Find out how many characters are in the file
			fragFile.seekg(0, fragFile.end);
			int length = (int)fragFile.tellg();
			fragFile.seekg(0, fragFile.beg);

			// Create our buffer
			fShaderText = new char[length];

			// Transfer data from file to buffer
			fragFile.read(fShaderText, length);

			// Check it reached the end of the file
			if (!fragFile.eof())
			{
				fragFile.close();
				std::cerr << "WARNING: could not read fragment shader from file: " << fragmentCode << std::endl;
				return;
			}

			// Find out how many characters were actually read
			length = (int)fragFile.gcount();

			// Needs to be NULL-terminated
			fShaderText[length - 1] = 0;

			fragFile.close();
		}
		else
		{
			std::cerr << "WARNING: could not open fragment shader from file: " << fragmentCode << std::endl;
			return;
		}

		//Upload Geometry Shader
		
		char* gShaderText = NULL;

		if (geometryPath != nullptr)
		{
			std::string geometryCode(geometryPath);
			std::ifstream geometryFile(geometryCode);
			
			if (geometryFile.is_open())
			{
				// Find out how many characters are in the file
				geometryFile.seekg(0, geometryFile.end);
				int length = (int)geometryFile.tellg();
				geometryFile.seekg(0, geometryFile.beg);

				// Create our buffer
				gShaderText = new char[length];

				// Transfer data from file to buffer
				geometryFile.read(gShaderText, length);

				// Check it reached the end of the file
				if (!geometryFile.eof())
				{
					geometryFile.close();
					std::cerr << "WARNING: could not read fragment shader from file: " << geometryCode << std::endl;
					return;
				}

				// Find out how many characters were actually read
				length = (int)geometryFile.gcount();

				// Needs to be NULL-terminated
				gShaderText[length - 1] = 0;

				geometryFile.close();
			}
			else
			{
				std::cerr << "WARNING: could not open geometry shader from file: " << geometryCode << std::endl;
				return;
			}
		}

		// 2. Compile the Shaders
		unsigned int vertex, fragment, geometry;

		//Vertex Shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderText, NULL);
		glCompileShader(vertex);
		if (!CheckShaderCompiled(vertex))
		{
			std::cerr << "ERROR: failed to compile vertex shader" << std::endl;
			return;
		}
		errorCheck(vertex, "VERTEX");

		//Fragment Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderText, NULL);
		glCompileShader(fragment);
		if (!CheckShaderCompiled(fragment))
		{
			std::cerr << "ERROR: failed to compile fragment shader" << std::endl;
			return;
		}
		errorCheck(fragment, "FRAGMENT");

		//Geometry Shader
		if (geometryPath != nullptr)
		{
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &gShaderText, NULL);
			glCompileShader(geometry);
			if (!CheckShaderCompiled(geometry))
			{
				std::cerr << "ERROR: failed to compile geometry shader" << std::endl;
				return;
			}
			errorCheck(geometry, "GEOMETRY");
		}
		

		//Shader Program
		id = glCreateProgram();
		glAttachShader(id, vertex);
		glAttachShader(id, fragment);
		if (geometryPath != nullptr)
		{
			glAttachShader(id, geometry);
		}
		glLinkProgram(id);
		errorCheck(id, "PROGRAM");

		//Check if successfully linked.
		GLint linked;
		glGetProgramiv(id, GL_LINK_STATUS, &linked);
		if (!linked)
		{
			GLsizei len;
			glGetProgramiv(id, GL_INFO_LOG_LENGTH, &len);

			GLchar* log = new GLchar[len + 1];
			glGetProgramInfoLog(id, len, &len, log);
			std::cerr << "ERROR: Shader linking failed: " << log << std::endl;
			delete[] log;

			return;
		}

		//Delete the shaders as they are now linked to our program and no longer neeeded
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		if (geometryPath != nullptr)
		{
			glDeleteShader(geometry);
		}

	}

	void use()
	{
		glUseProgram(id);
	}

	// utility uniform functions
	// ------------------------------------------------------------------------
	void setBool(const std::string& name, bool value) const
	{
		glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
	}
	// ------------------------------------------------------------------------
	void setInt(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(id, name.c_str()), value);
	}
	// ------------------------------------------------------------------------
	void setFloat(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(id, name.c_str()), value);
	}
	// ------------------------------------------------------------------------
	void setVec2(const std::string& name, const glm::vec2& value) const
	{
		glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
	}
	void setVec2(const std::string& name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
	}
	// ------------------------------------------------------------------------
	void setVec3(const std::string& name, const glm::vec3& value) const
	{
		glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
	}
	void setVec3(const std::string& name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
	}
	// ------------------------------------------------------------------------
	void setVec4(const std::string& name, const glm::vec4& value) const
	{
		glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
	}
	void setVec4(const std::string& name, float x, float y, float z, float w)
	{
		glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w);
	}
	// ------------------------------------------------------------------------
	void setMat2(const std::string& name, const glm::mat2& mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	void setMat3(const std::string& name, const glm::mat3& mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	void setMat4(const std::string& name, const glm::mat4& mat) const
	{
		int location = glGetUniformLocation(id, name.c_str());

		glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
	}

private:
	void errorCheck(GLuint shader, std::string type)
	{
		GLint success;
		GLchar infoLog[1024];

		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}

		}

		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
	}

	//Checks if the shader compiles or not
	bool CheckShaderCompiled(GLuint shader)
	{
		GLint compiled;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled)
		{
			GLsizei len;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

			// OpenGL will store an error message as a string that we can retrieve and print
			GLchar* log = new GLchar[len + 1];
			glGetShaderInfoLog(shader, len, &len, log);
			std::cerr << "ERROR: Shader compilation failed: " << log << std::endl;
			delete[] log;

			return false;
		}
		return true;
	}
};



