#pragma once
#include "particle.h"

namespace Simflow {

    template<typename T>
    class Array2D {
        T* _data = nullptr;
        int _width = 0, _height = 0;
    public:
        Array2D(int h, int w) {
            _width = w;
            _height = h;
            _data = new T[w * h];
        }
        Array2D(const Array2D&) = delete;
        Array2D(Array2D&&) = delete;
        ~Array2D() { if (_data) delete _data; }

        int width() const { return _width; }
        int height() const { return _height; }
        bool in_bound(int row, int col) const { return row >= 0 && row < _height && col >= 0 && col < _width; }
        const T& item(int row, int col) const { return _data[row * _width + col]; }
        T& item(int row, int col) { return _data[row * _width + col]; }
        const T* operator[](int row) const { return &item(row, 0); }
        T* operator[](int row) { return &item(row, 0); }
    };
}