/**
 * \file
 * \author Rudy Castan
 * \author JUNSEOK LEE
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#include "Mesh.hpp"
#include <algorithm>
#include <cmath>
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <gsl/gsl>
#include <numbers>

namespace
{
    int                               positive_count(int value) noexcept;
    std::vector<graphics::MeshVertex> create_plane_vertices(int stacks, int slices);
    std::vector<unsigned>             build_index_buffer(int stacks, int slices);
    std::vector<unsigned>             convert_to_lines_pattern(const std::vector<unsigned>& indices);
}

namespace graphics
{
    Geometry create_plane(int stacks, int slices)
    {
        auto vertices = create_plane_vertices(stacks, slices);
        auto indices  = build_index_buffer(stacks, slices);
        return Geometry{ std::move(vertices), std::move(indices) };
    }

    Geometry create_cube(int stacks, int slices)
    {
        const auto plane_vertices = create_plane_vertices(stacks, slices);
        const auto plane_indices  = build_index_buffer(stacks, slices);

        enum Axis
        {
            X,
            Y
        };

        constexpr glm::vec3 AxisVectors[2] = { glm::vec3(1, 0, 0), glm::vec3(0, 1, 0) };

        constexpr glm::vec3 translate_array[] = {
            glm::vec3(+0.0f, +0.0f, +0.5f), // Z+
            glm::vec3(+0.0f, +0.0f, -0.5f), // Z-
            glm::vec3(+0.5f, +0.0f, +0.0f), // X+
            glm::vec3(-0.5f, +0.0f, +0.0f), // X-
            glm::vec3(+0.0f, +0.5f, +0.0f), // Y+
            glm::vec3(+0.0f, -0.5f, +0.0f), // Y-
        };

        struct rotation
        {
            Axis  axis;
            float angle;
        };

        constexpr rotation rotate_array[] = {
            { Axis::Y,                 0.0f }, // Z+
            { Axis::Y, glm::radians(180.0f) }, // Z-
            { Axis::Y,  glm::radians(90.0f) }, // X+
            { Axis::Y, glm::radians(-90.0f) }, // X-
            { Axis::X, glm::radians(-90.0f) }, // Y+
            { Axis::X,  glm::radians(90.0f) }  // Y-
        };

        std::vector<MeshVertex> vertices;
        std::vector<unsigned>   indices;
        vertices.reserve(plane_vertices.size() * 6u);
        indices.reserve(plane_indices.size() * 6u);
        /*  Transform the plane to 6 positions to form the faces of the cube */
        for (unsigned i = 0; i < 6u; ++i)
        {
            constexpr glm::mat4 identity(1.0f);
            const auto          axis             = AxisVectors[rotate_array[i].axis];
            const auto          angle            = rotate_array[i].angle;
            const auto          rotation_matrix  = glm::rotate(identity, angle, axis);
            const auto          transform_matrix = glm::translate(identity, translate_array[i]) * rotation_matrix;
            for (const auto plane_vertex : plane_vertices)
            {
                MeshVertex v;
                v.position = glm::vec3(transform_matrix * glm::vec4(plane_vertex.position, 1.0f));
                v.normal   = glm::vec3(rotation_matrix * glm::vec4(plane_vertex.normal, 0.0f));
                v.uv       = plane_vertex.uv;
                vertices.push_back(v);
            }
            for (const auto plane_index : plane_indices)
            {
                indices.push_back(plane_index + static_cast<unsigned>(plane_vertices.size()) * i);
            }
        }
        return Geometry{ std::move(vertices), std::move(indices) };
    }

    Geometry create_sphere(int stacks, int slices)
    {
        const int       stack_count = positive_count(stacks);
        const int       slice_count = positive_count(slices);
        constexpr float radius      = 0.5f;

        std::vector<MeshVertex> vertices;
        vertices.reserve(static_cast<std::size_t>(stack_count + 1) * static_cast<std::size_t>(slice_count + 1));

        for (int stack = 0; stack <= stack_count; ++stack)
        {
            const float row      = static_cast<float>(stack) / static_cast<float>(stack_count);
            const float beta     = PI * (row - 0.5f);
            const float sin_beta = std::sin(beta);
            const float cos_beta = std::cos(beta);

            for (int slice = 0; slice <= slice_count; ++slice)
            {
                const float col   = static_cast<float>(slice) / static_cast<float>(slice_count);
                const float alpha = col * 2.0f * PI;

                MeshVertex vertex;
                vertex.position = glm::vec3(radius * std::sin(alpha) * cos_beta, radius * sin_beta, radius * std::cos(alpha) * cos_beta);
                vertex.normal   = vertex.position / radius;
                vertex.uv       = glm::vec2(col, row);
                vertices.push_back(vertex);
            }
        }

        auto indices = build_index_buffer(stack_count, slice_count);
        return Geometry{ std::move(vertices), std::move(indices) };
    }

    Geometry create_torus(int stacks, int slices, float start_angle, float end_angle)
    {
        const int stack_count = positive_count(stacks);
        const int slice_count = positive_count(slices);

        constexpr float major_radius = 0.35f;
        constexpr float minor_radius = 0.15f;

        std::vector<MeshVertex> vertices;
        vertices.reserve(static_cast<std::size_t>(stack_count + 1) * static_cast<std::size_t>(slice_count + 1));

        for (int stack = 0; stack <= stack_count; ++stack)
        {
            const float row       = static_cast<float>(stack) / static_cast<float>(stack_count);
            const float alpha     = start_angle + (end_angle - start_angle) * row;
            const float sin_alpha = std::sin(alpha);
            const float cos_alpha = std::cos(alpha);

            for (int slice = 0; slice <= slice_count; ++slice)
            {
                const float col       = static_cast<float>(slice) / static_cast<float>(slice_count);
                const float beta      = (1.0f - col) * 2.0f * PI;
                const float sin_beta  = std::sin(beta);
                const float cos_beta  = std::cos(beta);
                const auto  center    = glm::vec3(major_radius * sin_alpha, 0.0f, major_radius * cos_alpha);
                const float ring_size = major_radius + minor_radius * cos_beta;

                MeshVertex vertex;
                vertex.position = glm::vec3(ring_size * sin_alpha, minor_radius * sin_beta, ring_size * cos_alpha);
                vertex.normal   = (vertex.position - center) / minor_radius;
                vertex.uv       = glm::vec2(col, row);
                vertices.push_back(vertex);
            }
        }

        auto indices = build_index_buffer(stack_count, slice_count);
        return Geometry{ std::move(vertices), std::move(indices) };
    }

    void add_cap(std::vector<MeshVertex>& vertices, std::vector<unsigned>& indices, float center_y, int slices)
    {
        const int       slice_count = positive_count(slices);
        constexpr float radius      = 0.5f;
        const bool      is_top      = center_y > 0.0f;
        const auto      center      = static_cast<unsigned>(vertices.size());

        MeshVertex vertex;
        vertex.position = glm::vec3(0.0f, center_y, 0.0f);
        vertex.normal   = is_top ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(0.0f, -1.0f, 0.0f);
        vertex.uv       = glm::vec2(0.5f, 0.5f);
        vertices.push_back(vertex);

        const float uv_scale = is_top ? 1.0f : -1.0f;
        for (int slice = 0; slice <= slice_count; ++slice)
        {
            const float col       = static_cast<float>(slice) / static_cast<float>(slice_count);
            const float alpha     = col * 2.0f * PI;
            const float sin_alpha = std::sin(alpha);
            const float cos_alpha = std::cos(alpha);

            vertex.position = glm::vec3(radius * sin_alpha, center_y, radius * cos_alpha);
            vertex.uv       = glm::vec2(uv_scale * 0.5f * cos_alpha + 0.5f, 0.5f * sin_alpha + 0.5f);
            vertices.push_back(vertex);
        }

        for (int slice = 0; slice < slice_count; ++slice)
        {
            const auto current = center + 1u + static_cast<unsigned>(slice);
            const auto next    = current + 1u;
            if (is_top)
            {
                indices.push_back(center);
                indices.push_back(current);
                indices.push_back(next);
            }
            else
            {
                indices.push_back(center);
                indices.push_back(next);
                indices.push_back(current);
            }
        }
    }

    Geometry create_cylinder(int stacks, int slices)
    {
        const int       stack_count = positive_count(stacks);
        const int       slice_count = positive_count(slices);
        constexpr float radius      = 0.5f;

        std::vector<MeshVertex> vertices;
        vertices.reserve(static_cast<std::size_t>(stack_count + 1) * static_cast<std::size_t>(slice_count + 1) + 2u * (static_cast<std::size_t>(slice_count) + 2u));

        for (int stack = 0; stack <= stack_count; ++stack)
        {
            const float row = static_cast<float>(stack) / static_cast<float>(stack_count);
            for (int slice = 0; slice <= slice_count; ++slice)
            {
                const float col       = static_cast<float>(slice) / static_cast<float>(slice_count);
                const float alpha     = col * 2.0f * PI;
                const float sin_alpha = std::sin(alpha);
                const float cos_alpha = std::cos(alpha);

                MeshVertex vertex;
                vertex.position = glm::vec3(radius * sin_alpha, row - 0.5f, radius * cos_alpha);
                vertex.normal   = glm::vec3(sin_alpha, 0.0f, cos_alpha);
                vertex.uv       = glm::vec2(col, row);
                vertices.push_back(vertex);
            }
        }

        auto indices = build_index_buffer(stack_count, slice_count);
        add_cap(vertices, indices, 0.5f, slice_count);
        add_cap(vertices, indices, -0.5f, slice_count);

        return Geometry{ std::move(vertices), std::move(indices) };
    }

    Geometry create_cone(int stacks, int slices)
    {
        const int       stack_count = positive_count(stacks);
        const int       slice_count = positive_count(slices);
        constexpr float radius      = 0.5f;
        constexpr float top_y       = 0.5f;
        constexpr float bottom_y    = -0.5f;
        constexpr float side_slope  = radius / (top_y - bottom_y);

        std::vector<MeshVertex> vertices;
        vertices.reserve(static_cast<std::size_t>(stack_count + 1) * static_cast<std::size_t>(slice_count + 1) + static_cast<std::size_t>(slice_count + 2));

        for (int stack = 0; stack <= stack_count; ++stack)
        {
            const float row      = static_cast<float>(stack) / static_cast<float>(stack_count);
            const float height   = row - 0.5f;
            const float row_size = radius * (0.5f - height);

            for (int slice = 0; slice <= slice_count; ++slice)
            {
                const float col       = static_cast<float>(slice) / static_cast<float>(slice_count);
                const float alpha     = col * 2.0f * PI;
                const float sin_alpha = std::sin(alpha);
                const float cos_alpha = std::cos(alpha);

                MeshVertex vertex;
                vertex.position = glm::vec3(row_size * sin_alpha, height, row_size * cos_alpha);
                vertex.normal   = (stack == stack_count) ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::normalize(glm::vec3(sin_alpha, side_slope, cos_alpha));
                vertex.uv       = glm::vec2(col, row);
                vertices.push_back(vertex);
            }
        }

        auto indices = build_index_buffer(stack_count, slice_count);
        add_cap(vertices, indices, bottom_y, slice_count);

        return Geometry{ std::move(vertices), std::move(indices) };
    }

    SubMesh to_submesh_as_triangles(const Geometry& geometry, Material* material)
    {
        SubMesh sub_mesh;
        sub_mesh.VertexArrayObj.SetPrimitivePattern(opengl::Primitive::Triangles);
        sub_mesh.VertexArrayObj.AddVertexBuffer(opengl::VertexBuffer(std::span{ geometry.Vertices }), get_meshvertex_layout());
        sub_mesh.VertexArrayObj.SetIndexBuffer(opengl::IndexBuffer(std::span{ geometry.Indicies }));
        sub_mesh.Material = material;
        return sub_mesh;
    }

    SubMesh to_submesh_as_lines(const Geometry& geometry, Material* material)
    {
        std::vector<unsigned> lines_indices = convert_to_lines_pattern(geometry.Indicies);
        SubMesh               sub_mesh;
        sub_mesh.VertexArrayObj.SetPrimitivePattern(opengl::Primitive::Lines);
        sub_mesh.VertexArrayObj.AddVertexBuffer(opengl::VertexBuffer(std::span{ geometry.Vertices }), get_meshvertex_layout());
        sub_mesh.VertexArrayObj.SetIndexBuffer(opengl::IndexBuffer(std::span{ lines_indices }));
        sub_mesh.Material = material;
        return sub_mesh;
    }
}

