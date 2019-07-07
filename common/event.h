#pragma once
#include <vector>
#include <memory>

namespace T {
    using namespace std;

    template <typename ...Args>
    class EventHandler {
    public:
        virtual void trigger(Args... args) = 0;
    };

    template <typename ...Args>
    class EventSource {
        vector<shared_ptr<EventHandler<Args...>>> _handlers;
    public:
        void add_handler(shared_ptr<EventHandler<Args...>> h) {
            _handlers.push_back(h);
        }
        void remove_handler(shared_ptr<EventHandler<Args...>> h) {
            for (int i = 0; i < _handlers.size(); i++) {
                if (_handlers[i].get() == h.get()) {
                    _handlers.erase(_handlers.begin() + i);
                    break;
                }
            }
        }
        EventSource& operator+=(shared_ptr<EventHandler<Args...>> h) {
            add_handler(h);
            return *this;
        }
        EventSource& operator-=(shared_ptr<EventHandler<Args...>> h) {
            remove_handler(h);
            return *this;
        }
        void trigger(Args... args) {
            for (auto& h : _handlers) {
                h->trigger(args);
            }
        }
    };
}