cmake_minimum_required(VERSION 3.16)

# ============================================================================
# Toolchain file for building "valve-tuner" with Qt 6 + MinGW on Windows
#
# Использование на другом компьютере:
#   1. Установить Qt (например, 6.9.2 MinGW 64-bit).
#   2. Указать путь к Qt через переменную QT_ROOT (или поправить значение ниже).
#   3. Конфигурировать проект так:
#        cmake -S . -B build ^
#              -DCMAKE_TOOLCHAIN_FILE=toolchain.cmake
#
#   В Qt Creator / CLion:
#     - добавить CMake-профиль и в поле "Toolchain file" указать этот файл.
# ============================================================================

# ----------------------------------------------------------------------------
# Базовая информация о целевой системе
# ----------------------------------------------------------------------------
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_VERSION 10)

# ----------------------------------------------------------------------------
# Путь к Qt
#   Можно переопределить при вызове CMake:
#     cmake -DQT_ROOT=C:/Qt/6.9.2/mingw_64 ...
# ----------------------------------------------------------------------------
if(NOT DEFINED QT_ROOT)
    # Значение по умолчанию — такое же, как на текущем компьютере автора
    set(QT_ROOT "C:/Qt/6.9.2/mingw_64")
endif()

set(Qt6_DIR "${QT_ROOT}/lib/cmake/Qt6")
set(CMAKE_PREFIX_PATH "${QT_ROOT};${CMAKE_PREFIX_PATH}")

# ----------------------------------------------------------------------------
# Компиляторы MinGW (можно переопределить при необходимости)
# ----------------------------------------------------------------------------
set(CLION_SDK_DIR "C:/Program Files/JetBrains/CLion 2025.3/bin")

if(NOT DEFINED CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER "${CLION_SDK_DIR}/mingw/bin/gcc.exe")
endif()

if(NOT DEFINED CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER "${CLION_SDK_DIR}/mingw/bin/g++.exe")
endif()

# ----------------------------------------------------------------------------
# Общие флаги (по желанию можно расширять)
# ----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


