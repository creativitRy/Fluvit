#include <GL/glew.h>
#include "render_pass.h"
#include <iostream>
#include <debuggl.h>
#include <map>

RenderInputMeta::RenderInputMeta() {
}

bool RenderInputMeta::isInteger() const {
    return element_type == GL_INT || element_type == GL_UNSIGNED_INT;
}

RenderInputMeta::RenderInputMeta(int _position,
                                 const std::string &_name,
                                 const void *_data,
                                 size_t _nelements,
                                 size_t _element_length,
                                 int _element_type)
        : position(_position), name(_name), data(_data),
          nelements(_nelements), element_length(_element_length),
          element_type(_element_type) {
}

RenderDataInput::RenderDataInput()
        : index_meta_(new RenderInputMeta) {
}

RenderDataInput::~RenderDataInput() {
}

RenderPass::RenderPass(int vao, // -1: create new VAO, otherwise use given VAO
                       const RenderDataInput &input,
                       const std::vector<const char *> shaders, // Order: VS, GS, FS [, TCS, TES]
                       const std::vector<ShaderUniformPtr> uniforms,
                       const std::vector<const char *> output // Order: 0, 1, 2...
)
        : vao_(vao), input_(input), uniforms_(uniforms) {
    if (vao_ < 0) {
        CHECK_GL_ERROR(glGenVertexArrays(1, (GLuint *) &vao_));
    }
    CHECK_GL_ERROR(glBindVertexArray(vao_));

    // Program first
    vs_ = compileShader(shaders[0], GL_VERTEX_SHADER);
    gs_ = compileShader(shaders[1], GL_GEOMETRY_SHADER);
    fs_ = compileShader(shaders[2], GL_FRAGMENT_SHADER);
    CHECK_GL_ERROR(sp_ = glCreateProgram());
    glAttachShader(sp_, vs_);
    glAttachShader(sp_, fs_);
    if (shaders[1])
        glAttachShader(sp_, gs_);
    if (shaders.size() > 3) {
        auto tcs = compileShader(shaders[3], GL_VERTEX_SHADER);
        glAttachShader(sp_, tcs);
        auto tes = compileShader(shaders[4], GL_GEOMETRY_SHADER);
        glAttachShader(sp_, tes);
    }

    // ... and then buffers
    size_t nbuffer = input.getNBuffers();
    if (input.hasIndex())
        nbuffer++;
    glbuffers_.resize(nbuffer);
    CHECK_GL_ERROR(glGenBuffers(nbuffer, glbuffers_.data()));
    for (int i = 0; i < input.getNBuffers(); i++) {
        auto meta = input.getBufferMeta(i);
        CHECK_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, glbuffers_[i]));
        CHECK_GL_ERROR(glBufferData(GL_ARRAY_BUFFER,
                                    meta.getElementSize() * meta.nelements,
                                    meta.data,
                                    GL_STATIC_DRAW));
        if (meta.isInteger()) {
            CHECK_GL_ERROR(glVertexAttribIPointer(meta.position,
                                                  meta.element_length,
                                                  meta.element_type,
                                                  0, 0));
        } else {
            CHECK_GL_ERROR(glVertexAttribPointer(meta.position,
                                                 meta.element_length,
                                                 meta.element_type,
                                                 GL_FALSE, 0, 0));
        }
        CHECK_GL_ERROR(glEnableVertexAttribArray(meta.position));
        // ... because we need program to bind location
        CHECK_GL_ERROR(glBindAttribLocation(sp_, meta.position, meta.name.c_str()));
    }
    // .. bind output position
    for (size_t i = 0; i < output.size(); i++) {
        if (output[i] != nullptr) {
            CHECK_GL_ERROR(glBindFragDataLocation(sp_, i, output[i]));
        }
    }
    // ... then we can link
    glLinkProgram(sp_);
    CHECK_GL_PROGRAM_ERROR(sp_);

    if (input.hasIndex()) {
        auto meta = input.getIndexMeta();
        CHECK_GL_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                                    glbuffers_.back()
        ));
        CHECK_GL_ERROR(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                                    meta.getElementSize() * meta.nelements,
                                    meta.data, GL_STATIC_DRAW));
    }
    // after linking uniform locations can be determined
    unilocs_.resize(uniforms.size());
    for (size_t i = 0; i < uniforms.size(); i++) {
        CHECK_GL_ERROR(unilocs_[i] = glGetUniformLocation(sp_, uniforms[i]->name.c_str()));
        std::cout << "Uniform " << uniforms[i]->name << " has location " << unilocs_[i] << std::endl;
    }
    if (input_.hasMaterial()) {
        createMaterialTexture();
        initMaterialUniform();
    }
}

