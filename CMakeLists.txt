# Currently officially support Windows 11 X64
cmake_minimum_required(VERSION 3.5.0)
project(parallel-launcher VERSION 0.1.0 LANGUAGES C CXX)

set(RESOURCE_FILE "${CMAKE_SOURCE_DIR}/data/resources.qrc")

# Set the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Rechercher Qt 5
find_package(Qt5 COMPONENTS Widgets Network Core LinguistTools REQUIRED)

# Ajouter SQLite3
set(SQLITE3_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/!third_party_libs/SQLite")
set(SQLITE3_SOURCE "${CMAKE_SOURCE_DIR}/!third_party_libs/SQLite/sqlite3.c")

file(GLOB_RECURSE SOURCES
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    ${PROJECT_SOURCE_DIR}/src/*.hpp
    ${PROJECT_SOURCE_DIR}/src/*.c
    ${SQLITE3_SOURCE}
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

# Définir les fichiers de traduction
set(TRANSLATIONS
    ${PROJECT_SOURCE_DIR}/lang/parallel-launcher_en_us.ts
    ${PROJECT_SOURCE_DIR}/lang/parallel-launcher_en_gb.ts
    ${PROJECT_SOURCE_DIR}/lang/parallel-launcher_it.ts
    ${PROJECT_SOURCE_DIR}/lang/parallel-launcher_es.ts
    ${PROJECT_SOURCE_DIR}/lang/parallel-launcher_fr.ts
    ${PROJECT_SOURCE_DIR}/lang/parallel-launcher_fr_ca.ts
    ${PROJECT_SOURCE_DIR}/lang/parallel-launcher_de.ts
    ${PROJECT_SOURCE_DIR}/lang/parallel-launcher_ar.ts
    ${PROJECT_SOURCE_DIR}/lang/parallel-launcher_pt.ts
)

# Définir le répertoire où les fichiers .qm seront placés
set(QM_OUTPUT_DIR "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/translations")
file(MAKE_DIRECTORY ${QM_OUTPUT_DIR})

# Commande pour générer les fichiers .qm
foreach(TS_FILE ${TRANSLATIONS})
    get_filename_component(TS_FILENAME ${TS_FILE} NAME_WE)
    set(QM_FILE "${QM_OUTPUT_DIR}/${TS_FILENAME}.qm")
    add_custom_command(
        OUTPUT ${QM_FILE}
        COMMAND ${Qt5_LRELEASE_EXECUTABLE} ${TS_FILE} -qm ${QM_FILE}
        DEPENDS ${TS_FILE}
        COMMENT "Generating ${QM_FILE} from ${TS_FILE}"
    )
    list(APPEND QM_FILES ${QM_FILE})
endforeach()

# Ajouter les fichiers .qm générés à la liste des sources
set(SOURCES ${SOURCES} ${UI_HEADERS} ${QM_FILES})

# Define the executable target
file(GLOB_RECURSE MOC_HEADERS
    ${PROJECT_SOURCE_DIR}/src/**/*.hpp
)

# Générer les fichiers MOC
qt5_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
add_executable(parallel-launcher WIN32 ${SOURCES} ${MOC_SOURCES} ${RESOURCE_FILE})

# Créer le répertoire de sortie s'il n'existe pas
add_custom_command(
    TARGET parallel-launcher POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${QM_OUTPUT_DIR}
    COMMENT "Creating output directory if it doesn't exist"
)

# Copier les fichiers .qm
foreach(QM_FILE ${QM_FILES})
    add_custom_command(
        TARGET parallel-launcher POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QM_FILE} ${QM_OUTPUT_DIR}
        COMMENT "Copying ${QM_FILE} to ${QM_OUTPUT_DIR}"
    )
endforeach()

# Assurez-vous que le projet utilise Qt pour les traductions
set_target_properties(parallel-launcher PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    # AUTOUIC ON
)

if (MSVC)
    set_target_properties(parallel-launcher PROPERTIES
        WIN32_EXECUTABLE ON
    )
endif()

# Inclure les répertoires d'en-têtes Qt et SQLite3
target_include_directories(parallel-launcher PUBLIC ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Core_INCLUDE_DIRS} ${Qt5Network_INCLUDE_DIRS} ${SQLITE3_INCLUDE_DIR} ${CMAKE_AUTOUIC_OUTPUT_DIRECTORY})

# Lier les bibliothèques Qt
target_link_libraries(parallel-launcher Qt5::Widgets Qt5::Network Qt5::Core bcrypt winmm)

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
include_directories("${CMAKE_SOURCE_DIR}/!third_party_libs/SDL2-2.30.6/include" "${CMAKE_SOURCE_DIR}/!third_party_libs/OpenSSL-Win64-1.1.1a/x64/include")
target_include_directories(parallel-launcher PUBLIC ${ALL_INCLUDE_DIR})

# Include .lib files from a directory
file(GLOB LIBS "${CMAKE_SOURCE_DIR}/!dlls_and_libs/windows_x64/libs/*.lib")
target_link_libraries(parallel-launcher ${LIBS} "${CMAKE_SOURCE_DIR}/!third_party_libs/OpenSSL-Win64-1.1.1a/x64/lib/*.lib" "${CMAKE_SOURCE_DIR}/!third_party_libs/SDL2-2.30.6/build/Release/SDL2.lib" "${CMAKE_SOURCE_DIR}/!third_party_libs/discord-rpc/win64-static/lib/*.lib")
# Custom command to copy Qt DLLs
add_custom_command(TARGET parallel-launcher POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Copying Qt DLLs..."
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${Qt5Widgets_DIR}/../../../bin/Qt5Widgets.dll"
        "${Qt5Core_DIR}/../../../bin/Qt5Core.dll"
        "${Qt5Network_DIR}/../../../bin/Qt5Network.dll"
        "${Qt5Network_DIR}/../../../bin/Qt5Gui.dll"
        "$<TARGET_FILE_DIR:parallel-launcher>"
)

file(GLOB DLL_FILES "${CMAKE_SOURCE_DIR}/!third_party_libs/OpenSSL-Win64-1.1.1a/x64/bin/*.dll" "${CMAKE_SOURCE_DIR}/!third_party_libs/SDL2-2.30.6/build/Release/*.dll")
foreach(DLL ${DLL_FILES})
    add_custom_command(TARGET parallel-launcher POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Copying ${DLL} to $<TARGET_FILE_DIR:parallel-launcher>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${DLL}"
            "$<TARGET_FILE_DIR:parallel-launcher>"
    )
endforeach()

# Find windeployqt
find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${Qt5_DIR}/../../../bin")

# Custom command to run windeployqt
add_custom_command(TARGET parallel-launcher POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Running windeployqt..."
    COMMAND ${WINDEPLOYQT_EXECUTABLE} $<TARGET_FILE:parallel-launcher>
)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)