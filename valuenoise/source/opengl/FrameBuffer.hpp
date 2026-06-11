/**
 * \file
 * \author Junseok Lee
 * \date 2025 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#pragma once

#include "Handle.hpp"
#include "Texture.hpp"
#include <GL/glew.h>

namespace opengl
{
    class [[nodiscard]] FrameBuffer
    {
    public:
        enum ColorComponent : GLenum
        {
            None    = Texture::ColorFormat::None,
            RGBA8   = Texture::ColorFormat::RGBA8,
            RGBA32F = Texture::ColorFormat::RGBA32F,
            R32F    = Texture::ColorFormat::R32F,
        };

        struct Specification
        {
            int                  Width       = 0;
            int                  Height      = 0;
            Texture::ColorFormat DepthFormat = Texture::ColorFormat::None;
            ColorComponent       ColorFormat = ColorComponent::RGBA8;
        };

        FrameBuffer() = default;
        ~FrameBuffer() noexcept;

        FrameBuffer(const FrameBuffer&)            = delete;
        FrameBuffer& operator=(const FrameBuffer&) = delete;
        FrameBuffer(FrameBuffer&& other) noexcept;
        FrameBuffer& operator=(FrameBuffer&& other) noexcept;

        [[nodiscard]] bool LoadWithSpecification(const Specification& spec) noexcept;
        void               Use(bool bind = true) const noexcept;

        [[nodiscard]] Texture& ColorTexture() noexcept
        {
            return colorTexture;
        }

        [[nodiscard]] const Texture& ColorTexture() const noexcept
        {
            return colorTexture;
        }

        [[nodiscard]] Handle GetHandle() const noexcept
        {
            return frameBufferHandle;
        }

    private:
        void delete_framebuffer() noexcept;

    private:
        Handle        frameBufferHandle = 0;
        Specification specification{};
        Texture       colorTexture{};
    };
}
