#ifndef SHADER_UNIFORM_H
#define SHADER_UNIFORM_H

#include <GL/glew.h>
#include <debuggl.h>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <functional>
#include <type_traits>
#include <memory>
#include <iostream>
#include <glm/gtx/io.hpp>

void bindUniform(unsigned, int);
void bindUniform(unsigned, float);
void bindUniform(unsigned, const glm::vec3&);
void bindUniform(unsigned, const glm::vec4&);
void bindUniform(unsigned, const glm::fquat&);
void bindUniform(unsigned, const glm::mat4& mat);
void bindUniform(unsigned, const glm::mat4* pmat);

void bindUniform(unsigned, const std::vector<float>&);
void bindUniform(unsigned, const std::vector<glm::vec3>&);
void bindUniform(unsigned, const std::vector<glm::vec4>&);
void bindUniform(unsigned, const std::vector<glm::fquat>&);
void bindUniform(unsigned, const std::vector<glm::mat4>&);

// FIXME: overload bindUniform function to handle new data types.

struct ShaderUniformBase {
	std::string name;

	virtual void bind(unsigned loc) = 0;
};

typedef std::shared_ptr<ShaderUniformBase> ShaderUniformPtr;

template<typename T>
std::ostream& operator<<(std::ostream& stm, const std::vector<T>& obj) {
    stm << "[";
    if (!obj.empty()) {
        for (size_t i = 0 ; i<obj.size()-1 ; ++i) {
            stm << obj[i] << ",";
        }
        stm << obj.back();
    }
    stm << "]";
    return stm;
}

template<typename T>
struct ShaderUniform : public ShaderUniformBase {
	std::function<T()> data_source;

	ShaderUniform(const std::string& name,
	              std::function<T()> func)
	{
		this->name = name;
		this->data_source = func;
	}

	virtual void bind(unsigned loc) override
	{
		CHECK_GL_ERROR(bindUniform(loc, this->data_source()));
	};
};

template<typename T>
std::shared_ptr<ShaderUniformBase>
make_uniform(const std::string& name,
             std::function<T()> func)
{
	// using RC = typename std::remove_const<T>::type;
	// using Bare = typename std::remove_reference<RC>::type;
	return std::make_shared<ShaderUniform<T>>(name, func);
}

struct TextureCombo : public ShaderUniformBase {
	std::function<unsigned()> sampler_source;
	unsigned texture_unit;
	std::function<unsigned()> texture_source;
	virtual void bind(unsigned loc) override;
};

std::shared_ptr<TextureCombo>
make_texture(const std::string& name,
             std::function<unsigned()> sampler_source,
             unsigned texture_unit,
             std::function<unsigned()> texture_source);

#endif
