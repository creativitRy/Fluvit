#include <GL/glew.h>
#include <debuggl.h>
#include <iostream>
#include "texture_to_render.h"

TextureToRender::TextureToRender()
{
}

TextureToRender::~TextureToRender()
{
	if (fb_ < 0)
		return ;
	
	unbind();
	
	glDeleteFramebuffers(1, &fb_);
	glDeleteTextures(1, &tex_);
	glDeleteRenderbuffers(1, &dep_);

	release();
	std::cout<<"Hi"<<std::endl;
}

void TextureToRender::create(int width, int height)
{
	w_ = width;
	h_ = height;
	// FIXME: Create the framebuffer object backed by a texture
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Failed to create framebuffer object as render target" << std::endl;
	} else {
		std::cerr << "Framebuffer ready" << std::endl;
	}
	unbind();
}

void TextureToRender::bind()
{
	// FIXME: Unbind the framebuffer object to GL_FRAMEBUFFER
}

void TextureToRender::unbind()
{
	// FIXME: Unbind current framebuffer object from the render target
}

void TextureToRender::release()
{
	if (fb_ < 0)
		return ;
	
	unbind();
	
	glDeleteFramebuffers(1, &fb_);
	glDeleteTextures(1, &tex_);
	glDeleteRenderbuffers(1, &dep_);

	fb_ = 0;
	tex_ = 0;
	dep_ = 0;
}