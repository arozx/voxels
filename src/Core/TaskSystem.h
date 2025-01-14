#pragma once
#include "../pch.h"

namespace Engine {
    /**
     * @brief Thread pool-based task system for concurrent execution
     * 
     * Provides a simple interface for executing tasks asynchronously using
     * a pool of worker threads.
     */
    class TaskSystem {
    public:
        /**
         * @brief Gets the singleton instance of the task system
         * @return Reference to the task system instance
         */
        static TaskSystem& Get() {
            static TaskSystem instance;
            return instance;
        }

        /**
         * @brief Initializes the task system with worker threads
         * @param threadCount Number of worker threads (0 for automatic)
         */
        void Initialize(size_t threadCount = 0) {
            if (threadCount == 0) {
                // Use physical cores only, not hyperthreaded
                threadCount = std::max(1u, std::thread::hardware_concurrency() / 2);
            }
            
            m_Running = true;
            
            // Create worker threads
            for (size_t i = 0; i < threadCount; ++i) {
                m_Workers.emplace_back([this] { WorkerThread(); });
            }
        }

        ~TaskSystem() {
            {
                std::unique_lock<std::mutex> lock(m_QueueMutex);
                m_Running = false;
            }
            m_Condition.notify_all();
            
            for (auto& worker : m_Workers) {
                if (worker.joinable()) {
                    worker.join();
                }
            }
        }

        /**
         * @brief Submits a task for asynchronous execution
         * @tparam F Function type of the task
         * @param task Task to be executed
         * @return Future containing the task's result
         */
        template<typename F>
        auto EnqueueTask(F&& task) -> std::future<typename std::invoke_result<F>::type> {
            using ReturnType = typename std::invoke_result<F>::type;
            auto promise = std::make_shared<std::promise<ReturnType>>();
            std::future<ReturnType> future = promise->get_future();

            {
                std::lock_guard<std::mutex> lock(m_QueueMutex);
                m_Tasks.emplace([promise, task = std::forward<F>(task)]() {
                    try {
                        if constexpr (std::is_void_v<ReturnType>) {
                            task();
                            promise->set_value();
                        } else {
                            promise->set_value(task());
                        }
                    } catch (...) {
                        promise->set_exception(std::current_exception());
                    }
                });
            }
            
            m_Condition.notify_one();
            return future;
        }

    private:
        TaskSystem() = default;
        
        /**
         * @brief Worker thread function that processes tasks from the queue
         */
        void WorkerThread() {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(m_QueueMutex);
                    m_Condition.wait(lock, [this] {
                        return !m_Running || !m_Tasks.empty();
                    });
                    
                    if (!m_Running && m_Tasks.empty()) {
                        return;
                    }
                    
                    task = std::move(m_Tasks.front());
                    m_Tasks.pop();
                }
                task();
            }
        }

        std::vector<std::thread> m_Workers;
        std::queue<std::function<void()>> m_Tasks;
        std::mutex m_QueueMutex;
        std::condition_variable m_Condition;
        bool m_Running = false;
    };
}
