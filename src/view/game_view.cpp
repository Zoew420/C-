#include "game_view.h"
int display_w, display_h;

/*错误回调函数*/
static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

/*显示帮助*/
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
	mode_draw = true;
	mode_heat = false;
	mode_pressure = false;
	on_frame_ready = make_shared<FrameReadyEventHandler>(this);

	//错误回调函数
	glfwSetErrorCallback(glfw_error_callback);
	//初始化GLFW
	if (!glfwInit()) exit(EXIT_FAILURE);

	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	//配置GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	//创建窗口
	window = glfwCreateWindow(810, 512, "Simflow", NULL, NULL);
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
	io.Fonts->AddFontFromFileTTF("c:/Windows/Fonts/simhei.ttf", 13.0f, NULL, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());

	//设置ImGui模式为深色
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	pressure_graph = make_unique<PressureGraphRenderer>();
	heat_graph = make_unique<HeatGraphRenderer>();
}

Simflow::GameView::~GameView()
{
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Simflow::GameView::Handler(FrameData data)
{
	if (mode_pressure)
	{
		ImGui::SetNextWindowPos(ImVec2(515, 250), ImGuiCond_Appearing);
		ImGui::Begin("Pressure Graph", (bool *)(&mode_pressure), ImGuiWindowFlags_AlwaysAutoResize);
		GLuint tex = pressure_graph->render_texture(data);
		ImGui::Image((void*)tex, ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0));
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted("High pressure is red, Low pressure is blue");
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
		ImGui::End();
	}

	if (mode_heat)
	{
		ImGui::SetNextWindowPos(ImVec2(660, 250), ImGuiCond_Appearing);
		ImGui::Begin("Heat Graph", (bool *)(&mode_heat), ImGuiWindowFlags_AlwaysAutoResize);
		GLuint tex_heat = heat_graph->render_texture(data);
		ImGuiIO& io = ImGui::GetIO();
		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImGui::Image((void*)tex_heat, ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0));
		float my_tex_w = 128;
		float my_tex_h = 128;
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted("The temperature is higher, the color close to red more\nIt is lower, the color close to blue more");
			ImGui::PopTextWrapPos();
			float region_sz = 16.0f;
			float region_x = io.MousePos.x - pos.x - region_sz * 0.5f; if (region_x < 0.0f) region_x = 0.0f; else if (region_x > my_tex_w - region_sz) region_x = my_tex_w - region_sz;
			float region_y = io.MousePos.y - pos.y - region_sz * 0.5f; if (region_y < 0.0f) region_y = 0.0f; else if (region_y > my_tex_h - region_sz) region_y = my_tex_h - region_sz;
			float zoom = 4.0f;
			ImGui::Text("Min: (%.2f, %.2f)", region_x, region_y);
			ImGui::Text("Max: (%.2f, %.2f)", region_x + region_sz, region_y + region_sz);
			ImVec2 uv0 = ImVec2((region_x) / my_tex_w, 1 - (region_y) / my_tex_h);
			ImVec2 uv1 = ImVec2((region_x + region_sz) / my_tex_w, 1 - (region_y + region_sz) / my_tex_h);
			ImGui::Image((void*)tex_heat, ImVec2(region_sz * zoom, region_sz * zoom), uv0, uv1);
			ImGui::EndTooltip();
		}
		ImGui::End();
	}

	reset_matrix(display_w, display_h);

	for (int i = 0; i < data.particles.size(); i++)
	{
		float x = data.particles[i].position.x;
		float y = data.particles[i].position.y;
		float temperature = data.particles[i].temperature;
		DrawParticle(x, y, data.particles[i].type);
	}
}

