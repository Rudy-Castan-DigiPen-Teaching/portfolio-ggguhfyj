/**
 * \file
 * \author Junseok Lee
 * \date 2025 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#pragma once

#include <functional>

namespace util
{
    class JobSystem
    {
    public:
        using Job = std::function<void(void)>;
        void DoJob(Job job);
        void WaitUntilDone();
        bool IsDone() const;

        using ComputeAtIndex = std::function<void(int)>;
        void DoJobs(int how_many, ComputeAtIndex compute);
    };
}
