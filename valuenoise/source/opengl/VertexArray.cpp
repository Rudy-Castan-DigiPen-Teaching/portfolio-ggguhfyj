/**
 * \file
 * \author Rudy Castan
 * \author JUNSEOK LEE
 * \date 2025 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */

#include "VertexArray.hpp"

#include "GL.hpp"
#include <GL/glew.h>
#include <cstdint>
#include <gsl/gsl>

namespace opengl
{

    VertexArray::VertexArray(Primitive::Type the_primitive_pattern)
    {
        primitive_pattern = the_primitive_pattern;
        GL::GenVertexArrays(1, &vertex_array_handle);
    }

    VertexArray::~VertexArray()
    {
        if (vertex_array_handle != 0)
        {
            GL::DeleteVertexArrays(1, &vertex_array_handle);
        }
    }

    VertexArray::VertexArray(VertexArray&& temp) noexcept
        : vertex_array_handle(temp.vertex_array_handle), vertex_buffers(std::move(temp.vertex_buffers)), index_buffer(std::move(temp.index_buffer)), num_indices(temp.num_indices),
          indices_type(temp.indices_type), primitive_pattern(temp.primitive_pattern), num_vertices(temp.num_vertices)
    {
        temp.vertex_array_handle = 0;
        temp.num_indices         = 0;
        temp.indices_type        = IndexElement::None;
        temp.num_vertices        = 0;
    }

    VertexArray& VertexArray::operator=(VertexArray&& temp) noexcept
    {
        std::swap(vertex_array_handle, temp.vertex_array_handle);
        std::swap(vertex_buffers, temp.vertex_buffers);
        std::swap(index_buffer, temp.index_buffer);
        std::swap(num_indices, temp.num_indices);
        std::swap(indices_type, temp.indices_type);
        std::swap(primitive_pattern, temp.primitive_pattern);
        std::swap(num_vertices, temp.num_vertices);

        return *this;
    }

    void VertexArray::Use(bool bind) const
    {
        GL::BindVertexArray(bind ? vertex_array_handle : 0);
    }

    void VertexArray::AddVertexBuffer(VertexBuffer&& vertex_buffer, BufferLayout buffer_layout)
    {
        GLsizei stride = 0;
        for (const auto attribute : buffer_layout.Attributes)
        {
            stride += static_cast<GLsizei>(attribute.SizeBytes);
        }

        if (stride > 0)
        {
            const auto available_bytes = vertex_buffer.GetSizeBytes() - static_cast<GLsizei>(buffer_layout.BufferStartingByteOffset);
            num_vertices               = available_bytes / stride;
        }

        Use();
        vertex_buffer.Use();

        uint32_t offset          = buffer_layout.BufferStartingByteOffset;
        GLuint   attribute_index = 0;
        for (const auto attribute : buffer_layout.Attributes)
        {
            if (attribute == Attribute::None)
            {
                continue;
            }

            GL::EnableVertexAttribArray(attribute_index);
            const auto* attribute_offset = reinterpret_cast<const void*>(static_cast<std::uintptr_t>(offset));
            if (attribute.IntAttribute)
            {
                GL::VertexAttribIPointer(attribute_index, static_cast<GLint>(attribute.ComponentCount), attribute.GLType, stride, attribute_offset);
            }
            else
            {
                GL::VertexAttribPointer(attribute_index, static_cast<GLint>(attribute.ComponentCount), attribute.GLType, attribute.Normalize ? GL_TRUE : GL_FALSE, stride, attribute_offset);
            }
            GL::VertexAttribDivisor(attribute_index, static_cast<GLuint>(attribute.Divisor));

            ++attribute_index;
            offset += attribute.SizeBytes;
        }

        Use(false);
        vertex_buffer.Use(false);
        vertex_buffers.emplace_back(std::move(vertex_buffer));
    }

    void VertexArray::SetIndexBuffer(IndexBuffer&& the_indices)
    {
        num_indices  = the_indices.GetCount();
        indices_type = the_indices.GetElementType();
        Use();
        the_indices.Use();
        index_buffer = std::move(the_indices);
    }

    void DrawIndexed(const VertexArray& vertex_array) noexcept
    {
        GL::DrawElements(vertex_array.GetPrimitivePattern(), vertex_array.GetIndicesCount(), vertex_array.GetIndicesType(), nullptr);
    }

    void DrawVertices(const VertexArray& vertex_array) noexcept
    {
        GL::DrawArrays(vertex_array.GetPrimitivePattern(), 0, vertex_array.GetVertexCount());
    }
}
