/**
 * \file
 * \author Rudy Castan
 * \author Junseok Lee
 * \date 2025 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#pragma once

namespace window
{
    inline int                FPS                = 0;
    inline unsigned long long FrameCount         = 0;
    inline float              DeltaTime          = 0; // seconds
    inline float              ElapsedTime        = 0; // seconds
    inline int                WindowWidth        = 0;
    inline int                WindowHeight       = 0;
    inline int                DisplayWidth       = 0;
    inline int                DisplayHeight      = 0;
    inline float              HorizontalDPIScale = 1.0;
    inline float              VerticalDPIScale   = 1.0;
#if defined(CAN_USE_THREADS)
    inline constexpr bool CanUseThreads = true;
#else
    inline constexpr bool CanUseThreads = false;
#endif
}
