/**
 * \file
 * \author Junseok Lee
 * \date 2025 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#include "FrameBuffer.hpp"

#include "GL.hpp"
#include "util/Logger.hpp"
#include <utility>

namespace opengl
{
    FrameBuffer::~FrameBuffer() noexcept
    {
        delete_framebuffer();
    }

    FrameBuffer::FrameBuffer(FrameBuffer&& other) noexcept
        : frameBufferHandle(other.frameBufferHandle), specification(other.specification), colorTexture(std::move(other.colorTexture))
    {
        other.frameBufferHandle = 0;
        other.specification     = {};
    }

    FrameBuffer& FrameBuffer::operator=(FrameBuffer&& other) noexcept
    {
        std::swap(frameBufferHandle, other.frameBufferHandle);
        std::swap(specification, other.specification);
        std::swap(colorTexture, other.colorTexture);
        return *this;
    }

    bool FrameBuffer::LoadWithSpecification(const Specification& spec) noexcept
    {
        delete_framebuffer();
        specification = spec;
        if (specification.Width <= 0 || specification.Height <= 0 || specification.ColorFormat == ColorComponent::None)
        {
            return false;
        }

        GL::GenFramebuffers(1, &frameBufferHandle);
        Use();

        const auto color_format = static_cast<Texture::ColorFormat>(specification.ColorFormat);
        if (!colorTexture.LoadAsFormat(specification.Width, specification.Height, color_format))
        {
            delete_framebuffer();
            return false;
        }

        GL::FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture.GetHandle(), 0);
        const GLenum color_attachment = GL_COLOR_ATTACHMENT0;
        GL::DrawBuffers(1, &color_attachment);

        const GLenum status = GL::CheckFramebufferStatus(GL_FRAMEBUFFER);
        Use(false);
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            util::log_error("Framebuffer is incomplete. Status: {}", status);
            delete_framebuffer();
            return false;
        }

        return true;
    }

    void FrameBuffer::Use(bool bind) const noexcept
    {
        GL::BindFramebuffer(GL_FRAMEBUFFER, bind ? frameBufferHandle : 0);
    }

    void FrameBuffer::delete_framebuffer() noexcept
    {
        colorTexture = Texture{};
        if (frameBufferHandle != 0)
        {
            GL::DeleteFramebuffers(1, &frameBufferHandle);
        }
        frameBufferHandle = 0;
        specification     = {};
    }
}
