cmake_minimum_required(VERSION 3.16)
project(StudentStudyGroupManager VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_add_executable(StudentStudyGroupManager
    main.cpp
    mainwindow.cpp       mainwindow.h       mainwindow.ui
    studentmanager.cpp   studentmanager.h
    groupmanager.cpp     groupmanager.h
    eventmanager.cpp     eventmanager.h
    taskmanager.cpp      taskmanager.h
    filehandler.cpp      filehandler.h
)

target_link_libraries(StudentStudyGroupManager PRIVATE Qt6::Widgets)
set_target_properties(StudentStudyGroupManager PROPERTIES WIN32_EXECUTABLE TRUE)
