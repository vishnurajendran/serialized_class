cmake_minimum_required(VERSION 3.30)
project(serialized_class)
set(CMAKE_CXX_STANDARD 20)
include(FetchContent)

# ---------------------------------------------------------------------------
# PUGIXML
# ---------------------------------------------------------------------------
FetchContent_Declare(
        pugixml
        GIT_REPOSITORY https://github.com/zeux/pugixml.git
        GIT_TAG v1.15
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(pugixml)

add_executable(serialized_class main.cpp
        src/data/field_base.h
        src/data/serialized_class_base.cpp
        src/data/serialized_class_base.h
        src/Test/test_data.cpp
        src/Test/test_data.h
        src/data/field.h
)
target_link_libraries(serialized_class PRIVATE
        pugixml)