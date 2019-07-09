#pragma once
#define _CRT_SECURE_NO_WARNINGS 
#include "../common/event.h"
#include "../common/particle.h"
#include "imgui/imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/glew/GL/glew.h"
#include "imgui/glfw/include/GLFW/glfw3.h"
#include "ViewEventHandler/game_DataReadyEventHandle.h"
#include "game_drawParticles.h"
#include <iostream>
#include <vector>

namespace T {
    using namespace std;

    class GameView {
    public:
		/*����ɳ������ƹ����boolֵ*/
		bool draw_sand;
		bool draw_iron;
		/*���ʴ�С��ѡ��*/
		bool brush_1pix;
		bool brush_5pix;
		bool brush_10pix;
		/*Iron��Sand���ʵ�����*/
		GLuint texture[1];

        // �����¼�Դ��View֪ͨViewModel�����߼����£�
        EventSource<> event_update;

        // �����������¼�Դ��View֪ͨViewModel���������ӣ�
        EventSource<ParticleBrush> event_new_particles;

        // ����׼����ϵĴ�������ViewModel֪ͨView����׼����ϣ����Ի��ƣ�
        shared_ptr<EventHandler<const vector<ParticleInfo>&>> on_data_ready;
		
		GameView();

		void Handler(const vector<ParticleInfo>& particles);

		void UpdataParticles(const vec2& point);

    };

	class GameWindow :public GameView{
	public:
		GLFWwindow* window;
		GameWindow();
		~GameWindow();
		void OnCreate();
		void MouseClickEvent();
	};
}