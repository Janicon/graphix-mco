#pragma once
/* Loads shader files and creates shader
*  Vertex and fragment file must have the same name,
*  and saved in "./Shaders/"
*/ 
class ShaderManager {
private:
	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint shaderProgram;
	
	/* Creates vertex shader from the specified file */
	void createVertexShader(std::string name) {
		std::fstream vertSrc("Shaders/" + name + ".vert");
		std::stringstream vertBuff;
		vertBuff << vertSrc.rdbuf();
		std::string vertString = vertBuff.str();
		const char* v = vertString.c_str();

		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &v, NULL);
		glCompileShader(vertexShader);
	}

	/* Creates fragment shader from the specified file */
	void createFragmentShader(std::string name) {
		std::fstream fragSrc("Shaders/" + name + ".frag");
		std::stringstream fragBuff;
		fragBuff << fragSrc.rdbuf();
		std::string fragString = fragBuff.str();
		const char* f = fragString.c_str();

		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &f, NULL);
		glCompileShader(fragmentShader);
	}

public:
	ShaderManager() {}

	ShaderManager(std::string name) {
		createVertexShader(name);
		createFragmentShader(name);

		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);
	}

	/* Getters */
	GLuint getShaderProgram() {
		return shaderProgram;
	}
	unsigned int getUniformLoc(std::string varname) {
		return glGetUniformLocation(shaderProgram, varname.c_str());
	}

	/* Sets managed shader to be active in program */
	void useShaderProgram() {
		glUseProgram(shaderProgram);
	}

	/* Sends uniform int value */
	void sendInt(std::string varname, float value) {
		glUniform1i(glGetUniformLocation(shaderProgram, varname.c_str()), value);
	}

	/* Sends uniform float value */
	void sendFloat(std::string varname, float value) {
		glUniform1f(glGetUniformLocation(shaderProgram, varname.c_str()), value);
	}

	/* Sends uniform vec3 value */
	void sendVec3(std::string varname, glm::vec3 value) {
		glUniform3fv(glGetUniformLocation(shaderProgram, varname.c_str()), 1, glm::value_ptr(value));
	}

	/* Sends uniform vec4 value */
	void sendVec4(std::string varname, glm::vec3 value) {
		glUniform4fv(glGetUniformLocation(shaderProgram, varname.c_str()), 1, glm::value_ptr(value));
	}

	/* Sends uniform mat4 value */
	void sendMat4(std::string varname, glm::mat4 value) {
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, varname.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}
};