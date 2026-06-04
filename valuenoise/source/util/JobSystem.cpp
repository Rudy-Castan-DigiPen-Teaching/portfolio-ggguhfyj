/**
 * \file
 * \author Junseok Lee
 * \date 2025 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */

#include "JobSystem.hpp"

namespace util
{
    void JobSystem::DoJob(JobSystem::Job job)
    {
        job();
    }

    void JobSystem::WaitUntilDone()
    {
    }

    void JobSystem::DoJobs(int how_many, JobSystem::ComputeAtIndex compute)
    {
        for (int i = 0; i < how_many; ++i)
        {
            compute(i);
        }
    }

    bool JobSystem::IsDone() const
    {
        return true;
    }
}
