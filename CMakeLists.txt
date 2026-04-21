cmake_minimum_required(VERSION 3.10)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

project(Lab4_OS)

# Лаба требует C++98
set(CMAKE_CXX_STANDARD 98)

# Тотальное отключение предупреждений MSVC, чтобы не было ошибки C2220
if(MSVC)
    add_compile_options(/W0 /WX-)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# --- 1. Загрузка Google Test ---
include(FetchContent)

# Отключаем всё лишнее, чтобы GTest просто тихо собрался
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)

# Используем 1.10.0 (в ней убрали жесткий форс ошибки C2220)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.10.0.zip
)
FetchContent_MakeAvailable(googletest)

# Принудительно отключаем ошибки в скачанных файлах GTest
if(TARGET gtest)
    target_compile_options(gtest PRIVATE /W0 /WX-)
    target_compile_options(gtest_main PRIVATE /W0 /WX-)
endif()

# --- 2. Настройка путей ---
include_directories(headers)
# Обрати внимание: в 1.10.0 путь к include немного другой
include_directories(${googletest_SOURCE_DIR}/googletest/include)

# --- 3. RECEIVER ---
add_executable(receiver
    src/Receiver.cpp
    src/SharedQueue.cpp
)

# --- 4. SENDER ---
add_executable(sender
    src/Sender.cpp
    src/SharedQueue.cpp
)

# --- 5. TESTS ---
add_executable(tests
    tests/test_reciever.cpp
    tests/test_sender.cpp
    tests/test_shared_queue.cpp
    src/SharedQueue.cpp
)

# Связываем тесты с библиотекой Google Test
target_link_libraries(tests PRIVATE gtest_main)