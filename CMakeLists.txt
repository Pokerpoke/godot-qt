cmake_minimum_required(VERSION 3.25)

project(godot-qt)

set(EXECUTABLE_NAME ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS filesystem)
find_package(Qt6 REQUIRED COMPONENTS
    Core Widgets Quick)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 查找 windeployqt 可执行文件
find_program(WINDEPLOYQT_EXECUTABLE windeployqt
    HINTS ${QT_BINARY_DIR} # Qt bin 目录
    PATHS $ENV{QTDIR}/bin # 环境变量中的 Qt 路径
)

if(NOT WINDEPLOYQT_EXECUTABLE)
    message(WARNING "windeployqt not found! Qt dependencies may not be deployed.")
endif()

include_directories(
    3rd/godot/
    3rd/godot-cpp/include
    3rd/godot-cpp/gen/include
    include
    ${Boost_INCLUDE_DIRS}
)

link_directories(
    ${Boost_LIBRARY_DIRS}
    build/godot/lib
    3rd/godot-cpp/bin
)

file(GLOB SRC_FILES
    "src/*.cpp"
    "src/*.cc"
)

add_executable(${EXECUTABLE_NAME}
    ${SRC_FILES}
)

target_link_libraries(${EXECUTABLE_NAME}
    ${Boost_LIBRARIES}
    libgodot-cpp.windows.template_debug.x86_64

    # godot.windows.template_debug.x86_64
    godot.windows.template_debug.dev.x86_64.dll

    Qt6::Core
    Qt6::Widgets
    Qt6::Quick
)

if(WIN32 AND WINDEPLOYQT_EXECUTABLE)
    # 构建后执行 windeployqt
    add_custom_command(
        TARGET godot-qt
        POST_BUILD
        COMMAND ${WINDEPLOYQT_EXECUTABLE}

        # 根据构建类型传递参数
        $<$<CONFIG:Debug>:--debug>
        $<$<CONFIG:Release>:--release>

        # 强制覆盖已存在的文件
        # --force
        --qml

        # --qmldir ${CMAKE_CURRENT_SOURCE_DIR}/src/qml

        # 可执行文件路径
        $<TARGET_FILE:godot-qt>

        # 输出执行信息
        COMMENT "Deploying Qt dependencies using windeployqt..."
    )
endif()