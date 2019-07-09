#pragma once
#include <Windows.h>
namespace my3d {
    typedef float Color[3];

    namespace present {
#ifdef WIN32
#include <Windows.h>
        void initialize(HWND hwnd, int w, int h, int pixel_size = 5);
        void close();
        void present();
        void set_pixel(int x, int y, my3d::Color color);
#else
#error("my3dpresent currently supports only Windows.")
#endif
    }  // namespace present
}  // namespace my3d