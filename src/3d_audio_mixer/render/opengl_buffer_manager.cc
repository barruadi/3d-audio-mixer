#include "app/pch.h"

#include "render/opengl_buffer_manager.hh"
#include <glad/glad.h>

namespace nrender
{
    void OpenGL_FrameBuffer::create_buffers(int32_t width, int32_t height)
    {
        if (width <= 0 || height <= 0) return;

        mWidth = width;
        mHeight = height;

        if (mFBO) delete_buffers();

        glGenFramebuffers(1, &mFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

        // Color texture
        glGenTextures(1, &mTextureId);
        glBindTexture(GL_TEXTURE_2D, mTextureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureId, 0);

        // Depth+Stencil renderbuffer
        glGenRenderbuffers(1, &mDepthId);
        glBindRenderbuffer(GL_RENDERBUFFER, mDepthId);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mWidth, mHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthId);

        // Draw buffer
        const GLenum drawBuffers = GL_COLOR_ATTACHMENT0;
        glDrawBuffers(1, &drawBuffers);

        // Completeness check
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "[FBO] Framebuffer not complete" << std::endl;
        }

        unbind();
    }

    void OpenGL_FrameBuffer::delete_buffers()
    {
        if (mFBO) 
        {
            glDeleteFramebuffers(1, &mFBO);
            mFBO = 0;
        }

        if (mTextureId) 
        {
            glDeleteTextures(1, &mTextureId);
            mTextureId = 0;
        }

        if (mDepthId) 
        {
            glDeleteRenderbuffers(1, &mDepthId);
            mDepthId = 0;
        }

        mWidth  = 0;
        mHeight = 0;
    }

    void OpenGL_FrameBuffer::bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
        glViewport(0, 0, mWidth, mHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGL_FrameBuffer::unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGL_VertexIndexBuffer::create_buffers(const glm::vec3& nodePosition)
    {
        glGenVertexArrays(1, &mVAO);
        glGenBuffers(1, &mVBO);

        glBindVertexArray(mVAO);

        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3), &nodePosition, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0); // location = 0 : position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

        glEnable(GL_PROGRAM_POINT_SIZE);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void OpenGL_VertexIndexBuffer::delete_buffers()
    {
        if (mVBO) {
            glDeleteBuffers(1, &mVBO);
            mVBO = 0;
        }

        if (mVAO) {
            glDeleteVertexArrays(1, &mVAO);
            mVAO = 0;
        }
    }

    void OpenGL_VertexIndexBuffer::bind()
    {
        glBindVertexArray(mVAO);
    }

    void OpenGL_VertexIndexBuffer::unbind()
    {
        glBindVertexArray(0);
    }

    void OpenGL_VertexIndexBuffer::draw()
    {
        bind();

        glDrawArrays(GL_POINTS, 0, 1);

        unbind();
    }
} // namespace nrender
