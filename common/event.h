#pragma once
#include <vector>
#include <memory>
#include <functional>
#include <iostream>
namespace T {
    using namespace std;


    // �¼��������
    // �̳�����ʵ�־���Ĵ������
    template <typename ...Args>
    class EventHandler {
    public:
        virtual void handle(Args... args) = 0;
    };

    template <typename... Args>
    class LambdaEventHandler : public EventHandler<Args...> {
        function<void(Args...)> f;
    public:
        LambdaEventHandler(function<void(Args...)> f) : f(f) {}
        void handle(Args... args) {
            f(args...);
        }
    };

    template <typename... Args>
    inline shared_ptr<EventHandler<Args...>> function_handler(function<void(Args...)> f) {
        return static_pointer_cast<EventHandler<Args...>>(make_shared<LambdaEventHandler<Args...>>(f));
        //return shared_ptr<EventHandler<Args...>>(new LambdaEventHandler<Args...>(f));
    }

    // ����һ���¼�Դ
    template <typename ...Args>
    class EventSource {
        vector<shared_ptr<EventHandler<Args...>>> _handlers;

    public:

        EventSource() { 
			cout << _handlers.size() << endl; };

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
            //cout << _handlers.size() << endl;
            for (auto& h : _handlers) {
               // cout << "!!!" << endl;
                h->handle(args...);
            }
        }
    };


    template <typename F, typename ...Args>
    inline shared_ptr<EventHandler<Args...>> create_function_handler() {

    };
}