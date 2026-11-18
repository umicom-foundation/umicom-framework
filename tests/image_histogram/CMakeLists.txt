#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/image_histogram/CMakeLists.txt
#
# PURPOSE:
#   Compose focused checks of exact RGB frequencies, view composition and unchanged refused output.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.25)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(UmicomImageHistogramChecks LANGUAGES C)
    include(CTest)
    get_filename_component(_histogram_root "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
    add_library(umicom_image_histogram_subject STATIC "${_histogram_root}/src/ai/training/image_histogram.c"
        "${_histogram_root}/src/ai/training/image_flip.c"
        "${_histogram_root}/src/ai/training/image_tensor.c" "${_histogram_root}/src/ai/training/image_view.c")
    target_include_directories(umicom_image_histogram_subject PUBLIC "${_histogram_root}/include")
    target_compile_features(umicom_image_histogram_subject PUBLIC c_std_23)
    if(NOT MSVC)
        target_compile_options(umicom_image_histogram_subject PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
    set(_histogram_library umicom_image_histogram_subject)
elseif(TARGET Umicom::ai)
    set(_histogram_library Umicom::ai)
else()
    set(_histogram_library Umicom::AiTraining)
endif()
add_executable(umicom-image-histogram-test test_image_histogram.c)
 target_link_libraries(umicom-image-histogram-test PRIVATE ${_histogram_library})
 target_compile_features(umicom-image-histogram-test PRIVATE c_std_23)
if(NOT MSVC)
    target_compile_options(umicom-image-histogram-test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
if(NOT WIN32)
    target_link_libraries(umicom-image-histogram-test PRIVATE m)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-image-histogram-test)
endif()
foreach(_case IN ITEMS golden padding crop flip constant refusal metadata limits conservation)
    add_test(NAME framework.ai.training.image_histogram.${_case} COMMAND umicom-image-histogram-test ${_case})
    set_tests_properties(framework.ai.training.image_histogram.${_case} PROPERTIES TIMEOUT 30 LABELS "framework;training;image;view;ownership")
endforeach()


# Derived moments reuse the production histogram owner and its admitted view.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    target_sources(umicom_image_histogram_subject PRIVATE "${_histogram_root}/src/ai/training/image_moments.c")
endif()
add_executable(umicom-image-moments-test test_image_moments.c)
target_link_libraries(umicom-image-moments-test PRIVATE ${_histogram_library})
target_compile_features(umicom-image-moments-test PRIVATE c_std_23)
if(NOT MSVC)
    target_compile_options(umicom-image-moments-test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
if(NOT WIN32)
    target_link_libraries(umicom-image-moments-test PRIVATE m)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-image-moments-test)
endif()
foreach(_case golden constant single padded-reference crop maximum-count zero-count over-count late-invalid-bins view-refusal arguments)
    add_test(NAME framework.ai.training.image_moments.${_case} COMMAND umicom-image-moments-test ${_case})
    set_tests_properties(framework.ai.training.image_moments.${_case} PROPERTIES TIMEOUT 30 LABELS "framework;training;image;statistics;ownership")
endforeach()
