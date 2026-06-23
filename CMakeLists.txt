cmake_minimum_required(VERSION 3.14)
project(Sculpture_Player_30 LANGUAGES C CXX)

find_package(fmt REQUIRED)

find_package(OpenCV REQUIRED COMPONENTS
  core
  imgproc
  imgcodecs
  videoio
)

find_package(SDL2 REQUIRED)
find_package(OpenMP REQUIRED COMPONENTS CXX)
find_package(PkgConfig REQUIRED)

pkg_check_modules(GTKMM4 REQUIRED gtkmm-4.0)
pkg_check_modules(GTK4 REQUIRED gtk4)

add_executable(Sculpture_Player_30
  main.cpp
  ControlPanelWindow.cpp

  function_misc.cpp
  fs_utils.cpp
  player_class_Mat.cpp
  image_processor.cpp
  measure2.cpp
  sculpture.cpp
  ftdi_local.cpp
  sdl2_local.cpp
)

set_target_properties(Sculpture_Player_30 PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
)

target_include_directories(Sculpture_Player_30 PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/rapidjson/include
  ${GTKMM4_INCLUDE_DIRS}
  ${GTK4_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
  ${SDL2_INCLUDE_DIRS}
)

target_link_libraries(Sculpture_Player_30 PRIVATE
  ${GTKMM4_LINK_LIBRARIES}
  ${GTK4_LINK_LIBRARIES}
  ${OpenCV_LIBS}
  ${SDL2_LIBRARIES}
  OpenMP::OpenMP_CXX
  fmt::fmt
)

find_library(FTD2XX_LIB ftd2xx
  PATHS
    /usr/local/lib
    /usr/lib
    /opt/homebrew/lib
)

if(FTD2XX_LIB)
  target_link_libraries(Sculpture_Player_30 PRIVATE ${FTD2XX_LIB})
else()
  message(WARNING "libftd2xx not found; continuing without it")
endif()