namespace
{
    int positive_count(int value) noexcept
    {
        return std::max(1, value);
    }

    std::vector<graphics::MeshVertex> create_plane_vertices(int stacks, int slices)
    {
        const int stack_count = positive_count(stacks);
        const int slice_count = positive_count(slices);

        std::vector<graphics::MeshVertex> vertices;
        vertices.reserve(static_cast<std::size_t>(stack_count + 1) * static_cast<std::size_t>(slice_count + 1));

        for (int stack = 0; stack <= stack_count; ++stack)
        {
            const float row = static_cast<float>(stack) / static_cast<float>(stack_count);
            for (int slice = 0; slice <= slice_count; ++slice)
            {
                const float col = static_cast<float>(slice) / static_cast<float>(slice_count);

                graphics::MeshVertex vertex;
                vertex.position = glm::vec3(col - 0.5f, row - 0.5f, 0.0f);
                vertex.normal   = glm::vec3(0.0f, 0.0f, 1.0f);
                vertex.uv       = glm::vec2(col, row);
                vertices.push_back(vertex);
            }
        }

        return vertices;
    }

    std::vector<unsigned> build_index_buffer(int stacks, int slices)
    {
        const auto stack_count = static_cast<unsigned>(positive_count(stacks));
        const auto slice_count = static_cast<unsigned>(positive_count(slices));
        const auto stride      = slice_count + 1u;

        std::vector<unsigned> indices;
        indices.reserve(static_cast<std::size_t>(stack_count) * static_cast<std::size_t>(slice_count) * 6u);

        for (unsigned stack = 0; stack < stack_count; ++stack)
        {
            const unsigned curr_row = stack * stride;
            for (unsigned slice = 0; slice < slice_count; ++slice)
            {
                const unsigned p0 = curr_row + slice;
                const unsigned p1 = p0 + 1u;
                const unsigned p2 = p1 + stride;

                indices.push_back(p0);
                indices.push_back(p1);
                indices.push_back(p2);

                indices.push_back(p2);
                indices.push_back(p2 - 1u);
                indices.push_back(p0);
            }
        }

        return indices;
    }

