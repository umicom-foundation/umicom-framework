#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomRootedFileCreation.cmake
#
# PURPOSE:
#   Qualify exclusive rooted creation separately from document replacement.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(_umicom_create_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umicom_platform FILES
        "${_umicom_create_root}/src/platform/rooted_create_win32.inc"
        "${_umicom_create_root}/src/platform/rooted_create_posix.inc")
endif()
if(BUILD_TESTING)
    add_executable(umicom-rooted-create-test
        "${_umicom_create_root}/tests/platform/test_rooted_create.c")
    target_link_libraries(umicom-rooted-create-test PRIVATE Umicom::platform)
    add_executable(umicom-rooted-create-failures-test
        "${_umicom_create_root}/tests/platform/test_rooted_create_failures.c")
    # The failure harness includes the real native writer and substitutes only
    # its operating-system calls. No product implementation is linked twice.
    target_include_directories(umicom-rooted-create-failures-test PRIVATE "${_umicom_create_root}/include")
    foreach(_target umicom-rooted-create-test umicom-rooted-create-failures-test)
        set_target_properties("${_target}" PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings("${_target}")
        umicom_apply_sanitizers("${_target}")
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target("${_target}")
        endif()
    endforeach()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-rooted-create-failures-test FILES
            "${_umicom_create_root}/src/platform/rooted_create_win32.inc"
            "${_umicom_create_root}/src/platform/rooted_create_posix.inc")
    endif()
    foreach(_case roundtrip empty unicode large existing missing-parent directory arguments
            traversal absolute git device leaf-link parent-link root-link)
        set(_directory "${CMAKE_CURRENT_BINARY_DIR}/qualification/rooted-create/${_case}")
        file(MAKE_DIRECTORY "${_directory}")
        add_test(NAME "framework.filesystem.create.${_case}" COMMAND umicom-rooted-create-test "${_case}")
        set_tests_properties("framework.filesystem.create.${_case}" PROPERTIES TIMEOUT 60
            SKIP_RETURN_CODE 77 WORKING_DIRECTORY "${_directory}"
            LABELS "framework;platform;filesystem;learning;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.filesystem.create.${_case}" TARGETS umicom-rooted-create-test)
        endif()
    endforeach()
    set(_failure_cases open inspect write partial zero flush close short success empty)
    if(NOT WIN32)
        list(APPEND _failure_cases interrupt-write interrupt-flush)
    endif()
    foreach(_case IN LISTS _failure_cases)
        add_test(NAME "framework.filesystem.create.failure.${_case}"
            COMMAND umicom-rooted-create-failures-test "${_case}")
        set_tests_properties("framework.filesystem.create.failure.${_case}" PROPERTIES TIMEOUT 30
            LABELS "framework;platform;filesystem;regression;fault-injection")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.filesystem.create.failure.${_case}"
                TARGETS umicom-rooted-create-failures-test)
        endif()
    endforeach()
endif()

if(BUILD_TESTING)
    add_executable(umicom-rooted-create-race-test
        "${_umicom_create_root}/tests/platform/test_rooted_create_race.c")
    target_link_libraries(umicom-rooted-create-race-test PRIVATE Umicom::platform)
    set_target_properties(umicom-rooted-create-race-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-rooted-create-race-test)
    umicom_apply_sanitizers(umicom-rooted-create-race-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-rooted-create-race-test)
    endif()
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/rooted-create/race")
    add_test(NAME framework.filesystem.create.concurrent COMMAND umicom-rooted-create-race-test)
    set_tests_properties(framework.filesystem.create.concurrent PROPERTIES TIMEOUT 60
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/rooted-create/race"
        LABELS "framework;platform;filesystem;concurrency;regression")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(framework.filesystem.create.concurrent TARGETS umicom-rooted-create-race-test)
    endif()
endif()
