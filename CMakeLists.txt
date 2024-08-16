# SUPPORT OFFICIALLY WINDOWS 11 X64
cmake_minimum_required(VERSION 3.5.0)
project(parallel-launcher VERSION 0.1.0 LANGUAGES C CXX)

# Rechercher Qt 5
find_package(Qt5 COMPONENTS Widgets Network Core REQUIRED)

# Ajouter SQLite3
set(SQLITE3_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/!third_party_libs/SQLite")
set(SQLITE3_SOURCE "${CMAKE_SOURCE_DIR}/!third_party_libs/SQLite/sqlite3.c")

file(GLOB_RECURSE SOURCES
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    ${PROJECT_SOURCE_DIR}/src/*.hpp
    ${PROJECT_SOURCE_DIR}/src/*.c
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
file(GLOB_RECURSE MOC_HEADERS
    ${PROJECT_SOURCE_DIR}/src/**/*.hpp
)

# Générer les fichiers MOC
qt5_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
add_executable(parallel-launcher ${SOURCES} ${MOC_SOURCES})

# Inclure les répertoires d'en-têtes Qt et SQLite3
target_include_directories(parallel-launcher PUBLIC ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Core_INCLUDE_DIRS} ${Qt5Network_INCLUDE_DIRS} ${SQLITE3_INCLUDE_DIR} ${CMAKE_AUTOUIC_OUTPUT_DIRECTORY})

# Lier les bibliothèques Qt
target_link_libraries(parallel-launcher Qt5::Widgets Qt5::Network Qt5::Core bcrypt)

# Ajouter l'option /FS pour éviter les conflits de fichiers .PDB
if (MSVC)
    target_compile_options(parallel-launcher PRIVATE /FS)
endif()

# Additional include directories
set(ALL_INCLUDE_DIR
    "${CMAKE_SOURCE_DIR}/"
    "${CMAKE_SOURCE_DIR}/build/"
    "${CMAKE_SOURCE_DIR}/!third_party_libs/discord-rpc/win64-static/include"
)
include_directories("${CMAKE_SOURCE_DIR}/!third_party_libs/SDL2-2.30.6/include")
target_include_directories(parallel-launcher PUBLIC ${ALL_INCLUDE_DIR})

# Include .lib files from a directory
file(GLOB LIBS "${CMAKE_SOURCE_DIR}/!dlls_and_libs/windows_x64/libs/*.lib")
target_link_libraries(parallel-launcher ${LIBS} "${CMAKE_SOURCE_DIR}/!third_party_libs/SDL2-2.30.6/build/Release/SDL2.lib" "${CMAKE_SOURCE_DIR}/!third_party_libs/discord-rpc/win64-static/lib/*.lib")

# Custom command to copy Qt DLLs
add_custom_command(TARGET parallel-launcher POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${Qt5Widgets_DIR}/../../../bin/Qt5Widgets.dll"
        "${Qt5Core_DIR}/../../../bin/Qt5Core.dll"
        "${Qt5Network_DIR}/../../../bin/Qt5Network.dll"
        "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/"
)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)