#pragma once
#include "../pch.h"

namespace Engine {
    class ThreadPool {
    public:
        ThreadPool(size_t numThreads = std::thread::hardware_concurrency());
        ~ThreadPool();

        template<class F, class... Args>
        auto Enqueue(F&& f, Args&&... args) 
            -> std::future<typename std::invoke_result<F, Args...>::type>;

    private:
        std::vector<std::thread> m_Workers;
        std::queue<std::function<void()>> m_Tasks;
        
        std::mutex m_QueueMutex;
        std::condition_variable m_Condition;
        bool m_Stop;
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