    std::vector<unsigned> convert_to_lines_pattern(const std::vector<unsigned>& indices)
    {
        std::vector<unsigned> line_indices;
        line_indices.reserve(indices.size() * 2u);

        std::size_t i = 0;
        while (i + 5u < indices.size())
        {
            const unsigned p0 = indices[i];
            const unsigned p1 = indices[i + 1u];
            const unsigned p2 = indices[i + 2u];
            const unsigned p3 = indices[i + 3u];
            const unsigned p4 = indices[i + 4u];
            const unsigned p5 = indices[i + 5u];

            if (p1 != p0 + 1u || p3 != p2 || p4 != p3 - 1u || p5 != p0)
            {
                break;
            }

            line_indices.push_back(p0);
            line_indices.push_back(p1);
            line_indices.push_back(p1);
            line_indices.push_back(p2);
            line_indices.push_back(p2);
            line_indices.push_back(p4);
            line_indices.push_back(p4);
            line_indices.push_back(p0);

            i += 6u;
        }

        while (i + 2u < indices.size())
        {
            const unsigned p0 = indices[i];
            const unsigned p1 = indices[i + 1u];
            const unsigned p2 = indices[i + 2u];

            line_indices.push_back(p0);
            line_indices.push_back(p1);
            line_indices.push_back(p1);
            line_indices.push_back(p2);
            line_indices.push_back(p2);
            line_indices.push_back(p0);

            i += 3u;
        }

        return line_indices;
    }
}
