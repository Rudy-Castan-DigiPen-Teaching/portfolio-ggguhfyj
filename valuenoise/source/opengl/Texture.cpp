/**
 * \file
 * \author Rudy Castan
 * \author JUNSEOK LEE
 * \date 2025 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#include "Texture.hpp"

#include "Environment.hpp"
#include "GL.hpp"
#include "assets/Path.hpp"
#include <GL/glew.h>
#include <stb_image.h>

namespace
{
    GLenum external_format(opengl::Texture::ColorFormat format) noexcept
    {
        switch (format)
        {
            case opengl::Texture::R32F: return GL_RED;
            case opengl::Texture::RGBA8:
            case opengl::Texture::RGBA32F: return GL_RGBA;
            case opengl::Texture::None: break;
        }
        return GL_RGBA;
    }

    GLenum external_type(opengl::Texture::ColorFormat format) noexcept
    {
        switch (format)
        {
            case opengl::Texture::RGBA32F:
            case opengl::Texture::R32F: return GL_FLOAT;
            case opengl::Texture::RGBA8:
            case opengl::Texture::None: break;
        }
        return GL_UNSIGNED_BYTE;
    }
}

namespace opengl
{

    Texture::~Texture() noexcept
    {
        delete_texture();
    }

    Texture::Texture(Texture&& other) noexcept : texture_handle(other.texture_handle), width(other.width), height(other.height), color_format(other.color_format)
    {
        other.texture_handle = 0;
        other.width          = 0;
        other.height         = 0;
        other.color_format   = ColorFormat::None;
    }

    Texture& Texture::operator=(Texture&& other) noexcept
    {
        std::swap(texture_handle, other.texture_handle);
        std::swap(width, other.width);
        std::swap(height, other.height);
        std::swap(color_format, other.color_format);
        return *this;
    }

    bool Texture::LoadFromFileImage(std::filesystem::path image_filepath, bool flip_vertical) noexcept
    {
        if (!std::filesystem::exists(image_filepath))
        {
            // try prepending the asset directory path
            image_filepath = assets::get_base_path() / image_filepath;
            if (!std::filesystem::exists(image_filepath))
            {
                return false;
            }
        }
        int           pixel_width;
        int           pixel_height;
        int           files_channels_count  = 0;
        constexpr int desired_channel_count = 4;
        stbi_set_flip_vertically_on_load(flip_vertical);
        RGBA* const rgba_pixels = reinterpret_cast<RGBA*>(stbi_load(image_filepath.string().c_str(), &pixel_width, &pixel_height, &files_channels_count, desired_channel_count));
        if (rgba_pixels == nullptr)
            return false;
        const bool result = LoadFromMemory(pixel_width, pixel_height, rgba_pixels);
        stbi_image_free(rgba_pixels);
        return result;
    }

    bool Texture::LoadFromMemory(int image_width, int image_height, const RGBA* colors) noexcept
    {
        delete_texture();
        if (image_width <= 0 || image_height <= 0 || colors == nullptr)
        {
            return false;
        }

        width  = image_width;
        height = image_height;
        color_format = ColorFormat::RGBA8;

        GL::GenTextures(1, &texture_handle);
        GL::BindTexture(GL_TEXTURE_2D, texture_handle);
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(filtering));
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(filtering));
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapping[S]));
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrapping[T]));
        GL::TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, colors);
        GL::BindTexture(GL_TEXTURE_2D, 0);

        return true;
    }

    bool Texture::LoadAsRGBA(int image_width, int image_height) noexcept
    {
        return LoadAsFormat(image_width, image_height, ColorFormat::RGBA8);
    }

    bool Texture::LoadAsFormat(int image_width, int image_height, ColorFormat format) noexcept
    {
        delete_texture();
        if (image_width <= 0 || image_height <= 0 || format == ColorFormat::None)
        {
            return false;
        }

        width        = image_width;
        height       = image_height;
        color_format = format;

        GL::GenTextures(1, &texture_handle);
        GL::BindTexture(GL_TEXTURE_2D, texture_handle);
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(filtering));
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(filtering));
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapping[S]));
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrapping[T]));
        if (opengl::HasTextureStorage)
        {
            GL::TexStorage2D(GL_TEXTURE_2D, 1, static_cast<GLenum>(format), width, height);
        }
        else
        {
            GL::TexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(format), width, height, 0, external_format(format), external_type(format), nullptr);
        }
        GL::BindTexture(GL_TEXTURE_2D, 0);

        return true;
    }

    void Texture::UploadAsRGBA(const RGBA* colors) noexcept
    {
        if (texture_handle == 0 || colors == nullptr)
        {
            return;
        }

        GL::BindTexture(GL_TEXTURE_2D, texture_handle);
        GL::TexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, colors);
        GL::BindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::UseForSlot(unsigned texture_unit) const noexcept
    {
        GL::ActiveTexture(GL_TEXTURE0 + texture_unit);
        GL::BindTexture(GL_TEXTURE_2D, texture_handle);
    }

    void Texture::SetFiltering(Filtering how_to_filter) noexcept
    {
        if (filtering == how_to_filter || texture_handle == 0)
            return;

        filtering = how_to_filter;
        GL::BindTexture(GL_TEXTURE_2D, texture_handle);
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(filtering));
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(filtering));
        GL::BindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::SetWrapping(Wrapping how_to_wrap, Coordinate coord) noexcept
    {
        if (texture_handle == 0)
            return;

        GLenum coords_to_set[2] = {};
        int    num_coords       = 1;
        switch (coord)
        {
            case Coordinate::S:
                coords_to_set[0] = GL_TEXTURE_WRAP_S;
                wrapping[S]      = how_to_wrap;
                break;
            case Coordinate::T:
                coords_to_set[0] = GL_TEXTURE_WRAP_T;
                wrapping[T]      = how_to_wrap;
                break;
            case Coordinate::Both:
                coords_to_set[0] = GL_TEXTURE_WRAP_S;
                coords_to_set[1] = GL_TEXTURE_WRAP_T;
                wrapping[S]      = how_to_wrap;
                wrapping[T]      = how_to_wrap;
                num_coords       = 2;
                break;
        }

        GL::BindTexture(GL_TEXTURE_2D, texture_handle);
        for (int i = 0; i < num_coords; ++i)
        {
            GL::TexParameteri(GL_TEXTURE_2D, coords_to_set[i], static_cast<GLint>(how_to_wrap));
        }
        GL::BindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::delete_texture() noexcept
    {
        if (texture_handle != 0)
        {
            GL::DeleteTextures(1, &texture_handle);
        }
        texture_handle = 0;
        width          = 0;
        height         = 0;
        color_format   = ColorFormat::None;
    }
}
