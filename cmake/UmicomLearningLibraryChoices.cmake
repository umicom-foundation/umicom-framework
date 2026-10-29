#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomLearningLibraryChoices.cmake
#
# PURPOSE:
#   Attach remembered-library services and focused qualification cases.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(_choices_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
target_sources(umicom_developer PRIVATE
    "${_choices_root}/src/teacher/learning_library_choices.c"
    "${_choices_root}/src/teacher/learning_library_choices_codec.c"
    "${_choices_root}/src/teacher/learning_library_choices_files.c")
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umicom_developer FILES
        "${_choices_root}/src/teacher/learning_library_choices_internal.h")
endif()
install(FILES "${_choices_root}/include/umicom/teacher/learning_library_choices.h"
    DESTINATION "include/umicom/teacher" COMPONENT FrameworkSDK)
if(BUILD_TESTING)
    foreach(_kind library_choices library_choices_files)
        add_executable("umicom-${_kind}-test" "${_choices_root}/tests/learning_pack/test_${_kind}.c")
        set_target_properties("umicom-${_kind}-test" PROPERTIES
            C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        target_link_libraries("umicom-${_kind}-test" PRIVATE Umicom::developer)
        umicom_apply_warnings("umicom-${_kind}-test")
        umicom_apply_sanitizers("umicom-${_kind}-test")
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target("umicom-${_kind}-test")
        endif()
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs("umicom-${_kind}-test" FILES
                "${_choices_root}/tests/learning_pack/library_choices_fixture.h")
        endif()
    endforeach()
    foreach(_case empty schema version fractional-version duplicate escaped-duplicate
            unknown missing entry-duplicate entry-unknown entry-type revision revision-overflow
            traversal absolute-manifest device control embedded-nul locale identifier array-type
            relative-root trailing utf8 nul bounded outputs arguments clone forget capacity
            duplicate-id roundtrip)
        add_test(NAME "framework.learning_pack.library_choices.${_case}"
            COMMAND umicom-library_choices-test "${_case}" "${_choices_root}")
        set_tests_properties("framework.learning_pack.library_choices.${_case}" PROPERTIES
            TIMEOUT 30 LABELS "framework;learning;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.learning_pack.library_choices.${_case}"
                TARGETS umicom-library_choices-test)
        endif()
    endforeach()
    foreach(_case missing-settings refused-save existing-save missing-manifest
            identity product locale revision locale-case incompatible changed-resource reopen)
        set(_directory "${CMAKE_CURRENT_BINARY_DIR}/learning-library-choices/${_case}")
        file(MAKE_DIRECTORY "${_directory}")
        add_test(NAME "framework.learning_pack.library_choices_files.${_case}"
            COMMAND umicom-library_choices_files-test "${_case}" "${_directory}")
        set_tests_properties("framework.learning_pack.library_choices_files.${_case}" PROPERTIES
            TIMEOUT 30 WORKING_DIRECTORY "${_directory}" LABELS "framework;learning;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.learning_pack.library_choices_files.${_case}"
                TARGETS umicom-library_choices_files-test)
        endif()
    endforeach()
endif()
