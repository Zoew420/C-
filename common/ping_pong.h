#pragma once
#include <memory>
namespace T {
    using namespace std;

    template<typename T>
    class PingPongBuffer {
        int _size;
        unique_ptr<T[]> _buffer_cur;
        unique_ptr<T[]> _buffer_prev;
    public:

        void swap() {
            T* t = _buffer_cur;
            _buffer_cur = _buffer_prev;
            _buffer_prev = t;
        }

        int size() { return _size };

        T* buffer_cur() { return _buffer_cur.get(); }

        T* buffer_prev() { return _buffer_prev.get(); }

        PingPongBuffer(int size) : _size(size), _buffer_cur(new T[size]), _buffer_prev(new T[size]) {}
    };

    template<typename T>
    class PingPong {
        unique_ptr<T> _cur, _prev;
    public:
        void swap() {
            unique_ptr<T> t = move(_cur);
            _cur = move(_prev);
            _prev = move(t);
        }
        T* cur() { return _cur.get(); }
        T* prev() { return _prev.get(); }
        PingPong() : _cur(new T), _prev(new T) { }
        PingPong(T&& cur, T&& prev) : _cur(new T(move(cur))), _prev(new T(move(prev))) {}
    };
}