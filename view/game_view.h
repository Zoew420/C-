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

        // ��������id
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
        /*ѹǿͼ������*/
        unique_ptr<PressureGraphRenderer> pressure_graph = nullptr;

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

        /*����ParticleBrush*/
        void UpdataParticles(const vec2& point);

        /*����HeatBrush*/
        void UpdataParticlesHeat(const vec2& point);

    public:
        /*GLF����*/
        GLFWwindow* window;

        // �����¼�Դ��View֪ͨViewModel�����߼����£�
        EventSource<> event_update;

        // �µ��¼�Դ������һ֡�ڵ���������
        EventSource<FrameData> event_frame_ready;

        // �����������¼�Դ
        EventSource<ParticleBrush> event_new_particles;

        // �ı��¶��¼�Դ
        EventSource<HeatBrush> event_change_heat;

        //framedata�����¼�ָ��
        shared_ptr<EventHandler<FrameData>> on_frame_ready;

        GameView();

        ~GameView();

        //framedata�����¼�
        void Handler(FrameData data);

        //���촰�ڻ���
        void OnCreate();

        //������¼�����
        void MouseClickEvent();
    };
}

