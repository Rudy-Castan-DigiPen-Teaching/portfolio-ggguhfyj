/**
 * \file
 * \author Junseok Lee
 * \date 2025 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#pragma once

#include "Fade.hpp"
#include "Interpolation.hpp"
#include "NoiseCoordinate.hpp"
#include "PeriodDimension.hpp"
#include "PermutationHash.hpp"
#include "util/Random.hpp"

#include <span>
#include <vector>

namespace graphics::noise
{
    template <typename T>
    class [[nodiscard]] ValueNoise
    {
    public:
        explicit ValueNoise(PeriodDimension period = PeriodDimension::_256, SmoothMethod smooth_method = SmoothMethod::Quintic);

        [[nodiscard]] T Evaluate(float x) const noexcept;
        [[nodiscard]] T Evaluate(float x, float y) const noexcept;
        [[nodiscard]] T Evaluate(float x, float y, float z) const noexcept;

        [[nodiscard]] constexpr PeriodDimension GetPeriodDimension() const noexcept;
        void                                    SetPeriod(PeriodDimension period);

        [[nodiscard]] constexpr SmoothMethod GetSmoothing() const noexcept;
        constexpr void                       SetSmoothing(SmoothMethod smooth_method);

        [[nodiscard]] std::span<T> GetValues() noexcept;

    private:
        PeriodDimension periodDimension = PeriodDimension::_256;
        SmoothMethod    smoothMethod    = SmoothMethod::Quintic;
        PermutationHash permutationHash{ periodDimension };
        std::vector<T>  values{};
    };

    template <typename T>
    ValueNoise<T>::ValueNoise(PeriodDimension period, SmoothMethod smooth_method)
    {
        SetSmoothing(smooth_method);
        SetPeriod(period);
    }

    template <typename T>
    T ValueNoise<T>::Evaluate(float x) const noexcept
    {
        const auto coord = make_noise_coord(x);
        const auto s     = fade(coord.interpolant, smoothMethod);

        return linear_mix(
            LinearValues<T>{
                values[static_cast<std::size_t>(permutationHash(coord.base))],
                values[static_cast<std::size_t>(permutationHash(coord.next))] },
            s);
    }

    template <typename T>
    T ValueNoise<T>::Evaluate(float x, float y) const noexcept
    {
        const auto x_coord = make_noise_coord(x);
        const auto y_coord = make_noise_coord(y);
        const auto [s, t]  = fade(x_coord.interpolant, y_coord.interpolant, smoothMethod);

        return bilinear_mix(
            BiLinearValues<T>{
                LinearValues<T>{
                    values[static_cast<std::size_t>(permutationHash(x_coord.base, y_coord.base))],
                    values[static_cast<std::size_t>(permutationHash(x_coord.next, y_coord.base))] },
                LinearValues<T>{
                    values[static_cast<std::size_t>(permutationHash(x_coord.base, y_coord.next))],
                    values[static_cast<std::size_t>(permutationHash(x_coord.next, y_coord.next))] } },
            s, t);
    }

    template <typename T>
    T ValueNoise<T>::Evaluate(float x, float y, float z) const noexcept
    {
        const auto x_coord   = make_noise_coord(x);
        const auto y_coord   = make_noise_coord(y);
        const auto z_coord   = make_noise_coord(z);
        const auto [s, t, p] = fade(x_coord.interpolant, y_coord.interpolant, z_coord.interpolant, smoothMethod);

        return trilinear_mix(
            TriLinearValues<T>{
                BiLinearValues<T>{
                    LinearValues<T>{
                        values[static_cast<std::size_t>(permutationHash(x_coord.base, y_coord.base, z_coord.base))],
                        values[static_cast<std::size_t>(permutationHash(x_coord.next, y_coord.base, z_coord.base))] },
                    LinearValues<T>{
                        values[static_cast<std::size_t>(permutationHash(x_coord.base, y_coord.next, z_coord.base))],
                        values[static_cast<std::size_t>(permutationHash(x_coord.next, y_coord.next, z_coord.base))] } },
                BiLinearValues<T>{
                    LinearValues<T>{
                        values[static_cast<std::size_t>(permutationHash(x_coord.base, y_coord.base, z_coord.next))],
                        values[static_cast<std::size_t>(permutationHash(x_coord.next, y_coord.base, z_coord.next))] },
                    LinearValues<T>{
                        values[static_cast<std::size_t>(permutationHash(x_coord.base, y_coord.next, z_coord.next))],
                        values[static_cast<std::size_t>(permutationHash(x_coord.next, y_coord.next, z_coord.next))] } } },
            s, t, p);
    }

    template <typename T>
    constexpr PeriodDimension ValueNoise<T>::GetPeriodDimension() const noexcept
    {
        return periodDimension;
    }

    template <typename T>
    void ValueNoise<T>::SetPeriod(PeriodDimension period)
    {
        periodDimension = period;
        permutationHash = PermutationHash(periodDimension);
        values.resize(static_cast<std::size_t>(periodDimension));
        for (auto& value : values)
        {
            value = T{ util::random() };
        }
    }

    template <typename T>
    constexpr SmoothMethod ValueNoise<T>::GetSmoothing() const noexcept
    {
        return smoothMethod;
    }

    template <typename T>
    constexpr void ValueNoise<T>::SetSmoothing(SmoothMethod smooth_method)
    {
        smoothMethod = smooth_method;
    }

    template <typename T>
    std::span<T> ValueNoise<T>::GetValues() noexcept
    {
        return std::span<T>{ values };
    }
}
