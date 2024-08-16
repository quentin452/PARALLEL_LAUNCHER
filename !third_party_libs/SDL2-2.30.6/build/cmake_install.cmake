# Install script for directory: D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files/SDL2")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/Debug/SDL2d.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/Release/SDL2.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/MinSizeRel/SDL2.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/RelWithDebInfo/SDL2.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/Debug/SDL2d.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/Release/SDL2.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/MinSizeRel/SDL2.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/RelWithDebInfo/SDL2.dll")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE OPTIONAL FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/Debug/SDL2d.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE OPTIONAL FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/Release/SDL2.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE OPTIONAL FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/MinSizeRel/SDL2.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE OPTIONAL FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/RelWithDebInfo/SDL2.pdb")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/Debug/SDL2maind.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/Release/SDL2main.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/MinSizeRel/SDL2main.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/RelWithDebInfo/SDL2main.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE OPTIONAL FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/${CMAKE_INSTALL_CONFIG_NAME}/SDL2main.pdb")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/Debug/SDL2-staticd.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/Release/SDL2-static.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/MinSizeRel/SDL2-static.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/RelWithDebInfo/SDL2-static.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE OPTIONAL FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/${CMAKE_INSTALL_CONFIG_NAME}/SDL2-static.pdb")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/Debug/SDL2_test.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/Release/SDL2_test.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/MinSizeRel/SDL2_test.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/RelWithDebInfo/SDL2_test.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE OPTIONAL FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/${CMAKE_INSTALL_CONFIG_NAME}/SDL2_test.pdb")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/SDL2Targets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/SDL2Targets.cmake"
         "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/CMakeFiles/Export/272ceadb8458515b2ae4b5630a6029cc/SDL2Targets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/SDL2Targets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/SDL2Targets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/CMakeFiles/Export/272ceadb8458515b2ae4b5630a6029cc/SDL2Targets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/CMakeFiles/Export/272ceadb8458515b2ae4b5630a6029cc/SDL2Targets-debug.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/CMakeFiles/Export/272ceadb8458515b2ae4b5630a6029cc/SDL2Targets-minsizerel.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/CMakeFiles/Export/272ceadb8458515b2ae4b5630a6029cc/SDL2Targets-relwithdebinfo.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/CMakeFiles/Export/272ceadb8458515b2ae4b5630a6029cc/SDL2Targets-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/SDL2mainTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/SDL2mainTargets.cmake"
         "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/CMakeFiles/Export/272ceadb8458515b2ae4b5630a6029cc/SDL2mainTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/SDL2mainTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/SDL2mainTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/CMakeFiles/Export/272ceadb8458515b2ae4b5630a6029cc/SDL2mainTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/CMakeFiles/Export/272ceadb8458515b2ae4b5630a6029cc/SDL2mainTargets-debug.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/CMakeFiles/Export/272ceadb8458515b2ae4b5630a6029cc/SDL2mainTargets-minsizerel.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/CMakeFiles/Export/272ceadb8458515b2ae4b5630a6029cc/SDL2mainTargets-relwithdebinfo.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/CMakeFiles/Export/272ceadb8458515b2ae4b5630a6029cc/SDL2mainTargets-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/SDL2staticTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/SDL2staticTargets.cmake"
         "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/CMakeFiles/Export/272ceadb8458515b2ae4b5630a6029cc/SDL2staticTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/SDL2staticTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/SDL2staticTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/CMakeFiles/Export/272ceadb8458515b2ae4b5630a6029cc/SDL2staticTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/CMakeFiles/Export/272ceadb8458515b2ae4b5630a6029cc/SDL2staticTargets-debug.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/CMakeFiles/Export/272ceadb8458515b2ae4b5630a6029cc/SDL2staticTargets-minsizerel.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/CMakeFiles/Export/272ceadb8458515b2ae4b5630a6029cc/SDL2staticTargets-relwithdebinfo.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/CMakeFiles/Export/272ceadb8458515b2ae4b5630a6029cc/SDL2staticTargets-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/SDL2testTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/SDL2testTargets.cmake"
         "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/CMakeFiles/Export/272ceadb8458515b2ae4b5630a6029cc/SDL2testTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/SDL2testTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/SDL2testTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/CMakeFiles/Export/272ceadb8458515b2ae4b5630a6029cc/SDL2testTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/CMakeFiles/Export/272ceadb8458515b2ae4b5630a6029cc/SDL2testTargets-debug.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/CMakeFiles/Export/272ceadb8458515b2ae4b5630a6029cc/SDL2testTargets-minsizerel.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/CMakeFiles/Export/272ceadb8458515b2ae4b5630a6029cc/SDL2testTargets-relwithdebinfo.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/CMakeFiles/Export/272ceadb8458515b2ae4b5630a6029cc/SDL2testTargets-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Devel" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES
    "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/SDL2Config.cmake"
    "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/SDL2ConfigVersion.cmake"
    "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/cmake/sdlfind.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SDL2" TYPE FILE FILES
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_assert.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_atomic.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_audio.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_bits.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_blendmode.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_clipboard.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_copying.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_cpuinfo.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_egl.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_endian.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_error.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_events.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_filesystem.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_gamecontroller.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_gesture.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_guid.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_haptic.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_hidapi.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_hints.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_joystick.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_keyboard.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_keycode.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_loadso.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_locale.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_log.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_main.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_messagebox.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_metal.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_misc.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_mouse.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_mutex.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_name.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_opengl.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_opengl_glext.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_opengles.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_opengles2.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_opengles2_gl2.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_opengles2_gl2ext.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_opengles2_gl2platform.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_opengles2_khrplatform.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_pixels.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_platform.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_power.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_quit.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_rect.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_render.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_rwops.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_scancode.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_sensor.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_shape.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_stdinc.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_surface.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_system.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_syswm.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_assert.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_common.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_compare.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_crc32.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_font.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_fuzzer.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_harness.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_images.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_log.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_md5.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_memory.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_random.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_thread.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_timer.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_touch.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_types.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_version.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_video.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_vulkan.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/begin_code.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/close_code.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/include/SDL2/SDL_revision.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/include-config-debug/SDL2/SDL_config.h"
      )
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SDL2" TYPE FILE FILES
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_assert.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_atomic.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_audio.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_bits.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_blendmode.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_clipboard.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_copying.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_cpuinfo.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_egl.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_endian.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_error.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_events.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_filesystem.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_gamecontroller.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_gesture.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_guid.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_haptic.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_hidapi.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_hints.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_joystick.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_keyboard.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_keycode.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_loadso.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_locale.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_log.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_main.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_messagebox.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_metal.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_misc.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_mouse.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_mutex.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_name.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_opengl.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_opengl_glext.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_opengles.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_opengles2.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_opengles2_gl2.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_opengles2_gl2ext.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_opengles2_gl2platform.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_opengles2_khrplatform.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_pixels.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_platform.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_power.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_quit.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_rect.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_render.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_rwops.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_scancode.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_sensor.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_shape.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_stdinc.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_surface.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_system.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_syswm.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_assert.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_common.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_compare.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_crc32.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_font.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_fuzzer.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_harness.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_images.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_log.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_md5.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_memory.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_random.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_thread.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_timer.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_touch.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_types.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_version.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_video.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_vulkan.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/begin_code.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/close_code.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/include/SDL2/SDL_revision.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/include-config-release/SDL2/SDL_config.h"
      )
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SDL2" TYPE FILE FILES
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_assert.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_atomic.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_audio.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_bits.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_blendmode.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_clipboard.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_copying.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_cpuinfo.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_egl.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_endian.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_error.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_events.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_filesystem.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_gamecontroller.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_gesture.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_guid.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_haptic.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_hidapi.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_hints.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_joystick.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_keyboard.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_keycode.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_loadso.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_locale.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_log.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_main.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_messagebox.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_metal.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_misc.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_mouse.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_mutex.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_name.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_opengl.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_opengl_glext.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_opengles.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_opengles2.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_opengles2_gl2.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_opengles2_gl2ext.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_opengles2_gl2platform.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_opengles2_khrplatform.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_pixels.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_platform.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_power.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_quit.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_rect.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_render.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_rwops.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_scancode.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_sensor.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_shape.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_stdinc.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_surface.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_system.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_syswm.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_assert.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_common.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_compare.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_crc32.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_font.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_fuzzer.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_harness.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_images.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_log.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_md5.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_memory.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_random.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_thread.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_timer.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_touch.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_types.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_version.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_video.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_vulkan.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/begin_code.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/close_code.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/include/SDL2/SDL_revision.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/include-config-minsizerel/SDL2/SDL_config.h"
      )
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SDL2" TYPE FILE FILES
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_assert.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_atomic.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_audio.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_bits.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_blendmode.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_clipboard.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_copying.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_cpuinfo.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_egl.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_endian.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_error.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_events.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_filesystem.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_gamecontroller.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_gesture.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_guid.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_haptic.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_hidapi.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_hints.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_joystick.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_keyboard.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_keycode.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_loadso.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_locale.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_log.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_main.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_messagebox.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_metal.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_misc.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_mouse.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_mutex.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_name.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_opengl.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_opengl_glext.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_opengles.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_opengles2.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_opengles2_gl2.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_opengles2_gl2ext.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_opengles2_gl2platform.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_opengles2_khrplatform.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_pixels.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_platform.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_power.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_quit.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_rect.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_render.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_rwops.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_scancode.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_sensor.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_shape.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_stdinc.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_surface.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_system.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_syswm.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_assert.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_common.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_compare.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_crc32.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_font.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_fuzzer.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_harness.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_images.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_log.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_md5.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_memory.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_test_random.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_thread.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_timer.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_touch.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_types.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_version.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_video.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/SDL_vulkan.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/begin_code.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/include/close_code.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/include/SDL2/SDL_revision.h"
      "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/include-config-relwithdebinfo/SDL2/SDL_config.h"
      )
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/licenses/SDL2" TYPE FILE FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/LICENSE.txt")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/sdl2.pc")
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "D:/github-intellidea/PARALLEL_LAUNCHER/!third_party_libs/SDL2-2.30.6/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
