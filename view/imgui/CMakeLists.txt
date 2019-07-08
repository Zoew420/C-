cmake_minimum_required (VERSION 3.10)

find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})

set(src 
	imgui_impl_glfw
	imgui_impl_opengl3
	usynergy/uSynergy
	glew/GL/wglew
	glew/GL/glew
	glew/GL/eglew
	glew/GL/glxew
	gl3w/GL/gl3w
	imgui/imgui
	imgui/imgui_demo
	imgui/imgui_draw
	imgui/imgui_widgets
)

set(copy_files
	lib/glew32.dll
)

add_library(ImGui ${src})

foreach(file ${copy_files})
	add_custom_command(
		TARGET ImGui POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
				${CMAKE_CURRENT_SOURCE_DIR}/${file}
				${CMAKE_BINARY_DIR})
endforeach()

target_link_libraries(ImGui ${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw3.lib)
target_link_libraries(ImGui opengl32)
target_link_libraries(ImGui ${CMAKE_CURRENT_SOURCE_DIR}/lib/legacy_stdio_definitions.lib)
target_link_libraries(ImGui ${CMAKE_CURRENT_SOURCE_DIR}/lib/glew32.lib)