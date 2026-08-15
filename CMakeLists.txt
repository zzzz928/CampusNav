cmake_minimum_required(VERSION 3.20)
project(CampusNav)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt 配置
set(CMAKE_PREFIX_PATH "D:/QT/6.11.1/mingw_64")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)


add_executable(CampusNav
        main.cpp
        mainwindow.cpp
        mapwidget.cpp
        graph.cpp
        file.cpp
        admin.cpp
)

target_link_libraries(CampusNav Qt6::Widgets)

# 复制 data 文件夹到构建目录
if(EXISTS ${CMAKE_SOURCE_DIR}/data)
    file(COPY ${CMAKE_SOURCE_DIR}/data DESTINATION ${CMAKE_BINARY_DIR})
endif()
