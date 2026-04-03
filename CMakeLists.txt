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
    3rd/godot/thirdparty/vulkan/include
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
    "src/*.h"
    "src/*.hpp"
)

# 收集 qml 文件用于自动重新拷贝
file(GLOB_RECURSE QML_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qml/*.qml"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qml/*.js"
)

# 收集 project 文件用于自动重新拷贝
file(GLOB_RECURSE PROJECT_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/project/*.gd"
    "${CMAKE_CURRENT_SOURCE_DIR}/project/*.tscn"
    "${CMAKE_CURRENT_SOURCE_DIR}/project/*.tres"
)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin")

add_executable(${EXECUTABLE_NAME}
    ${SRC_FILES}
)

target_link_libraries(${EXECUTABLE_NAME}
    ${Boost_LIBRARIES}
    libgodot-cpp.windows.template_debug.x86_64
    godot.windows.template_debug.dev.x86_64.dll
    Qt6::Core
    Qt6::Widgets
    Qt6::Quick
)

if(WIN32 AND WINDEPLOYQT_EXECUTABLE)
    # 构建后执行 windeployqt
    add_custom_command(
        TARGET ${EXECUTABLE_NAME}
        POST_BUILD
        COMMAND ${WINDEPLOYQT_EXECUTABLE}
        $<$<CONFIG:Debug>:--debug>
        $<$<CONFIG:Debug>:--pdb>
        $<$<CONFIG:Release>:--release>
        --qml
        --no-translations
        $<TARGET_FILE:${EXECUTABLE_NAME}>
        COMMENT "Deploying Qt dependencies using windeployqt..."
    )

    # 拷贝 qml 目录到运行目录
    add_custom_command(
        TARGET ${EXECUTABLE_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/src/qml
        $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/qml/GodotQt
        COMMENT "Copying qml directory to runtime directory..."
    )

    # 拷贝 project 目录到运行目录
    add_custom_command(
        TARGET ${EXECUTABLE_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/project
        $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/project
        COMMENT "Copying project directory to runtime directory..."
    )

    # 设置源文件依赖，确保文件变化时重新拷贝
    add_custom_command(
        TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Checking qml changes..."
        DEPENDS ${QML_FILES} ${PROJECT_FILES}
    )
endif()