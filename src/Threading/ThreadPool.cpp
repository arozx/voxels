/**
 * @file ThreadPool.cpp
 * @brief Implementation of the thread pool system
 * 
 * Provides implementation for thread pool initialization, task distribution,
 * and cleanup operations.
 */
#include "ThreadPool.h"

namespace Engine {
    /**
     * @brief Initializes thread pool with specified number of threads
     * 
     * Creates worker threads and sets up task processing loop for each thread.
     */
    ThreadPool::ThreadPool(size_t numThreads) : m_Stop(false) {
        for(size_t i = 0; i < numThreads; ++i) {
            m_Workers.emplace_back([this] {
                while(true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(m_QueueMutex);
                        m_Condition.wait(lock, [this] { 
                            return m_Stop || !m_Tasks.empty(); 
                        });
                        
                        if(m_Stop && m_Tasks.empty())
                            return;
                            
                        task = std::move(m_Tasks.front());
                        m_Tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    /**
     * @brief Cleans up thread pool
     * 
     * Signals all threads to stop and waits for them to complete.
     */
    ThreadPool::~ThreadPool() {
        {
            std::lock_guard<std::mutex> lock(m_QueueMutex);
            m_Stop = true;
        }
        m_Condition.notify_all();
        
        for(auto& worker : m_Workers)
            worker.join();
    }
}
