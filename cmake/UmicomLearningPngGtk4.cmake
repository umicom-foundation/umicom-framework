#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomLearningPngGtk4.cmake
#
# PURPOSE:
#   Attach the existing optional PNG service after both native owners exist.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
# Both creation orders are supported. A missing optional owner must not consume
# this guard; the other composition point retries after creating its target.
if(NOT TARGET umicom_learning_catalogue_gtk4 OR NOT TARGET Umicom::png_image)
    return()
endif()
include_guard(GLOBAL)
target_compile_definitions(umicom_learning_catalogue_gtk4 PRIVATE UMICOM_LEARNING_PNG=1)
target_link_libraries(umicom_learning_catalogue_gtk4 PRIVATE Umicom::png_image)

# Test adapter allocation and reader rollback without relying on host libpng.
# The fixture supplies a deterministic decoded surface; the other image cases
# retain coverage of the real optional provider and its malformed input paths.
if(BUILD_TESTING)
    add_executable(umicom-learning-catalogue-image-failure-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/learning_pack/test_catalogue_image_failure_gtk4.c")
    set_target_properties(umicom-learning-catalogue-image-failure-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_link_libraries(umicom-learning-catalogue-image-failure-test
        PRIVATE Umicom::learning_catalogue_gtk4 Umicom::media_engine)
    umicom_apply_warnings(umicom-learning-catalogue-image-failure-test)
    umicom_apply_sanitizers(umicom-learning-catalogue-image-failure-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-learning-catalogue-image-failure-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-learning-catalogue-image-failure-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/learning_catalogue_images.c"
            "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/learning_catalogue_private.h"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/media/png_fixture.h")
    endif()
    foreach(_case pixels row decode initial reload back)
        set(_directory "${CMAKE_CURRENT_BINARY_DIR}/learning-image-failure/${_case}")
        file(MAKE_DIRECTORY "${_directory}")
        add_test(NAME "framework.learning_pack.image_failure.${_case}"
            COMMAND umicom-learning-catalogue-image-failure-test "${_case}" "${_directory}")
        set_tests_properties("framework.learning_pack.image_failure.${_case}" PROPERTIES
            TIMEOUT 60 SKIP_RETURN_CODE 77 WORKING_DIRECTORY "${_directory}"
            LABELS "framework;learning;gtk4;ownership;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.learning_pack.image_failure.${_case}"
                TARGETS umicom-learning-catalogue-image-failure-test)
        endif()
    endforeach()
endif()
