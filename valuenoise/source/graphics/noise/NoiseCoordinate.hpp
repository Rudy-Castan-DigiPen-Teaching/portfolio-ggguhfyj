/**
 * \file
 * \author Junseok Lee
 * \date 2025 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#pragma once

namespace graphics::noise
{
    struct [[nodiscard]] NoiseCoordinate
    {
        int   base        = 0;
        int   next        = 0;
        float interpolant = 0.0f;
    };

    constexpr NoiseCoordinate make_noise_coord(float input) noexcept
    {
        int base = static_cast<int>(input);
        if (input < static_cast<float>(base))
        {
            --base;
        }

        return NoiseCoordinate{ base, base + 1, input - static_cast<float>(base) };
    }
}