void RenderPass::initMaterialUniform() {
    material_uniforms_.clear();
    for (size_t i = 0; i < input_.getNMaterials(); i++) {
        auto &ma = input_.getMaterial(i);
        using V4F = std::function<glm::vec4()>;
        using IF = std::function<int()>;
        using FF = std::function<float()>;
        V4F diffuse_data = [&ma]() {
            return ma.diffuse;
        };
        V4F ambient_data = [&ma]() {
            return ma.ambient;
        };
        V4F specular_data = [&ma]() {
            return ma.specular;
        };
        FF shininess_data = [&ma]() {
            return ma.shininess;
        };
        int texid = matexids_[i];
        int sam = sampler2d_;
        IF texture_data = [texid]() {
            return texid;
        };
        IF sampler_data = [sam]() {
            return sam;
        };
        auto diffuse = make_uniform("diffuse", diffuse_data);
        auto ambient = make_uniform("ambient", ambient_data);
        auto specular = make_uniform("specular", specular_data);
        auto shininess = make_uniform("shininess", shininess_data);
        auto texture = make_texture("textureSampler", sampler_data, 0, texture_data);
        std::vector<ShaderUniformPtr> munis =
                {diffuse, ambient, specular, shininess, texture};
        material_uniforms_.emplace_back(munis);
    }
    malocs_.clear();
    CHECK_GL_ERROR(malocs_.emplace_back(glGetUniformLocation(sp_, "diffuse")));
    CHECK_GL_ERROR(malocs_.emplace_back(glGetUniformLocation(sp_, "ambient")));
    CHECK_GL_ERROR(malocs_.emplace_back(glGetUniformLocation(sp_, "specular")));
    CHECK_GL_ERROR(malocs_.emplace_back(glGetUniformLocation(sp_, "shininess")));
    CHECK_GL_ERROR(malocs_.emplace_back(glGetUniformLocation(sp_, "textureSampler")));
    std::cerr << "textureSampler location: " << malocs_.back() << std::endl;
}

/*
 * Create textures to gltextures_
 * and assign material specified textures to matexids_
 * 
 * Different materials may share textures
 */
