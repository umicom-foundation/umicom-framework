#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomLearningProgressGtk4.cmake
#
# PURPOSE:
#   Bind optional native checkpoint controls after both service targets exist.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
# Both owners call this module after defining themselves. Do not consume the
# guard on an early optional include: composition order must not remove a feature.
if(NOT TARGET umicom_learning_catalogue_gtk4 OR NOT TARGET Umicom::learning_progress)
    return()
endif()
include_guard(GLOBAL)
target_link_libraries(umicom_learning_catalogue_gtk4 PRIVATE Umicom::learning_progress)
target_compile_definitions(umicom_learning_catalogue_gtk4 PRIVATE UMICOM_LEARNING_PROGRESS_AVAILABLE=1)
get_filename_component(_learning_progress_gtk_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

if(BUILD_TESTING)
    add_executable(umicom-learning-catalogue-progress-test
        "${_learning_progress_gtk_root}/tests/learning_pack/test_catalogue_progress_gtk4.c")
    set_target_properties(umicom-learning-catalogue-progress-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_link_libraries(umicom-learning-catalogue-progress-test PRIVATE
        Umicom::learning_catalogue_gtk4 Umicom::learning_progress Umicom::browser)
    umicom_apply_warnings(umicom-learning-catalogue-progress-test)
    umicom_apply_sanitizers(umicom-learning-catalogue-progress-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-learning-catalogue-progress-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-learning-catalogue-progress-test FILES
            "${_learning_progress_gtk_root}/adapters/gtk4/learning_catalogue_jobs.c"
            "${_learning_progress_gtk_root}/adapters/gtk4/learning_catalogue_private.h"
            "${_learning_progress_gtk_root}/tests/support/gtk4_expanders.h")
    endif()
    foreach(_case arguments wrong-kind retired dispatch-retirement no-automatic-write
            inspect-missing record-inspect duplicate native-controls busy captured-request
            destroy-pending retire-pending uncertain-recovery other-learner-recovery
            other-database-recovery reentrant-result revision-binding receipt-retained)
        set(_directory "${CMAKE_CURRENT_BINARY_DIR}/learning-native-progress/${_case}")
        file(MAKE_DIRECTORY "${_directory}")
        add_test(NAME "framework.learning_pack.native_progress.${_case}"
            COMMAND umicom-learning-catalogue-progress-test "${_case}" "${_directory}")
        set_tests_properties("framework.learning_pack.native_progress.${_case}" PROPERTIES
            TIMEOUT 60 SKIP_RETURN_CODE 77 WORKING_DIRECTORY "${_directory}"
            LABELS "framework;learning;gtk4;storage;ownership;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.learning_pack.native_progress.${_case}"
                TARGETS umicom-learning-catalogue-progress-test)
        endif()
    endforeach()
endif()
