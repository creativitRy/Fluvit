#ifndef TEXTURE_TO_RENDER_H
#define TEXTURE_TO_RENDER_H

class TextureToRender {
public:
	TextureToRender();
	~TextureToRender();
	void create(int width, int height);
	void bind();
	void unbind();
	int getTexture() const { return tex_; }
	TextureToRender(const TextureToRender &) = delete;
  	TextureToRender &operator=(const TextureToRender &) = delete;
	TextureToRender(TextureToRender &&other) : w_(other.w_), h_(other.h_), fb_(other.fb_), tex_(other.tex_), dep_(other.dep_) 
	{
		other.fb_ = 0;
		other.tex_ = 0; //Use the "null" texture for the old object.
		other.dep_ = 0;
	}

	/*TextureToRender &operator=(TextureToRender &&other)
	{
		//ALWAYS check for self-assignment.
		if(this != &other)
		{
			release();
			//tex_ is now 0.
			std::swap(fb_, other.fb_);
			std::swap(tex_, other.tex_);
			std::swap(dep_, other.dep_);
		}
	}*/

private:
	int w_, h_;
	unsigned int fb_ = -1;
	unsigned int tex_ = -1;
	unsigned int dep_ = -1;
	void release();
};

#endif
