cmake_minimum_required (VERSION 3.10)

set(src
	common/event
	common/particle
	model/game_model
	view/game_view
	view/game_drawParticles
	view/ViewEventHandler/game_DataReadyEventHandle
	view_model/game_view_model
)

set(tests
	test/test.cpp
	test/01_placeholder
)

set(src_visualizer
	visualizer/main
	visualizer/my3dpresent
)

set(targets
	Game
	TestEntry
	Visualizer
)

add_subdirectory(view/imgui)

add_executable (TestEntry ${src} ${tests})
add_executable (Game "game_app.cpp" ${src})
add_executable (Visualizer ${src} ${src_visualizer})

foreach(tg ${targets})
	set_property(TARGET ${tg} PROPERTY CXX_STANDARD 17)
	target_link_libraries(${tg} ImGui)
endforeach()

set_target_properties(TestEntry PROPERTIES LINK_FLAGS "/PROFILE")