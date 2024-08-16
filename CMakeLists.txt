cmake_minimum_required(VERSION 3.5.0)
project(parallel-launcher VERSION 0.1.0 LANGUAGES C CXX)

# Rechercher Qt 5
find_package(Qt5 COMPONENTS Widgets Network REQUIRED)

# Ajouter SQLite3
set(SQLITE3_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/!third_party_libs/SQLite")
set(SQLITE3_LIBRARY "${CMAKE_SOURCE_DIR}/!third_party_libs/SQLite/sqlite3.c")

file(GLOB_RECURSE SOURCES
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    ${PROJECT_SOURCE_DIR}/src/*.h
    ${PROJECT_SOURCE_DIR}/src/*.c
    ${PROJECT_SOURCE_DIR}/src/*.hpp
)
add_executable(parallel-launcher lsjs.c sdl-proxy.c ${SOURCES})

# Inclure les répertoires d'en-têtes Qt et SQLite3
target_include_directories(parallel-launcher PUBLIC ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Network_INCLUDE_DIRS} ${SQLITE3_INCLUDE_DIR})

# Lier les bibliothèques Qt et SQLite3
target_link_libraries(parallel-launcher Qt5::Widgets Qt5::Network ${SQLITE3_LIBRARY})

# Ajouter l'option /FS pour éviter les conflits de fichiers .PDB
if (MSVC)
    target_compile_options(parallel-launcher PRIVATE /FS)
endif()

set(ALL_INCLUDE_DIR
    "${CMAKE_SOURCE_DIR}/"
)
include_directories("${CMAKE_SOURCE_DIR}/!third_party_libs/SDL2-2.30.6/include")
target_include_directories(${PROJECT_NAME} PUBLIC
${ALL_INCLUDE_DIR}
)
include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
