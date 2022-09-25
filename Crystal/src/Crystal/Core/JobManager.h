#pragma once

namespace Crystal
{
	namespace JobManager
	{
		struct JobArgs
		{
			uint32_t jobIndex;
			uint32_t groupIndex;
		};

		void Init();

		uint32_t GetThreadCount();

		void Execute(const std::function<void()>& job);

		//Implementation undefined - No current use.
		void Dispatch(uint32_t jobCount, uint32_t groupSize, const std::function<void(JobArgs)>& task, size_t sharedmemory_size = 0);

		void Wait();

	};
};
