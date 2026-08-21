#ifndef AMS_AGRO_FARM_SHADER_H
#define AMS_AGRO_FARM_SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

class Shader
{
public:
    unsigned int ID{0};

    Shader(const char* vertexPath, const char* fragmentPath)
    {
        const std::string vertexCode = readFile(vertexPath);
        const std::string fragmentCode = readFile(fragmentPath);

        const unsigned int vertexShader = compile(GL_VERTEX_SHADER, vertexCode, vertexPath);
        const unsigned int fragmentShader = compile(GL_FRAGMENT_SHADER, fragmentCode, fragmentPath);

        ID = glCreateProgram();
        glAttachShader(ID, vertexShader);
        glAttachShader(ID, fragmentShader);
        glLinkProgram(ID);

        int success = 0;
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success)
        {
            char infoLog[1024];
            glGetProgramInfoLog(ID, sizeof(infoLog), nullptr, infoLog);
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            throw std::runtime_error(std::string("Shader program link failed:\n") + infoLog);
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    ~Shader()
    {
        if (ID != 0)
            glDeleteProgram(ID);
    }

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    void use() const
    {
        glUseProgram(ID);
    }

    void setVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
    }

    void setVec2(const std::string& name, const glm::vec2& value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
    }

    void setBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value ? 1 : 0);
    }

    void setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    void setMat4(const std::string& name, const glm::mat4& value) const
    {
        glUniformMatrix4fv(
            glGetUniformLocation(ID, name.c_str()),
            1,
            GL_FALSE,
            glm::value_ptr(value));
    }

private:
    static std::string readFile(const char* path)
    {
        std::ifstream file(path);
        if (!file)
            throw std::runtime_error(std::string("Could not open shader file: ") + path);

        std::stringstream stream;
        stream << file.rdbuf();
        return stream.str();
    }

    static unsigned int compile(unsigned int type, const std::string& source, const char* path)
    {
        const unsigned int shader = glCreateShader(type);
        const char* sourcePointer = source.c_str();
        glShaderSource(shader, 1, &sourcePointer, nullptr);
        glCompileShader(shader);

        int success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char infoLog[1024];
            glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
            glDeleteShader(shader);
            throw std::runtime_error(std::string("Shader compilation failed for ") + path + ":\n" + infoLog);
        }
        return shader;
    }
};

#endif
