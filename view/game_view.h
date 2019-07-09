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
		/*����ɳ������ƹ����boolֵ*/
		bool Draw_Sand = false;
		bool Draw_Iron = false;

        // �����¼�Դ��View֪ͨViewModel�����߼����£�
        EventSource<> event_update;

        // �����������¼�Դ��View֪ͨViewModel���������ӣ�
        EventSource<ParticleBrush> event_new_particles;

        // ����׼����ϵĴ�������ViewModel֪ͨView����׼����ϣ����Ի��ƣ�
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