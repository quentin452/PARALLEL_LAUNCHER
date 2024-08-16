# SUPPORT OFFICIALLY WINDOWS 11 X64
cmake_minimum_required(VERSION 3.5.0)
project(parallel-launcher VERSION 0.1.0 LANGUAGES C CXX)

# Rechercher Qt 5
find_package(Qt5 COMPONENTS Widgets Network REQUIRED)

# Ajouter SQLite3
set(SQLITE3_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/!third_party_libs/SQLite")
set(SQLITE3_SOURCE "${CMAKE_SOURCE_DIR}/!third_party_libs/SQLite/sqlite3.c")

file(GLOB_RECURSE SOURCES
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    ${PROJECT_SOURCE_DIR}/src/*.h
    ${PROJECT_SOURCE_DIR}/src/*.c
    ${PROJECT_SOURCE_DIR}/src/*.hpp
    ${SQLITE3_SOURCE}  # Add the SQLite source file here
)

# Find all .ui files
file(GLOB UI_FILES 
    ${PROJECT_SOURCE_DIR}/src/ui/designer/*.ui 
    ${PROJECT_SOURCE_DIR}/src/rhdc/ui/designer/*.ui
    ${PROJECT_SOURCE_DIR}/src/updaters/*.ui
)

# Set the output directory for the generated UI headers
set(CMAKE_AUTOUIC_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/ui)

# Generate headers from .ui files
qt5_wrap_ui(UI_HEADERS ${UI_FILES})

# Add the generated headers to the list of sources
set(SOURCES ${SOURCES} ${UI_HEADERS})

# Define the executable target
add_executable(parallel-launcher lsjs.c sdl-proxy.c ${SOURCES})

# Inclure les répertoires d'en-têtes Qt et SQLite3
target_include_directories(parallel-launcher PUBLIC ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Network_INCLUDE_DIRS} ${SQLITE3_INCLUDE_DIR} ${CMAKE_AUTOUIC_OUTPUT_DIRECTORY})

# Lier les bibliothèques Qt
target_link_libraries(parallel-launcher Qt5::Widgets Qt5::Network)

# Ajouter l'option /FS pour éviter les conflits de fichiers .PDB
if (MSVC)
    target_compile_options(parallel-launcher PRIVATE /FS)
endif()

# Additional include directories
set(ALL_INCLUDE_DIR
    "${CMAKE_SOURCE_DIR}/"
    "${CMAKE_SOURCE_DIR}/build/"
)
include_directories("${CMAKE_SOURCE_DIR}/!third_party_libs/SDL2-2.30.6/include")
target_include_directories(${PROJECT_NAME} PUBLIC ${ALL_INCLUDE_DIR})

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
