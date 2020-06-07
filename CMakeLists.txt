cmake_minimum_required(VERSION 3.14)
project(contextinfo LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Qml)
find_package(Qt6 COMPONENTS Quick)

add_qt_gui_executable(nodestuff
    main.cpp gui.cpp gui.h
    graph.cpp graph.h nodetypes.h portdata.h nodeconstructors.cpp nodeconstructors.h grapheval.cpp grapheval.h
    qrhiimgui.cpp qrhiimgui.h qrhiimgui_p.h
    imgui/imgui.cpp imgui/imgui_demo.cpp imgui/imgui_draw.cpp imgui/imgui_widgets.cpp
    imnodes/imnodes.cpp
)
target_include_directories(nodestuff PUBLIC
    imgui
    imnodes
)
target_compile_definitions(nodestuff PUBLIC
    _CRT_SECURE_NO_WARNINGS
)
target_link_libraries(nodestuff PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
    Qt::GuiPrivate
)

set(nodestuff_resource_files
    "main.qml"
    "imgui.vert.qsb"
    "imgui.frag.qsb"
)

qt6_add_resources(nodestuff "nodestuff"
    PREFIX
        "/"
    FILES
        ${nodestuff_resource_files}
)
