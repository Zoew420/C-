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
		
        // 更新事件源（View通知ViewModel进行逻辑更新）
        EventSource<> event_update;

        // 绘制新粒子事件源（View通知ViewModel绘制新粒子）
        EventSource<ParticleBrush> event_new_particles;

        // 改变温度事件源
        EventSource<HeatBrush> event_change_heat;

        // 数据准备完毕的处理函数（ViewModel通知View数据准备完毕，可以绘制）
        shared_ptr<EventHandler<const vector<ParticleInfo>&>> on_data_ready;
		
		//温度数据准备完毕的处理函数
		shared_ptr<EventHandler<const vector<ParticleInfo>&>> on_heat_ready;
		
		//压强数据准备完毕的处理函数
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