void Simflow::GameView::OnCreate()
{
	bool control_pad = true;
	bool show_control_pad = false;
	bool show_mouse_state = true;
	bool show_user_guid = false;
	bool show_draw_item = true;
	bool show_brush_function = true;

	while (!glfwWindowShouldClose(window)) {
		//渲染循环
		glfwSwapBuffers(window);
		glfwPollEvents();
		//开启ImGui框架
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGuiIO& io = ImGui::GetIO();

		if (control_pad) {
			ImGui::SetNextWindowPos(ImVec2(710, 0), ImGuiCond_Appearing);
			ImGui::Begin("Control Pad", &control_pad, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(3 / 7.0f, 0.6f, 0.6f));
			if (ImGui::Button("Control Pad")) {
				show_control_pad = true;
				control_pad = false;
			}
			if (ImGui::BeginPopupContextWindow())
			{
				if (ImGui::MenuItem("About the game"))
				{
					show_user_guid = true;

				}
				if (control_pad && ImGui::MenuItem("Exit the game"))
				{
					exit(0);
				}
				ImGui::EndPopup();
			}
			ImGui::PopStyleColor(1);
			ImGui::End();
		}

		if (show_control_pad) {
			ImGui::SetNextWindowPos(ImVec2(515, 50), ImGuiCond_Always);
			ImGui::Begin("Control Pad", (bool *)(&show_control_pad), ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize);
			if (ImGui::Checkbox("Draw Item", &show_draw_item)); ImGui::SameLine();
			HelpMarker("Yes:Open the 'Draw Item' table\nNo:Close the table"); ImGui::SameLine();
			if (ImGui::Checkbox("Brush Fuction", &show_brush_function)); ImGui::SameLine();
			HelpMarker("Yes:Open the 'Brush Size'table\nNo:Close the table");
			if (ImGui::Checkbox("Heat Graph", &mode_heat)); ImGui::SameLine();
			HelpMarker("Yes:Show the 'Heat Graph'window\nNo:Close the window"); ImGui::SameLine();
			if (ImGui::Checkbox("Pressure Graph", &mode_pressure)); ImGui::SameLine();
			HelpMarker("Yes:Show the 'Heat Graph'window\nNo:Close the window");
			ImGui::Separator();

			if (ImGui::BeginTabBar("##TabBar", ImGuiTabBarFlags_AutoSelectNewTabs))
			{
				if (ImGui::BeginTabItem("Draw Item", (bool *)(&show_draw_item)))
				{
					ImGui::Text("Item:"); ImGui::SameLine();
					HelpMarker("Differnent item to draw, color shows below"); ImGui::SameLine(170);
					ImGui::Text("Brush Size:");  ImGui::SameLine();
					HelpMarker("Different size will influence the influenced region's size once you click");

					ImGui::ColorButton("Iron##3b", ImVec4(1, 1, 1, 0)); ImGui::SameLine();
					ImGui::CheckboxFlags("Iron", (unsigned int*)& draw_iron, 1); ImGui::SameLine(170);
					if (draw_iron == true)
					{
						draw_sand = false;
						draw_water = false;
					}
					ImGui::CheckboxFlags("1 pix", (unsigned int*)& brush_1pix, 1);
					if (brush_1pix == true) {
						brush_5pix = false;
						brush_10pix = false;
					}

					ImGui::ColorButton("Sand##3b", ImVec4(1, 1, 0, 0)); ImGui::SameLine();
					ImGui::CheckboxFlags("Sand", (unsigned int*)& draw_sand, 1); ImGui::SameLine(170);
					if (draw_sand == true)
					{
						draw_iron = false;
						draw_water = false;
					}
					ImGui::CheckboxFlags("5 pix", (unsigned int*)& brush_5pix, 1);
					if (brush_5pix == true) {
						brush_1pix = false;
						brush_10pix = false;
					}

					ImGui::ColorButton("Water##3b", ImVec4(0, 1, 1, 0)); ImGui::SameLine();
					ImGui::CheckboxFlags("Water", (unsigned int*)& draw_water, 1); ImGui::SameLine(170);
					if (draw_water == true)
					{
						draw_iron = false;
						draw_sand = false;
					}
					ImGui::CheckboxFlags("10 pix", (unsigned int*)& brush_10pix, 1);
					if (brush_10pix == true) {
						brush_1pix = false;
						brush_5pix = false;
					}

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Brush Function", (bool *)(&show_brush_function)))
				{
					HelpMarker("the first one means once you click it will add an item\nthe second one means once you click the particle's temperature will increase\nthe last one is the same reason");
					ImGui::CheckboxFlags("Painting Pen", (unsigned int*)& draw, 1);
					if (draw == true) {
						inc_heat = false;
						dec_heat = false;
					}
					ImGui::CheckboxFlags("Increase Heat", (unsigned int*)& inc_heat, 1);
					if (inc_heat == true) {
						draw = false;
						dec_heat = false;
					}
					ImGui::CheckboxFlags("Decrease Heat", (unsigned int*)& dec_heat, 1);
					if (dec_heat == true) {
						draw = false;
						inc_heat = false;
					}
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Exit"))
				{
					ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0, 0.6f, 0.6f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0, 0.7f, 0.7f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0, 0.8f, 0.8f));
					if (ImGui::Button("Exit the game", ImVec2(285, 75)))
					{
						exit(0);
					}
					ImGui::PopStyleColor(3);
				}

				ImGui::EndTabBar();
			}
			ImGui::End();
		}
		else {
			control_pad = true;
		}

		if (show_user_guid)
		{
			ImGui::SetNextWindowPos(ImVec2(150, 100), ImGuiCond_Appearing);
			ImGui::Begin("User Guide:", &show_user_guid, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::BulletText("This game is to simulate three physical object,\ntheir physical attribute will be considered when they move.");

			ImGui::BulletText("After Opening the control pad,\nchoose different attribute to show what you want to look at,\nyou must choose an item first!");
			ImGui::BulletText("Some Tips");
			ImGui::Indent();
			ImGui::BulletText("Double-click on title bar to collapse window.");
			ImGui::BulletText("ALT+F4 can be used to exit the console.");
			ImGui::Unindent();
		}

		if (show_mouse_state) {
			ImGui::SetNextWindowPos(ImVec2(485, 492), ImGuiCond_Appearing);
			ImGui::Begin("Mouse state:", &show_mouse_state, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
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

		event_update.trigger();

		MouseClickEvent();
		//绘制
		ImGui::Render();
		glViewport(0, 0, display_w, display_h);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}

void Simflow::GameView::MouseClickEvent()
{
	ImGuiIO& io = ImGui::GetIO();
	if (io.MouseDownDuration[0] >= 0.0f) {
		GLfloat x, y;
		x = io.MousePos.x;
		y = io.MousePos.y;
		if (x >= 0 && x <= 512) {
			vec2 p;
			p.x = x; p.y = y;
			if (draw == true)  UpdataParticles(p);
			else UpdataParticlesHeat(p);
		}
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
