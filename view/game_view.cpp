#include "game_view.h"


static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

T::GameWindow::GameWindow() {
	//错误回调函数
	glfwSetErrorCallback(glfw_error_callback);
	//初始化GLFW
	if (!glfwInit()) exit(EXIT_FAILURE);

	// GL 3.0 + GLSL 130
	const char *glsl_version = "#version 130";
	//配置GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	//创建窗口
	window = glfwCreateWindow(800, 600, "Simflow", NULL, NULL);
	if (window == NULL)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	//使用OpenGL API之前，必须设置OpenGL上下文环境
	glfwMakeContextCurrent(window);
	//设置交换间隔为1，vsync：要交换两个缓冲区之间的帧数
	glfwSwapInterval(1);

	//初始化 OpenGL Loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
	bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
	bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
	bool err = gladLoadGL() == 0;
#else
	bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
	if (err)
	{
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	IMGUI_CHECKVERSION();
	//设置默认字体
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//设置键盘输入文本
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	//设置ImGui模式为深色
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
}

T::GameWindow::~GameWindow()
{
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();
}

//显示帮助
static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void T::GameWindow::OnCreate()
{
	bool show_menu = true;
	while (!glfwWindowShouldClose(window)) {
		//渲染循环
		glfwSwapBuffers(window);
		glfwPollEvents();
		//开启ImGui框架
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGuiIO& io = ImGui::GetIO();

		if (show_menu) {
			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Appearing);
			ImGui::Begin("State Choose:", &show_menu, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::Text("State Choose:");
			ImGui::SameLine(); HelpMarker("Once can only choose one state to draw");
			ImGui::CheckboxFlags("Sand", (unsigned int*)&Draw_Sand, 1); ImGui::SameLine(150);
			ImGui::CheckboxFlags("Iron", (unsigned int*)&Draw_Iron, 1); ImGui::SameLine(300);
			ImGui::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y); ImGui::SameLine(550);
			ImGui::Text("Mouse down:");
			if (io.MouseDownDuration[0] >= 0.0f)
			{
				ImGui::SameLine();
				ImGui::Text("(%.02f secs)", io.MouseDownDuration[0]);
			}
			ImGui::End();
		}
		Draw();

		int display_w, display_h;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glClear(GL_COLOR_BUFFER_BIT);
		//绘制
		ImGui::Render();
		glViewport(0, 0, display_w, display_h);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}

void T::GameWindow::Draw()
{
	ImGuiIO& io = ImGui::GetIO();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION_MATRIX);
	glOrtho(-800 / 2, 800 / 2, -600 / 2, 600 / 2, -1000, 1000);

	glBegin(GL_LINES);
	GLfloat x, y;
	x = io.MousePos.x;
	y = io.MousePos.y;
	if (x >= 400 & y >= 300) {
		x -= 400; y = 300 - y;
	}
	else if (x >= 400 & y < 300) {//第一象限
		x -= 400; y = 300 - y;
	}
	else if (x < 400 & y < 300) {
		x -= 400; y = 300 - y;
	}
	else if (x < 400 & y > 300) {
		x -= 400; y = 300 - y;
	}
	DrawCircle(x, y, 1.0f, 10, texture[0]);
	DrawCircle(x + 10, y + 10, 1.0f, 10, texture[1]);
	glEnd();
}

T::GameView::GameView()
{
	texture[0] = load_texture("1.bmp");
	texture[1] = load_texture("2.bmp");
	on_data_ready = make_shared<DataReadyEventHandler>(this);
}

void T::GameView::Handler(const vector<ParticleInfo>& particles)
{
	for (int i = 0; i < particles.size(); i++)
	{
		ImGuiIO& io = ImGui::GetIO();
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION_MATRIX);
		glOrtho(-800 / 2, 800 / 2, -600 / 2, 600 / 2, -1000, 1000);
		if (particles[i].type == ParticleType::Iron)
		{
			float x = particles[i].position.x;
			float y = particles[i].position.y;
			DrawCircle(x, y, 1.0f, 10, texture[0]);
		}
	}
}

void T::GameView::DrawCircle(float cx, float cy, float r, int num_segments, GLuint texName)
{
	GLfloat vertex[4];
	GLfloat texcoord[2];

	const GLfloat delta_angle = 2.0 * PI / num_segments;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texName);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glBegin(GL_TRIANGLE_FAN);

	//draw the vertex at the center of the circle
	texcoord[0] = 0.5 + cx;
	texcoord[1] = 0.5 + cy;
	glTexCoord2fv(texcoord);
	vertex[0] = cx;
	vertex[1] = cy;
	vertex[2] = 0.0;
	vertex[3] = 1.0;
	glVertex4fv(vertex);

	//draw the vertex on the contour of the circle   
	for (int i = 0; i < num_segments; i++)
	{
		texcoord[0] = (std::cos(delta_angle * i) + 1.0) * 0.5 + cx;
		texcoord[1] = (std::sin(delta_angle * i) + 1.0) * 0.5 + cy;
		glTexCoord2fv(texcoord);

		vertex[0] = std::cos(delta_angle * i) * r + cx;
		vertex[1] = std::sin(delta_angle * i) * r + cy;
		vertex[2] = 0.0;
		vertex[3] = 1.0;
		glVertex4fv(vertex);
	}
	texcoord[0] = (1.0 + 1.0) * 0.5 + cx;
	texcoord[1] = (0.0 + 1.0) * 0.5 + cy;
	glTexCoord2fv(texcoord);

	vertex[0] = 1.0 * r + cx;
	vertex[1] = 0.0 * r + cy;
	vertex[2] = 0.0;
	vertex[3] = 1.0;
	glVertex4fv(vertex);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

