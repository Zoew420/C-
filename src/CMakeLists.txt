cmake_minimum_required (VERSION 3.10)

set(src
	common/array2d
	common/event
	common/parameter
	common/particle
	model/game_model
	model/air_solver
	view/game_view
	view/draw_particle
	view/event_handler/frame_ready
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

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
if(COMPILER_SUPPORTS_MARCH_NATIVE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif()

foreach(tg ${targets})
	set_property(TARGET ${tg} PROPERTY CXX_STANDARD 17)
	target_link_libraries(${tg} ImGui)
endforeach()