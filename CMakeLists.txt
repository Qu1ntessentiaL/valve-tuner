cmake_minimum_required(VERSION 3.16)

project(valve-tuner VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.9 REQUIRED COMPONENTS Quick SerialPort)

qt_standard_project_setup(REQUIRES 6.9)

qt_add_executable(appvalve-tuner
        src/main.cpp
        src/Controller.cpp
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
)

target_link_libraries(appvalve-tuner PRIVATE
        Qt6::Quick
        Qt6::SerialPort
)

if(WIN32)
    get_filename_component(QT_ROOT "${Qt6_DIR}/../../.." ABSOLUTE)

    add_custom_command(TARGET appvalve-tuner POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E env
            "QT_PLUGIN_PATH=${QT_ROOT}/plugins"
            "QT_QPA_PLATFORM_PLUGIN_PATH=${QT_ROOT}/plugins/platforms"
            "PATH=${QT_ROOT}/bin"
            "${QT_ROOT}/bin/windeployqt.exe"
            --no-translations
            "$<TARGET_FILE:appvalve-tuner>"
            COMMENT "Running windeployqt after build"
    )
endif()

include(GNUInstallDirs)
install(TARGETS appvalve-tuner
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
