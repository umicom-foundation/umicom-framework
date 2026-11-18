#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/image_tensor/CMakeLists.txt
#
# PURPOSE:
#   Compose focused checks of explicit RGB tensor packing without decoding or model execution.
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
    project(UmicomImageTensorChecks LANGUAGES C)
    include(CTest)
    get_filename_component(_image_root "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
    add_library(umicom_image_tensor_subject STATIC "${_image_root}/src/ai/training/image_tensor.c")
    target_include_directories(umicom_image_tensor_subject PUBLIC "${_image_root}/include")
    target_compile_features(umicom_image_tensor_subject PUBLIC c_std_23)
    if(NOT MSVC)
        target_compile_options(umicom_image_tensor_subject PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
    set(_image_library umicom_image_tensor_subject)
elseif(TARGET Umicom::ai)
    set(_image_library Umicom::ai)
else()
    set(_image_library Umicom::AiTraining)
endif()
add_executable(umicom-image-tensor-test test_image_tensor.c)
 target_link_libraries(umicom-image-tensor-test PRIVATE ${_image_library})
 target_compile_features(umicom-image-tensor-test PRIVATE c_std_23)
if(NOT MSVC)
    target_compile_options(umicom-image-tensor-test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
if(NOT WIN32)
    target_link_libraries(umicom-image-tensor-test PRIVATE m)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-image-tensor-test)
endif()
foreach(_case IN ITEMS hwc chw signed padding single-row reference maximum shape-refusal span-refusal arguments)
    add_test(NAME framework.ai.training.image_tensor.${_case} COMMAND umicom-image-tensor-test ${_case})
    set_tests_properties(framework.ai.training.image_tensor.${_case} PROPERTIES TIMEOUT 30 LABELS "framework;training;image;tensor;ownership")
endforeach()

# Standardization composes the existing unit packer; keep its regression target.
add_executable(umicom-image-standardization-test test_image_standardization.c)
target_link_libraries(umicom-image-standardization-test PRIVATE ${_image_library})
target_compile_features(umicom-image-standardization-test PRIVATE c_std_23)
if(NOT MSVC)
    target_compile_options(umicom-image-standardization-test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
if(NOT WIN32)
    target_link_libraries(umicom-image-standardization-test PRIVATE m)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-image-standardization-test)
endif()
foreach(_case golden chw identity all-values-reference maximum invalid-scale invalid-mean
    endpoint-overflow extreme-finite shape-refusal span-refusal arguments)
    add_test(NAME framework.ai.training.image_standardization.${_case} COMMAND umicom-image-standardization-test ${_case})
    set_tests_properties(framework.ai.training.image_standardization.${_case} PROPERTIES TIMEOUT 30 LABELS "framework;training;image;preprocessing;ownership")
endforeach()

# Output conversion shares geometry but owns explicit finite-input and clipping checks.
add_executable(umicom-image-quantization-test test_image_quantization.c)
target_link_libraries(umicom-image-quantization-test PRIVATE ${_image_library})
target_compile_features(umicom-image-quantization-test PRIVATE c_std_23)
if(NOT MSVC)
    target_compile_options(umicom-image-quantization-test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
if(NOT WIN32)
    target_link_libraries(umicom-image-quantization-test PRIVATE m)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-image-quantization-test)
endif()
foreach(_case unit signed chw halfway clipping outside nonfinite padding single-row
    input-tail all-bytes maximum shape-refusal span-refusal arguments)
    add_test(NAME framework.ai.training.image_quantization.${_case} COMMAND umicom-image-quantization-test ${_case})
    set_tests_properties(framework.ai.training.image_quantization.${_case} PROPERTIES TIMEOUT 30 LABELS "framework;training;image;quantization;ownership")
endforeach()
