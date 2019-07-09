#pragma once
#include "../common/event.h"
#include "../common/particle.h"
#include "imgui/imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/glew/GL/glew.h"
#include "imgui/glfw/include/GLFW/glfw3.h"
#include <iostream>

namespace T {
    using namespace std;

    class GameView {
    public:
		/*绘制沙子与绘制固体的bool值*/
		bool Draw_Sand = false;
		bool Draw_Iron = false;

        // 更新事件源（View通知ViewModel进行逻辑更新）
        EventSource<> event_update;

        // 绘制新粒子事件源（View通知ViewModel绘制新粒子）
        EventSource<ParticleBrush> event_new_particles;

        // 数据准备完毕的处理函数（ViewModel通知View数据准备完毕，可以绘制）
        shared_ptr<EventHandler<const vector<ParticleInfo>&>> on_data_ready;

        class DataReadyEventHandler : public EventHandler<const vector<ParticleInfo>&> {
            GameView* view;
        public:
            DataReadyEventHandler(GameView* view) : view(view) {}
            void handle(const vector<ParticleInfo>& particles) {
                view->Handler();
            }
        };

        void Handler() {

        }

        void Test() {
            on_data_ready = make_shared<DataReadyEventHandler>(this);
        }


        GameView();

		void Update_Event();

		void Updata_ParticleInfo();

	private:
		//shared_ptr<EventHandler<vector<string>>> Command;
    };

	class GameWindow :public GameView{
	public:
		GLFWwindow* window;
		GameWindow();
		~GameWindow();
		void OnCreate();
		void Draw();
	};
}