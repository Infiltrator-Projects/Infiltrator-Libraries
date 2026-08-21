# SPDX-License-Identifier: GPL-3.0-or-later
cmake_minimum_required(VERSION 3.20)

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" INFILTRATR_COMMON_VERSION)
string(STRIP "${INFILTRATR_COMMON_VERSION}" INFILTRATR_COMMON_VERSION)
if(NOT INFILTRATR_COMMON_VERSION MATCHES "^[0-9]+\\.[0-9]+\\.[0-9]+$")
    message(FATAL_ERROR
        "VERSION must contain a semantic major.minor.patch version.")
endif()

project(InfiltratrCommon
    VERSION "${INFILTRATR_COMMON_VERSION}"
    LANGUAGES C)

include(CTest)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(INFILTRATR_COMMON_BUILD_TESTS
    "Build Infiltratr Common contract and smoke tests"
    ${PROJECT_IS_TOP_LEVEL})
option(INFILTRATR_COMMON_WARNINGS_AS_ERRORS
    "Treat Infiltratr Common compiler warnings as errors"
    ${PROJECT_IS_TOP_LEVEL})

set(INFILTRATR_COMMON_PORTABLE_SOURCES
    src/core.c
    src/arithmetic.c
    src/config.c
    src/timing.c
    src/format.c)

set(INFILTRATR_COMMON_WARNING_FLAGS
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wformat=2
    -Wstrict-prototypes
    -Wmissing-prototypes)

function(infiltratr_common_configure_target target export_name)
    target_compile_features(${target} PUBLIC c_std_11)
    target_include_directories(${target}
        PUBLIC
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
            "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
    set_target_properties(${target} PROPERTIES
        C_EXTENSIONS OFF
        EXPORT_NAME "${export_name}")

    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
        target_compile_options(${target} PRIVATE
            ${INFILTRATR_COMMON_WARNING_FLAGS})
        if(INFILTRATR_COMMON_WARNINGS_AS_ERRORS)
            target_compile_options(${target} PRIVATE -Werror)
        endif()
    endif()
endfunction()

add_library(infiltratr-common-portable STATIC
    ${INFILTRATR_COMMON_PORTABLE_SOURCES})
add_library(InfiltratrCommon::Portable ALIAS infiltratr-common-portable)
set_target_properties(infiltratr-common-portable PROPERTIES
    OUTPUT_NAME infiltratr-portable)
infiltratr_common_configure_target(infiltratr-common-portable Portable)

set(INFILTRATR_COMMON_FULL_SOURCES
    ${INFILTRATR_COMMON_PORTABLE_SOURCES}
    src/dynlib.c)
if(UNIX)
    list(APPEND INFILTRATR_COMMON_FULL_SOURCES src/posix.c)
endif()

add_library(infiltratr-common STATIC ${INFILTRATR_COMMON_FULL_SOURCES})
add_library(InfiltratrCommon::Common ALIAS infiltratr-common)
set_target_properties(infiltratr-common PROPERTIES
    OUTPUT_NAME infiltratr-common)
infiltratr_common_configure_target(infiltratr-common Common)

if(UNIX)
    target_link_libraries(infiltratr-common-portable PUBLIC m)
    target_link_libraries(infiltratr-common PUBLIC m)
endif()
if(CMAKE_DL_LIBS)
    target_link_libraries(infiltratr-common PUBLIC "${CMAKE_DL_LIBS}")
endif()

function(infiltratr_common_add_test target source library)
    add_executable(${target} ${source})
    target_link_libraries(${target} PRIVATE ${library})
    target_compile_features(${target} PRIVATE c_std_11)
    set_target_properties(${target} PROPERTIES C_EXTENSIONS OFF)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
        target_compile_options(${target} PRIVATE
            ${INFILTRATR_COMMON_WARNING_FLAGS})
        if(INFILTRATR_COMMON_WARNINGS_AS_ERRORS)
            target_compile_options(${target} PRIVATE -Werror)
        endif()
    endif()
    add_test(NAME ${target} COMMAND ${target})
endfunction()

if(INFILTRATR_COMMON_BUILD_TESTS)
    infiltratr_common_add_test(infiltratr-core-smoke
        tests/core_smoke.c InfiltratrCommon::Common)
    infiltratr_common_add_test(infiltratr-arithmetic-smoke
        tests/arithmetic_smoke.c InfiltratrCommon::Portable)
    infiltratr_common_add_test(infiltratr-config-smoke
        tests/config_smoke.c InfiltratrCommon::Portable)
    infiltratr_common_add_test(infiltratr-timing-smoke
        tests/timing_smoke.c InfiltratrCommon::Portable)
    infiltratr_common_add_test(infiltratr-format-smoke
        tests/format_smoke.c InfiltratrCommon::Common)
    infiltratr_common_add_test(infiltratr-dynlib-smoke
        tests/dynlib_smoke.c InfiltratrCommon::Common)
    infiltratr_common_add_test(infiltratr-portable-smoke
        tests/portable_smoke.c InfiltratrCommon::Portable)
    infiltratr_common_add_test(infiltratr-portable-contract
        tests/portable_contract.c InfiltratrCommon::Portable)
    if(UNIX)
        infiltratr_common_add_test(infiltratr-posix-contract
            tests/posix_contract.c InfiltratrCommon::Common)
    endif()
endif()

install(TARGETS infiltratr-common-portable infiltratr-common
    EXPORT InfiltratrCommonTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
install(DIRECTORY include/infiltratr
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
install(EXPORT InfiltratrCommonTargets
    FILE InfiltratrCommonTargets.cmake
    NAMESPACE InfiltratrCommon::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/InfiltratrCommon")

configure_package_config_file(
    cmake/InfiltratrCommonConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/InfiltratrCommonConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/InfiltratrCommon")
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/InfiltratrCommonConfigVersion.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY SameMajorVersion)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/InfiltratrCommonConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/InfiltratrCommonConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/InfiltratrCommon")
