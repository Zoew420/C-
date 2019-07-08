cmake_minimum_required (VERSION 3.10)

set(src
	common/event
	common/particle
	model/game_model
	view/game_view
	view_model/game_view_model
)

set(tests
	test/01_placeholder
)

set(targets
	Game
	TestEntry
)

add_subdirectory(view/imgui)

add_executable (TestEntry "test/test.cpp" ${src} ${tests})
add_executable (Game "game_app.cpp" ${src})

foreach(tg ${targets})
	set_property(TARGET ${tg} PROPERTY CXX_STANDARD 17)
	target_link_libraries(${tg} ImGui)
endforeach()