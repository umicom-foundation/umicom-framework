#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/media/image_brand/CMakeLists.txt
#
# PURPOSE:
#   Qualify the real SVG watermark and PNG owners without the complete UI application graph.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.25)
project(UmicomImageBrandChecks LANGUAGES C)
include(CTest)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)
find_package(PNG REQUIRED)
get_filename_component(_root "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)
# These are the production owners, compiled afresh. No installed Framework
# library or stale archive supplies the subject under test.
add_library(umicom_image_brand_subject STATIC
    "${_root}/adapters/gtk4/image_brand_gtk4.c"
    "${_root}/src/media/image_surface.c"
    "${_root}/src/media/png_image.c"
    "${_root}/src/media/png_encode.c"
    "${_root}/src/media/png_memory.c"
    "${_root}/src/media/png_file.c"
    "${_root}/src/platform/cancellation.c"
    "${_root}/src/platform/input_file.c"
    "${_root}/src/platform/output_file.c")
target_include_directories(umicom_image_brand_subject PUBLIC "${_root}/include")
target_compile_definitions(umicom_image_brand_subject PRIVATE UMICOM_MEDIA_PNG_ENABLED=1)
target_link_libraries(umicom_image_brand_subject PUBLIC PkgConfig::GTK4 PNG::PNG)
include("${_root}/cmake/UmicomGtkBrandResources.cmake")
umicom_embed_gtk_brand(umicom_image_brand_subject)
include("${_root}/cmake/UmicomImageBrandCases.cmake")
foreach(_kind IN ITEMS brand brand-failure)
    if(_kind STREQUAL "brand")
        set(_file test_image_brand.c)
    else()
        set(_file test_image_brand_failure.c)
    endif()
    add_executable(umicom-image-${_kind}-test "${_root}/tests/media/${_file}")
    target_link_libraries(umicom-image-${_kind}-test PRIVATE umicom_image_brand_subject)
    target_include_directories(umicom-image-${_kind}-test PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/generated/gtk-brand")
endforeach()
foreach(_target IN ITEMS umicom_image_brand_subject umicom-image-brand-test umicom-image-brand-failure-test)
    set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(NOT MSVC)
        target_compile_options(${_target} PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
endforeach()
umicom_register_image_brand_cases(umicom-image-brand-test umicom-image-brand-failure-test)
