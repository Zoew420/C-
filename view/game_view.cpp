#include "game_view.h"


static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

Simflow::GameWindow::GameWindow() {
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

Simflow::GameWindow::~GameWindow()
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

void Simflow::GameWindow::OnCreate()
{
	bool show_state_choice = true;
	bool show_brush_choice = true;
	bool show_brush_func = true;
	bool show_mouse_state = true;
	bool exit_button = true;

	while (!glfwWindowShouldClose(window)) {
		//渲染循环
		glfwSwapBuffers(window);
		glfwPollEvents();
		//开启ImGui框架
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGuiIO& io = ImGui::GetIO();

		bool window_flag = show_state_choice & show_brush_choice & show_brush_func;
		if (window_flag) {
			ImGui::SetNextWindowPos(ImVec2(0, 5), ImGuiCond_Appearing);
			ImGui::Begin("Brush Function Choose:", &window_flag, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Brush Type Choose"))
				{
					ImGui::CheckboxFlags("Sand", (unsigned int*)&draw_sand, 1); ImGui::SameLine(150);
					if (draw_sand == true)
					{
						draw_iron = false;
						draw_water = false;
					}
					ImGui::CheckboxFlags("Iron", (unsigned int*)&draw_iron, 1); ImGui::SameLine(300);
					if (draw_iron == true) 
					{
						draw_sand = false;
						draw_water = false;
					}
					ImGui::CheckboxFlags("Water", (unsigned int*)&draw_water, 1);
					if (draw_water == true)
					{
						draw_iron = false;
						draw_sand = false;
					}
					ImGui::EndMenu();
				}
				ImGui::SameLine(150);
				if (ImGui::BeginMenu("Brush Size Choose"))
				{
					ImGui::CheckboxFlags("1 pix", (unsigned int*)&brush_1pix, 1); ImGui::SameLine(150);
					if (brush_1pix == true) {
						brush_5pix = false;
						brush_10pix = false;
					}
					ImGui::CheckboxFlags("5 pix", (unsigned int*)&brush_5pix, 1); ImGui::SameLine(300);
					if (brush_5pix == true) {
						brush_1pix = false;
						brush_10pix = false;
					}
					ImGui::CheckboxFlags("10 pix", (unsigned int*)&brush_10pix, 1);
					if (brush_10pix == true) {
						brush_1pix = false;
						brush_5pix = false;
					}
					ImGui::EndMenu();
				}
				ImGui::SameLine(300);
				if (ImGui::BeginMenu("Brush Function Choose"))
				{
					ImGui::CheckboxFlags("Painting Pen", (unsigned int*)&draw, 1); ImGui::SameLine(150);
					if (draw == true) {
						inc_heat = false;
						dec_heat = false;
					}
					ImGui::CheckboxFlags("Increase Heat", (unsigned int*)&inc_heat, 1); ImGui::SameLine(300);
					if (inc_heat == true) {
						draw = false;
						dec_heat = false;
					}
					ImGui::CheckboxFlags("Decrease Heat", (unsigned int*)&dec_heat, 1);
					if (dec_heat == true) {
						draw = false;
						dec_heat = false;
					}
					ImGui::EndMenu();
				}
				ImGui::SameLine(745);
				if (ImGui::BeginMenu("Exit")) exit(0);
				ImGui::EndMenuBar();
			}
			ImGui::End();
		}


		if (show_mouse_state) {
			ImGui::SetNextWindowPos(ImVec2(475, 580), ImGuiCond_Appearing);
			ImGui::Begin("Mouse state:", &show_state_choice, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y); ImGui::SameLine();
			ImGui::Text("Click Last:");
			if (io.MouseDownDuration[0] >= 0.0f)
			{
				ImGui::SameLine();
				ImGui::Text("(%.02f secs)", io.MouseDownDuration[0]);
			}
			else {
				ImGui::SameLine();
				ImGui::Text("(0 secs)");
			}
			ImGui::End();
		}

		int display_w, display_h;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glClear(GL_COLOR_BUFFER_BIT);
		event_update.trigger();
		MouseClickEvent();
		//绘制
		ImGui::Render();
		glViewport(0, 0, display_w, display_h);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}

void Simflow::GameWindow::MouseClickEvent()
{
	ImGuiIO& io = ImGui::GetIO();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glOrtho(-800 / 2, 800 / 2, -600 / 2, 600 / 2, -1000, 1000);
	if (io.MouseDownDuration[0] >= 0.0f) {
		GLfloat x, y;
		x = io.MousePos.x;
		y = io.MousePos.y;
        if (y >= 50 && y <= 600) {
            vec2 lp_p;
            lp_p.x = x; lp_p.y = y;
			if(draw == true)  UpdataParticles(lp_p);
			else UpdataParticlesHeat(lp_p);
        }
	}
}

Simflow::GameView::GameView()
{
	draw_iron = false;
	draw_sand = false;
	draw_water = false;
	brush_1pix = false;
	brush_5pix = true;
	brush_10pix = false;
	draw = true;
	inc_heat = false;
	dec_heat = false;
	on_data_ready = make_shared<DataReadyEventHandler>(this);
}

void Simflow::GameView::Handler(const std::vector<ParticleInfo>& particles)
{
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glOrtho(-800 / 2, 800 / 2, -600 / 2, 600 / 2, -1000, 1000);
	for (int i = 0; i < particles.size(); i++)
	{

		float x = particles[i].position.x;
		float y = particles[i].position.y;
		x -= 400; y = 300 - y;
        DrawParticle(x, y, particles[i].type);
	}
}

void Simflow::GameView::UpdataParticles(const glm::vec2 & point)
{
	ParticleBrush b;
	if (draw_iron) b.type = ParticleType::Iron;
	else if (draw_sand) b.type = ParticleType::Sand;
	else if (draw_water) b.type = ParticleType::Water;
	if (brush_1pix) b.radius = 1.0f;
	else if (brush_5pix) b.radius = 5.0f;
	else if (brush_10pix) b.radius = 10.0f;
	b.center = point;
	event_new_particles.trigger(b);
}

void Simflow::GameView::UpdataParticlesHeat(const glm::vec2 & point)
{
	HeatBrush h;
	if (brush_1pix) h.radius = 1.0f;
	else if (brush_5pix) h.radius = 5.0f;
	else if (brush_10pix) h.radius = 10.0f;
	h.center = point;
	if (inc_heat == true) h.increase = true;
	else if (dec_heat == true) h.increase = false;
	event_change_heat.trigger(h);
}
