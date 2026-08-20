# SPDX-License-Identifier: GPL-3.0-or-later
cmake_minimum_required(VERSION 3.20)
project(InfiltratrCommonConsumer LANGUAGES C)

find_package(InfiltratrCommon 1.10 CONFIG REQUIRED)

add_executable(infiltratr-common-consumer main.c)
target_compile_features(infiltratr-common-consumer PRIVATE c_std_11)
target_link_libraries(infiltratr-common-consumer
    PRIVATE InfiltratrCommon::Portable)
