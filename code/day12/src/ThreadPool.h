/******************************
*   author: yuesong-feng
*   
*
*
******************************/
#pragma once
// #include <functional>
// #include <vector>
// #include <queue>
// #include <thread>
// #include <mutex>
// #include <condition_variable>
// #include <future>

// class ThreadPool
// {
// private:
//     std::vector<std::thread> threads;
//     std::queue<std::function<void()>> tasks;
//     std::mutex tasks_mtx;
//     std::condition_variable cv;
//     bool stop;
// public:
//     ThreadPool(int size = 10);
//     ~ThreadPool();

//     // void add(std::function<void()>);
//     template<class F, class... Args>
//     auto add(F&& f, Args&&... args) 
//     -> std::future<typename std::result_of<F(Args...)>::type>;

// };


// //不能放在cpp文件，原因不明
// template<class F, class... Args>
// auto ThreadPool::add(F&& f, Args&&... args) 
//     -> std::future<typename std::result_of<F(Args...)>::type>
// {
//     using return_type = typename std::result_of<F(Args...)>::type;

//     auto task = std::make_shared< std::packaged_task<return_type()> >(
//             std::bind(std::forward<F>(f), std::forward<Args>(args)...)
//         );
        
//     std::future<return_type> res = task->get_future();
//     {
//         std::unique_lock<std::mutex> lock(tasks_mtx);

//         // don't allow enqueueing after stopping the pool
//         if(stop)
//             throw std::runtime_error("enqueue on stopped ThreadPool");

//         tasks.emplace([task](){ (*task)(); });
//     }
//     cv.notify_one();
//     return res;
// }


#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
class ThreadPool {
   private:
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    std::mutex tasks_mtx;
    bool stop;

   public:
    ThreadPool(int size = 10);
    ~ThreadPool();
    template <typename T, typename... Args>
    void add(std::function<T(Args...)> func, Args... args);
};

ThreadPool::ThreadPool(int size) : stop(false) {
    for (int i = 0; i < size; ++i) {
        threads.emplace_back(std::thread([this]() {
            while (true) {
                std::function<void()> func;
                std::unique_lock<std::mutex> tasks_lock(tasks_mtx);
                if (!tasks.empty()) {
                    func = tasks.front();
                    tasks.pop();
                    tasks_lock.unlock();
                    func();
                } else {
                    tasks_lock.unlock();
                }
                if (stop && tasks.empty()) return;
            }
        }));
    }
}

template <typename T, typename... Args>
void ThreadPool::add(std::function<T(Args...)> func, Args... args) {
    std::unique_lock<std::mutex> lock(tasks_mtx);
    std::function<void()> task = std::bind(func, args...);
    tasks.push(task);
}

ThreadPool::~ThreadPool() {
    stop = true;
    for (std::thread &th : threads) {
        if (th.joinable()) th.join();
    }
}