/**
 * \file
 * \author Junseok Lee
 * \date 2025 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace graphics
{
    class Camera
    {
    public:
        static constexpr glm::vec3 WORLD_UP{ 0.0f, 1.0f, 0.0f };

        Camera() = default;
        Camera(glm::vec3 eye_position, glm::vec3 look_direction, glm::vec3 up_direction = WORLD_UP);

        [[nodiscard]] glm::mat4 ViewMatrix() const;

        void Move(glm::vec3 camera_space_delta);
        void Rotate(float yaw_radians, float pitch_radians);

    private:
        void refresh_basis();

    private:
        glm::vec3 position{ 0.0f, 0.0f, 1.0f };
        glm::vec3 forward{ 0.0f, 0.0f, -1.0f };
        glm::vec3 right{ 1.0f, 0.0f, 0.0f };
        glm::vec3 up{ WORLD_UP };
        glm::vec3 worldUp{ WORLD_UP };
    };
}
