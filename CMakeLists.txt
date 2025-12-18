cmake_minimum_required(VERSION 3.16)

project(valve-tuner VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(Qt6_DIR C:/Qt/6.9.2/mingw_64/lib/cmake/Qt6)

find_package(Qt6 6.9 REQUIRED COMPONENTS Quick SerialPort)

qt_standard_project_setup(REQUIRES 6.9)

qt_add_executable(appvalve-tuner
        src/main.cpp
        src/Controller.cpp
        src/CRC.cpp
)

target_include_directories(appvalve-tuner PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

qt_add_qml_module(appvalve-tuner
        URI ValveTuner
        VERSION 1.0
        QML_FILES
        Main.qml
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appvalve-tuner PROPERTIES
        #    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appvalve-tuner
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
)

target_link_libraries(appvalve-tuner PRIVATE
        Qt6::Quick
        Qt6::SerialPort
)

include(GNUInstallDirs)
install(TARGETS appvalve-tuner
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
