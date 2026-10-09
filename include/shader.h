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
#include <unordered_map>

using namespace std;

class Shader
{
public:
    unsigned int ID{0};

    Shader(const char* vertexPath, const char* fragmentPath)
    {
        const string vertexCode = readFile(vertexPath);
        const string fragmentCode = readFile(fragmentPath);

        const unsigned int vertexShader = compile(GL_VERTEX_SHADER, vertexCode, vertexPath);
        unsigned int fragmentShader=0;
        try { fragmentShader=compile(GL_FRAGMENT_SHADER,fragmentCode,fragmentPath); }
        catch(...) { glDeleteShader(vertexShader);throw; }

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
            glDeleteProgram(ID);ID=0;
            throw runtime_error(string("Shader program link failed:\n") + infoLog);
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

    void setVec3(const string& name, const glm::vec3& value) const
    {
        glUniform3fv(location(name), 1, glm::value_ptr(value));
    }

    void setVec2(const string& name, const glm::vec2& value) const
    {
        glUniform2fv(location(name), 1, glm::value_ptr(value));
    }

    void setBool(const string& name, bool value) const
    {
        glUniform1i(location(name), value ? 1 : 0);
    }

    void setInt(const string& name, int value) const
    {
        glUniform1i(location(name), value);
    }

    void setFloat(const string& name, float value) const
    {
        glUniform1f(location(name), value);
    }

    void setMat4(const string& name, const glm::mat4& value) const
    {
        glUniformMatrix4fv(
            location(name),
            1,
            GL_FALSE,
            glm::value_ptr(value));
    }

private:
    mutable unordered_map<string,int> uniformLocations_;
    int location(const string& name) const
    {
        const auto found=uniformLocations_.find(name);
        if(found!=uniformLocations_.end())return found->second;
        const int value=glGetUniformLocation(ID,name.c_str());uniformLocations_.emplace(name,value);return value;
    }
    static string readFile(const char* path)
    {
        ifstream file(path);
        if (!file)
            throw runtime_error(string("Could not open shader file: ") + path);

        stringstream stream;
        stream << file.rdbuf();
        return stream.str();
    }

    static unsigned int compile(unsigned int type, const string& source, const char* path)
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
            throw runtime_error(string("Shader compilation failed for ") + path + ":\n" + infoLog);
        }
        return shader;
    }
};

#endif
