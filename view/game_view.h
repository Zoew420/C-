#pragma once
#define _CRT_SECURE_NO_WARNINGS 
#include "../common/event.h"
#include "../common/particle.h"
#include "imgui/imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/glew/GL/glew.h"
#include "imgui/glfw/include/GLFW/glfw3.h"
#include "event_handler/data_ready.h"
#include "event_handler/heat_ready.h"
#include "event_handler/pressure_ready.h"
#include "draw_particle.h"
#include <iostream>
#include <vector>

namespace Simflow {
    using namespace std;

    class GameView {
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

        // �����������¼�Դ��View֪ͨViewModel���������ӣ�
        EventSource<ParticleBrush> event_new_particles;

        // �ı��¶��¼�Դ
        EventSource<HeatBrush> event_change_heat;

        // ����׼����ϵĴ�������ViewModel֪ͨView����׼����ϣ����Ի��ƣ�
        shared_ptr<EventHandler<const vector<ParticleInfo>&>> on_data_ready;
		
		//�¶�����׼����ϵĴ�����
		shared_ptr<EventHandler<const vector<ParticleInfo>&>> on_heat_ready;
		
		//ѹǿ����׼����ϵĴ�����
		shared_ptr<EventHandler<const vector<vector<float>>>> on_pressure_ready;
		
		GameView();

		void Handler_Data(const vector<ParticleInfo>& particles);

		void Handler_Heat(const vector<ParticleInfo>& heat);

		void Handler_Pressure(const vector<vector<float>> pressure);

		void UpdataParticles(const vec2& point);

		void UpdataParticlesHeat(const vec2& point);
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