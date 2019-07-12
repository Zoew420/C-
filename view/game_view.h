#pragma once
#define _CRT_SECURE_NO_WARNINGS 
#include "../common/event.h"
#include "../common/particle.h"
#include "../common/parameter.h"
#include "imgui/imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/glew/GL/glew.h"
#include "imgui/glfw/include/GLFW/glfw3.h"
#include "event_handler/frame_ready.h"
#include "gl_util.h"
#include "draw_particle.h"
#include <iostream>
#include <vector>
#include <memory>

namespace Simflow {
    using namespace std;

    class PressureGraphRenderer {
        GLuint framebuffer = -1;
        GLuint framebuffer_tex = -1;

    public:
        PressureGraphRenderer() {
            create_framebuffer(&framebuffer, &framebuffer_tex, 128, 128);
        }

        // 返回纹理id
        GLuint render_texture(const FrameData& data) {
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
            int m = data.pressure.width();
            int n = data.pressure.height();
            reset_matrix(128, 128);
            glClear(GL_COLOR_BUFFER_BIT);
            glBegin(GL_POINTS);
            for (int j = 0; j < n; j += 4) {
                for (int i = 0; i < m; i += 4) {
                    float p = data.pressure[j][i];
                    int x = i / 4, y = j / 4;
                    set_color(p);
                    glVertex2i(x, y);                }
            }
            glEnd();
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            return framebuffer_tex;
        }
    };


    class GameView {
    protected:
        /*压强图的纹理*/
        unique_ptr<PressureGraphRenderer> pressure_graph = nullptr;

        /*绘制沙子与绘制固体的bool值*/
        bool draw_sand;
        bool draw_iron;
        bool draw_water;

        /*画笔大小的选择*/
        bool brush_1pix;
        bool brush_5pix;
        bool brush_10pix;

        /*画笔功能的选择*/
        bool draw;
        bool inc_heat;
        bool dec_heat;

        /*窗口模式设定*/
        bool mode_draw;
        bool mode_heat;
        bool mode_pressure;

        /*更新ParticleBrush*/
        void UpdataParticles(const vec2& point);

        /*更新HeatBrush*/
        void UpdataParticlesHeat(const vec2& point);

    public:
        /*GLF窗口*/
        GLFWwindow* window;

        // 更新事件源（View通知ViewModel进行逻辑更新）
        EventSource<> event_update;

        // 新的事件源，包含一帧内的所有数据
        EventSource<FrameData> event_frame_ready;

        // 绘制新粒子事件源
        EventSource<ParticleBrush> event_new_particles;

        // 改变温度事件源
        EventSource<HeatBrush> event_change_heat;

        //framedata处理事件指针
        shared_ptr<EventHandler<FrameData>> on_frame_ready;

        GameView();

        ~GameView();

        //framedata处理事件
        void Handler(FrameData data);

        //构造窗口环境
        void OnCreate();

        //鼠标点击事件处理
        void MouseClickEvent();
    };
}

