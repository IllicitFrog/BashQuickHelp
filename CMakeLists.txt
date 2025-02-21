cmake_minimum_required(VERSION 3.14)
project(bashquickhelp)

set(C++_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(FTXUI_ENABLE_INSTALL OFF)

if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

file(GLOB SRC_FILES ${CMAKE_HOME_DIRECTORY}/src/*.cpp)

find_package(ftxui QUIET)

if(NOT ftxui_FOUND)
  include(FetchContent)
  set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
  FetchContent_Declare(
    ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
    GIT_TAG main # Important: Specify a version or a commit hash here.
    EXCLUDE_FROM_ALL
  )
  FetchContent_MakeAvailable(ftxui)
endif()

# Add executable
add_executable(${PROJECT_NAME} ${SRC_FILES})

include_directories(include)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
