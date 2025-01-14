#pragma once
#include "../pch.h"

namespace Engine {
    /**
     * @brief Thread pool for parallel task execution
     * 
     * Manages a pool of worker threads and distributes tasks among them
     * for efficient parallel processing.
     */
    class ThreadPool {
    public:
        /**
         * @brief Constructs a thread pool
         * @param numThreads Number of worker threads (defaults to hardware threads)
         */
        ThreadPool(size_t numThreads = std::thread::hardware_concurrency());
        ~ThreadPool();

        /**
         * @brief Adds a task to the pool
         * @tparam F Function type
         * @tparam Args Argument types
         * @param f Function to execute
         * @param args Arguments for the function
         * @return Future containing the task result
         */
        template<class F, class... Args>
        auto Enqueue(F&& f, Args&&... args) 
            -> std::future<typename std::invoke_result<F, Args...>::type>;

    private:
        std::vector<std::thread> m_Workers;      ///< Worker threads
        std::queue<std::function<void()>> m_Tasks;  ///< Task queue
        
        std::mutex m_QueueMutex;                 ///< Mutex for task queue
        std::condition_variable m_Condition;      ///< Condition for thread synchronization
        bool m_Stop;                             ///< Stop flag for shutdown
    };

    // Template implementation
    template<class F, class... Args>
    auto ThreadPool::Enqueue(F&& f, Args&&... args) 
        -> std::future<typename std::invoke_result<F, Args...>::type>
    {
        using return_type = typename std::invoke_result<F, Args...>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
            
        std::future<return_type> res = task->get_future();
        {
            std::lock_guard<std::mutex> lock(m_QueueMutex);
            m_Tasks.emplace([task](){ (*task)(); });
        }
        m_Condition.notify_one();
        return res;
    }
}