void RenderPass::createMaterialTexture() {
    CHECK_GL_ERROR(glActiveTexture(GL_TEXTURE0 + 0));
    matexids_.clear();
    std::map<Image *, unsigned> tex2id;
    for (size_t i = 0; i < input_.getNMaterials(); i++) {
        auto &ma = input_.getMaterial(i);
#if 0
        std::cerr << __func__ << " Material " << i << " has texture pointer " << ma.texture.get() << std::endl;
#endif
        if (!ma.texture) {
            matexids_.emplace_back(0);
            continue;
        }
        // Do not create multiple texture for the same data.
        auto iter = tex2id.find(ma.texture.get());
        if (iter != tex2id.end()) {
            matexids_.emplace_back(iter->second);
            continue;
        }

        // Now create and upload texture data
        int w = ma.texture->width;
        int h = ma.texture->height;
        // TODO: enable stride
        // Translate RGB to RGBA for alignment
        std::vector<unsigned int> dummy(w * h);
        const unsigned char *bytes = ma.texture->bytes.data();
        for (int row = 0; row < h; row++) {
            for (int col = 0; col < w; col++) {
                unsigned r = bytes[row * w * 3 + col * 3];
                unsigned g = bytes[row * w * 3 + col * 3 + 1];
                unsigned b = bytes[row * w * 3 + col * 3 + 1];
                dummy[row * w + col] = r | (g << 8) | (b << 16) | (0xFF << 24);
            }
        }
        GLuint tex = 0;
        CHECK_GL_ERROR(glGenTextures(1, &tex));
        CHECK_GL_ERROR(glBindTexture(GL_TEXTURE_2D, tex));
        CHECK_GL_ERROR(glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8,
                                      w,
                                      h));
        CHECK_GL_ERROR(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h,
                                       GL_RGBA, GL_UNSIGNED_BYTE,
                                       dummy.data()));
        //CHECK_GL_ERROR(glPixelStorei(GL_UNPACK_ROW_LENGTH, 0));
        std::cerr << __func__ << " load data into texture " << tex <<
                  " dim: " << w << " x " << h << std::endl;
        CHECK_GL_ERROR(glBindTexture(GL_TEXTURE_2D, 0));
        matexids_.emplace_back(tex);
        tex2id[ma.texture.get()] = tex;
    }
    CHECK_GL_ERROR(glGenSamplers(1, &sampler2d_));
    CHECK_GL_ERROR(glSamplerParameteri(sampler2d_, GL_TEXTURE_WRAP_S, GL_REPEAT));
    CHECK_GL_ERROR(glSamplerParameteri(sampler2d_, GL_TEXTURE_WRAP_T, GL_REPEAT));
    CHECK_GL_ERROR(glSamplerParameteri(sampler2d_, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    CHECK_GL_ERROR(glSamplerParameteri(sampler2d_, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
}

RenderPass::~RenderPass() {
    // TODO: Free resources
}

void RenderPass::updateVBO(int position, const void *data, size_t size) {
    int bufferid = -1;
    for (int i = 0; i < input_.getNBuffers(); i++) {
        auto meta = input_.getBufferMeta(i);
        if (meta.position == position) {
            bufferid = i;
            break;
        }
    }
    if (bufferid < 0)
        throw __func__ + std::string(": error, can't find buffer with position ") + std::to_string(position);
    auto meta = input_.getBufferMeta(bufferid);
    CHECK_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, glbuffers_[bufferid]));
    CHECK_GL_ERROR(glBufferData(GL_ARRAY_BUFFER,
                                size * meta.getElementSize(),
                                data, GL_STATIC_DRAW));
}

void RenderPass::setup() {
    // Switch to our object VAO.
    CHECK_GL_ERROR(glBindVertexArray(vao_));
    // Use our program.
    CHECK_GL_ERROR(glUseProgram(sp_));

    bindUniformsTo(uniforms_, unilocs_);
}

void RenderPass::render(GLenum type) {
    CHECK_GL_ERROR(glDrawElements(type,
                                  input_.getIndexMeta().nelements * input_.getIndexMeta().element_length,
                                  GL_UNSIGNED_INT, 0));
}

bool RenderPass::renderWithMaterial(int mid) {
    if (mid >= int(material_uniforms_.size()) || mid < 0)
        return false;
    const auto &mat = input_.getMaterial(mid);
#if 0
    if (!mat.texture)
        return true;
#endif
    auto &matuni = material_uniforms_[mid];
    bindUniformsTo(matuni, malocs_);
    CHECK_GL_ERROR(glDrawElements(GL_TRIANGLES, mat.nfaces * 3,
                                  GL_UNSIGNED_INT,
                                  (const void *) (mat.offset * 3 * 4)) // Offset is in bytes
    );
    return true;
}

void RenderPass::bindUniformsTo(std::vector<ShaderUniformPtr> &uniforms,
                                const std::vector<unsigned> &unilocs) {
    for (size_t i = 0; i < uniforms.size(); i++) {
        const auto &uni = uniforms[i];
        uni->bind(unilocs[i]);
    }
}

unsigned RenderPass::compileShader(const char *source_ptr, int type) {
    if (!source_ptr)
        return 0;
    auto iter = shader_cache_.find(source_ptr);
    if (iter != shader_cache_.end()) {
        return iter->second;
    }
    GLuint ret = 0;
    CHECK_GL_ERROR(ret = glCreateShader(type));
#if 0
    std::cerr << __func__ << " shader id " << ret << " type " << type << "\tsource:\n" << source_ptr << std::endl;
#endif
    CHECK_GL_ERROR(glShaderSource(ret, 1, &source_ptr, nullptr));
    glCompileShader(ret);
    CHECK_GL_SHADER_ERROR(ret);
    shader_cache_[source_ptr] = ret;
    return ret;
}

void RenderDataInput::assign(int position,
                             const std::string &name,
                             const void *data,
                             size_t nelements,
                             size_t element_length,
                             int element_type) {
    meta_.emplace_back(position, name, data, nelements, element_length, element_type);
}

void RenderDataInput::assignIndex(const void *data, size_t nelements, size_t element_length) {
    has_index_ = true;
    *index_meta_ = {-1, "", data, nelements, element_length, GL_UNSIGNED_INT};
}

int RenderDataInput::getNBuffers() const {
    return int(meta_.size());
}

void RenderDataInput::useMaterials(const std::vector<Material> &ms) {
    materials_ = ms;
    for (const auto &ma : ms) {
        std::cerr << "Use Material from " << ma.offset << " size: " << ma.nfaces << std::endl;
    }
}

const RenderInputMeta &
RenderDataInput::getBufferMeta(int i) const {
    return meta_[i];
}

size_t RenderInputMeta::getElementSize() const {
    size_t element_size = 4;
    if (element_type == GL_FLOAT)
        element_size = 4;
    else if (element_type == GL_UNSIGNED_INT)
        element_size = 4;
    else if (element_type == GL_INT)
        element_size = 4;
    return element_size * element_length;
}

std::map<const char *, unsigned> RenderPass::shader_cache_;
