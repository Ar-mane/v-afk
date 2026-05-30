cmake_minimum_required(VERSION 3.16)

project(V-AFK)

set(CMAKE_CXX_STANDARD 17)

set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui")

if (NOT EXISTS "${IMGUI_DIR}/imgui.cpp")
    message(STATUS "ImGui not found in external/imgui, fetching with FetchContent...")

    include(FetchContent)

    FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG master
        GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(imgui)

    set(IMGUI_DIR "${imgui_SOURCE_DIR}")
endif()

add_executable(V-AFK
    src/main.cpp
    src/ui/main_view.cpp
    src/bot.cpp
    src/app_icon.rc

    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp

    ${IMGUI_DIR}/backends/imgui_impl_dx11.cpp
    ${IMGUI_DIR}/backends/imgui_impl_win32.cpp
)

target_include_directories(V-AFK PRIVATE
    src

    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

target_link_libraries(V-AFK PRIVATE
    d3d11
)

target_compile_definitions(V-AFK PRIVATE
    UNICODE
    _UNICODE
)

set_target_properties(V-AFK PROPERTIES
    WIN32_EXECUTABLE TRUE
)

if (EXISTS "${CMAKE_SOURCE_DIR}/assets")
    add_custom_command(TARGET V-AFK POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:V-AFK>/assets"
        COMMENT "Copy assets next to V-AFK.exe")
endif()