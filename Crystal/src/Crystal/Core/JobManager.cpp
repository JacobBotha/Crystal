#include "clpch.h"
#include "JobManager.h"

#include <thread>
#include <atomic>
#include <condition_variable>

namespace Crystal 
{
	namespace JobManager
	{
		template <typename T, size_t capacity>
		class ThreadSafeRingBuffer
		{
		public:
			void Push(T element)
			{
				CL_CORE_ASSERT(!isFull(), "Cannot add element to full ring buffer!");
				m_Lock.lock();
				m_Data[m_Head] = element;
				m_Head = next(m_Head);
				m_Lock.unlock();
			}

			T Pop()
			{
				CL_CORE_ASSERT(!isEmpty(), "Cannot pop element from empty ring buffer!");
				m_Lock.lock();
				T element = m_Data[m_Tail];
				m_Tail = next(m_Tail);
				m_Lock.unlock();
				return element;
			}

			bool isEmpty()
			{
				return m_Head == m_Tail;
			}

			bool isFull()
			{
				return next(m_Head) == m_Tail;
			}

		private:
			size_t next(size_t element)
			{
				return (element + 1) % capacity;
			}

			T m_Data[capacity];
			size_t m_Head = 0;
			size_t m_Tail = 0;
			std::mutex m_Lock;
		};

		//Job Manager globals
		uint32_t threadCount = 0;    
		ThreadSafeRingBuffer<std::function<void()>, 256> jobPool;
		std::condition_variable wakeCondition;
		std::mutex wakeMutex;
		uint64_t currentLabel = 0;
		std::atomic<uint64_t> finishedLabel;

		void Init()
		{
			finishedLabel.store(0);
			auto hardwareCoreCount = std::thread::hardware_concurrency();
			threadCount = std::max(1u, hardwareCoreCount);

			for (uint32_t threadID = 0; threadID < hardwareCoreCount; ++threadID)
			{
				std::thread worker([] {
					while (true)
					{
						if (!jobPool.isEmpty()) 
						{
							CL_CORE_INFO("Executing job!");
							std::function<void()> job = jobPool.Pop();
							job(); 
							finishedLabel.fetch_add(1); 
							continue;
						}
						CL_CORE_INFO("No jobs in queue!");
						std::unique_lock<std::mutex> lock(wakeMutex);
						wakeCondition.wait(lock);
					}
				});

//#ifdef CL_PLATFORM_WINDOWS
//				HANDLE handle = (HANDLE)worker.native_handle();

				//DWORD_PTR affinityMask = 1ull << threadID;
				//DWORD_PTR affinity_result = SetThreadAffinityMask(handle, affinityMask);
				//CL_CORE_ASSERT(affinity_result > 0, "Thread affinity mask is not greater than 0!");

				//BOOL priority_result = SetThreadPriority(handle, THREAD_PRIORITY_HIGHEST);
				//assert(priority_result != 0);

//				std::wstringstream wss;
//				wss << "JobSystem_" << threadID;
//				HRESULT hr = SetThreadDescription(handle, wss.str().c_str());
//				CL_CORE_ASSERT(SUCCEEDED(hr), "Could not set thread description for {0}", wss);
//#endif // CL_PLATFORM_WINDOWS

				worker.detach();
			}
		}

		inline void poll()
		{
			wakeCondition.notify_one(); 
			std::this_thread::yield(); 
		}

		void Execute(const std::function<void()>& job)
		{
			currentLabel += 1;

			while (jobPool.isFull()) 
			{
				poll(); 
			}

			jobPool.Push(job);
			wakeCondition.notify_one(); // wake one thread
		}

		uint32_t GetThreadCount()
		{
			return threadCount;
		}

		bool IsBusy()
		{
			return finishedLabel.load() < currentLabel;
		}

		void Wait()
		{
			while (IsBusy()) { poll(); }
		}
	}
};