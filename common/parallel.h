#pragma once
#include <functional>
#include <vector>
#include <queue>
#include <thread>
#include <iostream>
#include <condition_variable>
#include <memory> //unique_ptr
#include <atomic>
#include <stdexcept>
#define N_WORKERS 4

namespace Simflow {
    using namespace std;

    class Parallel {

    private:

        struct Workload {
            function<void()> work;
            function<void()> on_finish;
        };

        std::vector<std::thread> work_threads;
        std::queue<Workload>task_queue;;
        std::mutex queue_mutex;
        std::condition_variable condition;
        bool stop;

        int count;

    private:

        static void* worker(void* arg) {
            Parallel* pool = (Parallel*)arg;
            pool->run();
            return pool;
        }

        void run() {
            while (!stop)
            {
                Workload wl;

                {
                    std::unique_lock<std::mutex> lk(this->queue_mutex);
                    /*　unique_lock() 出作用域自动解锁　*/

                    this->condition.wait(lk, [this] { return !this->task_queue.empty(); });
                    //如果任务队列为空，就停下来等待唤醒

                    if (this->task_queue.empty())
                    {
                        continue;
                    }
                    else
                    {
                        wl = task_queue.front();
                        task_queue.pop();
                    }
                }
                wl.work();
                wl.on_finish();
            }

        }
    public:
        Parallel() : count(N_WORKERS), stop(false) {
            for (int i = 0; i < count; i++)
            {
                std::cout << "创建第" << i << "个线程 " << std::endl;
                work_threads.emplace_back(worker, this);
            }
        };

        ~Parallel() {
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                stop = true;
            }
            condition.notify_all();
            for (auto& ww : work_threads) {
                if (ww.joinable())ww.join();
            }
        }

        void invoke(initializer_list<function<void()>> funcs) {
            mutex mtx_finish;
            unique_lock lock_finish(mtx_finish);
            condition_variable cv_finish;

            atomic<int> counter = 0;
            auto on_finish = [&counter, &cv_finish]() {
                counter++;
                cv_finish.notify_one();
            };

            {
                lock_guard lg(queue_mutex);
                for (auto& f : funcs) {
                    task_queue.push(Workload{ f, on_finish });
                    condition.notify_one();
                }
            }

            cv_finish.wait(lock_finish, [&funcs, &counter]() { return funcs.size() == counter; });

        }
    };

}

