#pragma once
#define _CRT_SECURE_NO_WARNINGS 
#include "../common/event.h"
#include "../common/particle.h"
#include "imgui/imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/glew/GL/glew.h"
#include "imgui/glfw/include/GLFW/glfw3.h"
#include "ViewEventHandler/DataReadyEventHandle.h"
#include "loadBrushTexture.h"
#include <iostream>
#include <vector>
#define PI       3.14159265358979323846
/*Iron��Sand���ʵ�����*/
GLuint texture[1];

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

        GameView();

		void Handler(const vector<ParticleInfo>& particles);

		void DrawCircle(float cx, float cy, float r, int num_segments, GLuint texName);
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