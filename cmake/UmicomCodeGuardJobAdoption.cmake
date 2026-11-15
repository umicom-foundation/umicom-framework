#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomCodeGuardJobAdoption.cmake
#
# PURPOSE:
# Register workspace identity and retained-service adoption regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

if(BUILD_TESTING)
    set(_identity_cases arguments copy small-buffer exact-buffer same-root normalized-root different-root invalid-root relative-drift)
    set(_adoption_cases arguments success empty busy cancelled budget wrong-workspace equivalent-root repeat already-taken service-destroyed relative-drift wrong-thread default-profile)
    set(_root "${CMAKE_CURRENT_BINARY_DIR}/tests/codeguard-job-adoption")
    file(MAKE_DIRECTORY "${_root}/workspace" "${_root}/other")
    file(CONFIGURE OUTPUT "${_root}/workspace/sample.c"
        CONTENT "void sample(void) {\n    malloc(4);\n}\n" NEWLINE_STYLE LF)
    foreach(_kind IN ITEMS identity adoption)
        if(_kind STREQUAL "identity")
            set(_source test_job_identity.c)
        else()
            set(_source test_service_job_adoption.c)
        endif()
        add_executable(umicom-codeguard-job-${_kind}-tests
            "${CMAKE_CURRENT_LIST_DIR}/../tests/codeguard/${_source}")
        target_link_libraries(umicom-codeguard-job-${_kind}-tests PRIVATE Umicom::codeguard)
        target_compile_features(umicom-codeguard-job-${_kind}-tests PRIVATE c_std_23)
        umicom_apply_warnings(umicom-codeguard-job-${_kind}-tests)
        umicom_apply_sanitizers(umicom-codeguard-job-${_kind}-tests)
        foreach(_case IN LISTS _${_kind}_cases)
            add_test(NAME "framework.codeguard_job_${_kind}.${_case}"
                COMMAND umicom-codeguard-job-${_kind}-tests "${_case}" "${_root}")
            set_tests_properties("framework.codeguard_job_${_kind}.${_case}" PROPERTIES
                WORKING_DIRECTORY "${_root}" TIMEOUT 25 LABELS "codeguard;jobs;ownership")
        endforeach()
    endforeach()
endif()
