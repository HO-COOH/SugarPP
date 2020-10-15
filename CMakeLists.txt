cmake_minimum_required(VERSION 3.12)

project(SugarPP
        VERSION 0.0.1
        HOMEPAGE_URL "https://github.com/HO-COOH/SugarPP"
        LANGUAGES CXX)

# ---- Warning guard ----

# Protect dependents from this project's warnings if the guard isn't disabled
set(sugarpp_warning_guard "SYSTEM")
if(SugarPP_INCLUDE_WITHOUT_SYSTEM)
  set(sugarpp_warning_guard "")
endif()

# ---- Declare library ----
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(SugarPP INTERFACE)
add_library(SugarPP::SugarPP ALIAS SugarPP)

target_include_directories(SugarPP
        ${sugarpp_warning_guard}
        INTERFACE
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>")

# target_compile_features(SugarPP INTERFACE cxx_std_17)  # makes it hard to develop because the IDE does not know to use C++17 standard

option(SugarPPUseNamespace "Make SugarPP inside namespace SugarPP" ON)
if(SugarPPUseNamespace)
    target_compile_definitions(SugarPP INTERFACE SugarPPNamespace)
endif()


# ---- Install ----

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

set(sugarpp_directory "SugarPP-${PROJECT_VERSION}")
set(sugarpp_include_directory "${CMAKE_INSTALL_INCLUDEDIR}/${sugarpp_directory}")

install(TARGETS SugarPP
        EXPORT SugarPPTargets
        INCLUDES DESTINATION "${sugarpp_include_directory}")

set(sugarpp_install_cmakedir
        "${CMAKE_INSTALL_LIBDIR}/cmake/${sugarpp_directory}")

write_basic_package_version_file(
        SugarPPConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
        ARCH_INDEPENDENT)

install(EXPORT SugarPPTargets
        FILE SugarPPConfig.cmake
        NAMESPACE SugarPP::
        DESTINATION "${sugarpp_install_cmakedir}")

install(FILES
        "${PROJECT_BINARY_DIR}/SugarPPConfigVersion.cmake"
        DESTINATION "${sugarpp_install_cmakedir}")

install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/"
        DESTINATION "${sugarpp_include_directory}")
