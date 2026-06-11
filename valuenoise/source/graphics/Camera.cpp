/**
 * \file
 * \author Junseok Lee
 * \date 2025 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#include "Camera.hpp"

#include <algorithm>
#include <cmath>
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>

namespace graphics
{
    Camera::Camera(glm::vec3 eye_position, glm::vec3 look_direction, glm::vec3 up_direction) : position(eye_position), forward(glm::normalize(look_direction)), worldUp(glm::normalize(up_direction))
    {
        refresh_basis();
    }

    glm::mat4 Camera::ViewMatrix() const
    {
        return glm::lookAt(position, position + forward, up);
    }

    void Camera::Move(glm::vec3 camera_space_delta)
    {
        position += right * camera_space_delta.x;
        position += worldUp * camera_space_delta.y;
        position += forward * camera_space_delta.z;
    }

    void Camera::Rotate(float yaw_radians, float pitch_radians)
    {
        if (yaw_radians != 0.0f)
        {
            const glm::mat4 yaw_matrix = glm::rotate(glm::mat4(1.0f), yaw_radians, worldUp);
            forward                    = glm::normalize(glm::vec3(yaw_matrix * glm::vec4(forward, 0.0f)));
        }

        refresh_basis();
        if (pitch_radians != 0.0f)
        {
            const glm::mat4 pitch_matrix = glm::rotate(glm::mat4(1.0f), pitch_radians, right);
            const glm::vec3 candidate    = glm::normalize(glm::vec3(pitch_matrix * glm::vec4(forward, 0.0f)));
            if (std::abs(glm::dot(candidate, worldUp)) < 0.98f)
            {
                forward = candidate;
            }
        }

        refresh_basis();
    }

    void Camera::refresh_basis()
    {
        forward = glm::normalize(forward);
        right   = glm::normalize(glm::cross(forward, worldUp));
        up      = glm::normalize(glm::cross(right, forward));
    }
}
