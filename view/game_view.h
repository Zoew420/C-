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
//#include "event_handler/data_ready.h"
//#include "event_handler/heat_ready.h"
//#include "event_handler/pressure_ready.h"
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
            create_framebuffer(&framebuffer, &framebuffer_tex, 512, 512);
        }

        // ��������id
        GLuint render_texture(const FrameData& data) {
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
            int m = data.pressure.width();
            int n = data.pressure.height();
            reset_matrix(512, 512);
            glClear(GL_COLOR_BUFFER_BIT);
            for (int j = 0; j < n; j++) {
                for (int i = 0; i < m; i++) {
                    float p = data.pressure[j][i];
                    int x = i, y = j;
                    DrawPressure(x, y, p);
                }
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            return framebuffer_tex;
        }
    };



    class GameView {
    protected:
        unique_ptr<PressureGraphRenderer> pressure_graph = nullptr;

    public:
        /*����ɳ������ƹ����boolֵ*/
        bool draw_sand;
        bool draw_iron;
        bool draw_water;
        /*���ʴ�С��ѡ��*/
        bool brush_1pix;
        bool brush_5pix;
        bool brush_10pix;
        /*���ʹ��ܵ�ѡ��*/
        bool draw;
        bool inc_heat;
        bool dec_heat;
        /*����ģʽ�趨*/
        bool mode_draw;
        bool mode_heat;
        bool mode_pressure;

        // �����¼�Դ��View֪ͨViewModel�����߼����£�
        EventSource<> event_update;

        // TODO: �µ��¼�Դ������һ֡�ڵ���������
        EventSource<FrameData> event_frame_ready;

        // �����������¼�Դ��View֪ͨViewModel���������ӣ�
        EventSource<ParticleBrush> event_new_particles;

        // �ı��¶��¼�Դ
        EventSource<HeatBrush> event_change_heat;

        shared_ptr<EventHandler<FrameData>> on_frame_ready;

        GameView();

        void Handler(FrameData data);

        void UpdataParticles(const vec2& point);

        void UpdataParticlesHeat(const vec2& point);
    };

    class GameWindow : public GameView {
    public:
        GLFWwindow* window;
        GameWindow();
        ~GameWindow();
        void OnCreate();
        void MouseClickEvent();
    };
}

