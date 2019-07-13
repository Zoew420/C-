#pragma once
#include <functional>
#include <vector>
#include <queue>
#include <thread>
#include <iostream>
#include <condition_variable>
#include <memory> //unique_ptr
#include <atomic>
#include <future>
#include <stdexcept>
#define threadsize 3

namespace Simflow {
	using namespace std;

	class Parallel {

	private:
		std::vector<std::thread> work_threads;
		std::queue<function<void()>>task_queue;;
		std::mutex queue_mutex;
		std::condition_variable condition;
		bool stop;

		int count;

	private:
		static void *worker(void *arg) {
			Parallel *pool = (Parallel *)arg;
			pool->run();
			return pool;
		}
		void run() {
			while (!stop)
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
					function<void()> request = task_queue.front();
					task_queue.pop();
					request();
				}
			}

		}
	public:
		Parallel() :count(threadsize), stop(false) {
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
			for (auto &ww : work_threads)if(ww.joinable())ww.join();
		}

		void invoke(initializer_list<function<void()>> funcs) {
			for (auto &f : funcs) {
				queue_mutex.lock();
				task_queue.push(f);
				queue_mutex.unlock();
				condition.notify_one();
			}
			return;
		}
	};


	class threadpool
	{
		using Task = std::function<void()>;
		// 线程池

		std::vector<std::thread> pool;
		// 任务队列

		std::queue<Task> tasks;
		// 同步

		std::mutex m_lock;
		// 条件阻塞

		std::condition_variable cv_task;
		// 是否关闭提交

		std::atomic<bool> stoped;
		//空闲线程数量

		std::atomic<int>  idlThrNum;

	public:
		inline threadpool(unsigned short size = 4) :stoped{ false }
		{
			idlThrNum = size < 1 ? 1 : size;
			for (size = 0; size < idlThrNum; ++size)
			{   //初始化线程数量
				pool.emplace_back(
					[this]
				{ // 工作线程函数
					while (!this->stoped)
					{
						std::function<void()> task;
						{   // 获取一个待执行的 task
							// unique_lock 相比 lock_guard 的好处是：可以随时 unlock() 和 lock()
							std::unique_lock<std::mutex> lock{ this->m_lock };
							this->cv_task.wait(lock,
								[this] {
								return this->stoped.load() || !this->tasks.empty();
							}
							); // wait 直到有 task
							if (this->stoped && this->tasks.empty())
								return;
							task = std::move(this->tasks.front()); // 取一个 task
							this->tasks.pop();
						}
						idlThrNum--;
						task();
						idlThrNum++;
					}
				}
				);
			}
		}
		inline ~threadpool()
		{
			stoped.store(true);
			cv_task.notify_all(); // 唤醒所有线程执行
			for (std::thread& thread : pool) {
				//thread.detach(); // 让线程“自生自灭”
				if (thread.joinable())
					thread.join(); // 等待任务结束， 前提：线程一定会执行完
			}
		}

	public:
		// 提交一个任务
		// 调用.get()获取返回值会等待任务执行完,获取返回值
		// 有两种方法可以实现调用类成员，
		// 一种是使用   bind： .commit(std::bind(&Dog::sayHello, &dog));
		// 一种是用 mem_fn： .commit(std::mem_fn(&Dog::sayHello), &dog)
		template<class F, class... Args>
		auto commit(F&& f, Args&&... args) ->std::future<decltype(f(args...))>
		{
			if (stoped.load()) 
				throw std::runtime_error("commit on ThreadPool is stopped.");

			using RetType = decltype(f(args...)); // typename std::result_of<F(Args...)>::type, 函数 f 的返回值类型
			auto task = std::make_shared<std::packaged_task<RetType()> >(
				std::bind(std::forward<F>(f), std::forward<Args>(args)...)
				);    // wtf !

			std::future<RetType> future = task->get_future();
			{    // 添加任务到队列
				//对当前块的语句加锁  lock_guard 是 mutex 的 stack 封装类，构造的时候 lock()，析构的时候 unlock()
				std::lock_guard<std::mutex> lock{ m_lock };
				tasks.emplace(
					[task]()
				{ // push(Task{...})
					(*task)();
				}
				);
			}
			cv_task.notify_one(); // 唤醒一个线程执行

			return future;
		}



		//空闲线程数量
		int idlCount() { return idlThrNum; }
	};

}

