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
#include "ViewEventHandler/game_mouseclickEventHandle.h"
#include "game_drawParticles.h"
#include <iostream>
#include <vector>

namespace T {
    using namespace std;

    class GameView {
    public:
		/*绘制沙子与绘制固体的bool值*/
		bool Draw_Sand;
		bool Draw_Iron;
		/*Iron和Sand画笔的纹理*/
		GLuint texture[1];

        // 更新事件源（View通知ViewModel进行逻辑更新）
        EventSource<> event_update;

        // 绘制新粒子事件源（View通知ViewModel绘制新粒子）
        EventSource<ParticleBrush> event_new_particles;

        // 数据准备完毕的处理函数（ViewModel通知View数据准备完毕，可以绘制）
        shared_ptr<EventHandler<const vector<ParticleInfo>&>> on_data_ready;

		//鼠标点击时的处理函数
		shared_ptr<EventHandler<const vec2 &>> mouse_click;
		
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
		/*void Draw();*/
	};
}