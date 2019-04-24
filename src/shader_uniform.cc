#include <GL/glew.h>
#include <debuggl.h>
#include <glm/gtc/quaternion.hpp>
#include "shader_uniform.h"

void bindUniform(unsigned loc, int scalar)
{
	glUniform1i(loc, scalar);
}

void bindUniform(unsigned loc, float scalar)
{
	glUniform1f(loc, scalar);
}

void bindUniform(unsigned loc, const glm::vec3& vec)
{
	CHECK_GL_ERROR(glUniform3fv(loc, 1, (const GLfloat*)&vec));
}

void bindUniform(unsigned loc, const glm::vec4& vec)
{
	glUniform4fv(loc, 1, (const GLfloat*)&vec);
}

void bindUniform(unsigned loc, const glm::fquat& vec)
{
	glUniform4fv(loc, 1, (const GLfloat*)&vec);
}

void bindUniform(unsigned loc, const glm::mat4& mat)
{
	glUniformMatrix4fv(loc, 1, GL_FALSE, (const GLfloat*)&mat);
}

void bindUniform(unsigned loc, const glm::mat4* mat)
{
	glUniformMatrix4fv(loc, 1, GL_FALSE, (const GLfloat*)mat);
}

void bindUniform(unsigned loc, const std::vector<float>& scalars)
{
	glUniform1fv(loc, scalars.size(), (const GLfloat*)scalars.data());
}

void bindUniform(unsigned loc, const std::vector<glm::vec3>& array)
{
	glUniform3fv(loc, array.size(), (const GLfloat*)array.data());
}

void bindUniform(unsigned loc, const std::vector<glm::vec4>& array)
{
	glUniform4fv(loc, array.size(), (const GLfloat*)array.data());
}

void bindUniform(unsigned loc, const std::vector<glm::fquat>& array)
{
	glUniform4fv(loc, array.size(), (const GLfloat*)array.data());
}

void bindUniform(unsigned loc, const std::vector<glm::mat4>& array)
{
	glUniformMatrix4fv(loc, array.size(), GL_FALSE, (const GLfloat*)array.data());
}


void TextureCombo::bind(unsigned loc)
{
	// Assign texture object to texture unit
	unsigned tex = texture_source();
	CHECK_GL_ERROR(glActiveTexture(GL_TEXTURE0 + texture_unit));
	CHECK_GL_ERROR(glBindTexture(GL_TEXTURE_2D, tex));

	// Set the OpenGL sampler used by the texture unit
	unsigned sam = sampler_source();
	CHECK_GL_ERROR(glBindSampler(texture_unit, sam));

	// Attach the GLSL sampler to a texture unit
	CHECK_GL_ERROR(glUniform1i(loc, texture_unit));
}

std::shared_ptr<TextureCombo>
make_texture(const std::string& name,
             std::function<unsigned()> sampler_source,
             unsigned texture_unit,
             std::function<unsigned()> texture_source)
{
	auto ret = std::make_shared<TextureCombo>();
	ret->sampler_source = sampler_source;
	ret->texture_unit = texture_unit;
	ret->texture_source = texture_source;
	return ret;
}

