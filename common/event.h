#pragma once
#include <vector>
#include <memory>

namespace T {
    using namespace std;

    // 事件处理程序
    // 继承它并实现具体的处理代码
    template <typename ...Args>
    class EventHandler {
    public:
        virtual void trigger(Args... args) = 0;
    };

    // 代表一个事件源
    template <typename ...Args>
    class EventSource {
        vector<shared_ptr<EventHandler<Args...>>> _handlers;

    public:
        // 新增一个处理程序
        void add_handler(shared_ptr<EventHandler<Args...>> h) {
            _handlers.push_back(h);
        }

        // 删除一个处理程序
        void remove_handler(shared_ptr<EventHandler<Args...>> h) {
            for (int i = 0; i < _handlers.size(); i++) {
                if (_handlers[i].get() == h.get()) {
                    _handlers.erase(_handlers.begin() + i);
                    break;
                }
            }
        }

        // 新增一个处理程序
        EventSource& operator+=(shared_ptr<EventHandler<Args...>> h) {
            add_handler(h);
            return *this;
        }

        // 删除一个处理程序
        EventSource& operator-=(shared_ptr<EventHandler<Args...>> h) {
            remove_handler(h);
            return *this;
        }

        // 触发该事件
        void trigger(Args... args) {
            for (auto& h : _handlers) {
                h->trigger(args);
            }
        }
    };
}