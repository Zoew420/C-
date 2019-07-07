#pragma once
#include <vector>
#include <memory>

namespace T {
    using namespace std;

    // �¼��������
    // �̳�����ʵ�־���Ĵ������
    template <typename ...Args>
    class EventHandler {
    public:
        virtual void trigger(Args... args) = 0;
    };

    // ����һ���¼�Դ
    template <typename ...Args>
    class EventSource {
        vector<shared_ptr<EventHandler<Args...>>> _handlers;

    public:
        // ����һ���������
        void add_handler(shared_ptr<EventHandler<Args...>> h) {
            _handlers.push_back(h);
        }

        // ɾ��һ���������
        void remove_handler(shared_ptr<EventHandler<Args...>> h) {
            for (int i = 0; i < _handlers.size(); i++) {
                if (_handlers[i].get() == h.get()) {
                    _handlers.erase(_handlers.begin() + i);
                    break;
                }
            }
        }

        // ����һ���������
        EventSource& operator+=(shared_ptr<EventHandler<Args...>> h) {
            add_handler(h);
            return *this;
        }

        // ɾ��һ���������
        EventSource& operator-=(shared_ptr<EventHandler<Args...>> h) {
            remove_handler(h);
            return *this;
        }

        // �������¼�
        void trigger(Args... args) {
            for (auto& h : _handlers) {
                h->trigger(args);
            }
        }
    };
}