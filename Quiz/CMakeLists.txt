cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

### Project ##################################################################

project(Quiz-dev)

# Custom CMake utilities
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(FormatOutputName)

find_package(Qt5 5.12 REQUIRED COMPONENTS Widgets Xml)

### Files ####################################################################

list(APPEND Quiz_FORMS
  forms/WMainWindow.ui
  forms/WQuestion.ui
)

list(APPEND Quiz_HEADERS
  include/Data.h
  include/Image.h
  include/QuestionsModel.h
  include/Util.h
  include/WImageViewer.h
  include/WMainWindow.h
  include/WQuestion.h
)

list(APPEND Quiz_SOURCES
  src/Data.cpp
  src/Image.cpp
  src/QuestionsModel.cpp
  src/Util.cpp
  src/WImageViewer.cpp
  src/WMainWindow.cpp
  src/WQuestion.cpp
  src/main.cpp
)

### Targets ##################################################################

add_executable(Quiz WIN32)

format_output_name(Quiz "Quiz")

set_target_properties(Quiz PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
)

set_target_properties(Quiz PROPERTIES
  AUTOMOC ON
  AUTORCC ON
  AUTOUIC ON
)

set_property(TARGET Quiz
  APPEND PROPERTY AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/forms
)

target_compile_definitions(Quiz
  PRIVATE QT_NO_CAST_FROM_ASCII
  PRIVATE QT_NO_CAST_TO_ASCII
)

target_include_directories(Quiz
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Quiz
  PRIVATE Qt5::Widgets
  PRIVATE Qt5::Xml
)

target_sources(Quiz
  PRIVATE ${Quiz_FORMS}
  PRIVATE ${Quiz_HEADERS}
  PRIVATE ${Quiz_SOURCES}
)
