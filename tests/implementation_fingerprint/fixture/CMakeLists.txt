#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/implementation_fingerprint/fixture/CMakeLists.txt
# PURPOSE: Build one real library and consumer for incremental-link regression.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomImplementationConsumer LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
include("${FINGERPRINT_MODULE}")
# The former single-object fixture is kept for review. An object library
# exposes exact object paths, allowing the regression to check that unrelated
# objects stay untouched while the real archive and consumer are rebuilt.
if(FALSE)
add_library(owner STATIC owner.c)
umicom_attach_implementation_fingerprint(owner "${CMAKE_CURRENT_SOURCE_DIR}" private.h)
get_target_property(identity owner UMICOM_IMPLEMENTATION_FINGERPRINT)
endif()
add_library(owner_objects OBJECT owner.c independent.c)
umicom_attach_implementation_fingerprint(owner_objects "${CMAKE_CURRENT_SOURCE_DIR}")
get_target_property(identity owner_objects UMICOM_IMPLEMENTATION_FINGERPRINT)
add_library(owner STATIC $<TARGET_OBJECTS:owner_objects>)
file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/objects-$<CONFIG>.txt"
    CONTENT "$<JOIN:$<TARGET_OBJECTS:owner_objects>,\n>")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/owner-identity.txt" "${identity}")
file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/owner-$<CONFIG>.txt" CONTENT "$<TARGET_FILE:owner>")
get_target_property(exposed owner INTERFACE_COMPILE_DEFINITIONS)
if(exposed MATCHES "UMI_IMPLEMENTATION_FINGERPRINT")
    message(FATAL_ERROR "An implementation identity leaked to the public consumer.")
endif()
add_executable(consumer main.c)
target_link_libraries(consumer PRIVATE owner)
target_compile_definitions(consumer PRIVATE EXPECTED_VALUE=${EXPECTED_VALUE})
file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/consumer-$<CONFIG>.txt" CONTENT "$<TARGET_FILE:consumer>")
