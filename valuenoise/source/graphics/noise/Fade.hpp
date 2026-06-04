/**
 * \file
 * \author Junseok Lee
 * \date 2025 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#pragma once

#include <array>
#include <cmath>

namespace graphics::noise
{
    enum class SmoothMethod
    {
        Linear,
        Cosine,
        Smoothstep,
        Quintic
    };

    inline float fade(float x, SmoothMethod smoothing) noexcept
    {
        constexpr float pi = 3.14159265358979323846f;

        switch (smoothing)
        {
            case SmoothMethod::Linear: return x;
            case SmoothMethod::Cosine: return (1.0f - std::cos(x * pi)) * 0.5f;
            case SmoothMethod::Smoothstep: return x * x * (3.0f - 2.0f * x);
            case SmoothMethod::Quintic:
            default: return x * x * x * (x * (x * 6.0f - 15.0f) + 10.0f);
        }
    }

    inline auto fade(float x, float y, SmoothMethod smoothing) noexcept
    {
        return std::array{ fade(x, smoothing), fade(y, smoothing) };
    }

    inline auto fade(float x, float y, float z, SmoothMethod smoothing) noexcept
    {
        return std::array{ fade(x, smoothing), fade(y, smoothing), fade(z, smoothing) };
    }
}
