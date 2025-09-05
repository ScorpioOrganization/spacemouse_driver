cmake_minimum_required(VERSION 3.15)
project(spacemouse_driver VERSION 1.0.0 LANGUAGES CXX)

# ---- Build options ----
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(CMAKE_BUILD_TYPE STREQUAL "Profile")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg -O2")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
endif()

# ---- Dependencies ----
find_package(PkgConfig REQUIRED)
pkg_check_modules(HIDAPI REQUIRED hidapi-hidraw)

include(FetchContent)
FetchContent_Declare(
    magic_enum
    GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
    GIT_TAG v0.9.5
)
FetchContent_MakeAvailable(magic_enum)

# ---- Sources ----
file(GLOB_RECURSE PUBLIC_HEADERS
    include/spacemouse_driver/*.hpp
)

file(GLOB_RECURSE PRIVATE_SOURCES
    src/*.cpp
    src/*.hpp
)

# ---- Library ----
add_library(spacemouse_driver SHARED ${PRIVATE_SOURCES})

target_include_directories(spacemouse_driver
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${magic_enum_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/magic_enum>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(spacemouse_driver
    PRIVATE
        ${HIDAPI_LIBRARIES}
)

set_target_properties(spacemouse_driver PROPERTIES
    PUBLIC_HEADER "${PUBLIC_HEADERS}"
)

# ---- Installation ----
include(GNUInstallDirs)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/spacemouse_driver
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(DIRECTORY ${magic_enum_SOURCE_DIR}/include/magic_enum
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(TARGETS spacemouse_driver
    EXPORT spacemouse_driverTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(EXPORT spacemouse_driverTargets
    FILE spacemouse_driverTargets.cmake
    NAMESPACE spacemouse_driver::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/spacemouse_driver
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/spacemouse_driverConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/spacemouse_driverConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/spacemouse_driverConfig.cmake
    @ONLY
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/spacemouse_driverConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/spacemouse_driverConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/spacemouse_driver
)