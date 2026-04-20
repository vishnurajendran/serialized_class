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

# ---------------------------------------------------------------------------
# LIBRARY
# ---------------------------------------------------------------------------
add_library(serialized_class
        src/data/serialized_class_base.cpp
)

# Headers (for IDE visibility, optional but nice)
target_sources(serialized_class PUBLIC
        FILE_SET HEADERS
        BASE_DIRS src
        FILES
        src/data/field_base.h
        src/data/field.h
        src/data/serialized_class_base.h
)

# Include dirs
target_include_directories(serialized_class PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link deps
target_link_libraries(serialized_class PUBLIC
        pugixml
)