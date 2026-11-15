#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomCodeGuardJobs.cmake
#
# PURPOSE:
# Register background CodeGuard ownership and real-filesystem qualification.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
if(BUILD_TESTING)
    foreach(_kind IN ITEMS lifecycle native)
        add_executable(umicom-codeguard-job-${_kind}-tests
            "${CMAKE_CURRENT_LIST_DIR}/../tests/codeguard/test_job_${_kind}.c")
        target_link_libraries(umicom-codeguard-job-${_kind}-tests PRIVATE Umicom::codeguard)
        target_compile_features(umicom-codeguard-job-${_kind}-tests PRIVATE c_std_23)
        umicom_apply_warnings(umicom-codeguard-job-${_kind}-tests)
        umicom_apply_sanitizers(umicom-codeguard-job-${_kind}-tests)
    endforeach()
    set(_lifecycle_cases arguments allocation mutex-allocation task-allocation candidate-allocation created copy defaults success failure cancel-before cancel-success cancel-error publication-cancel late-cancel double-take wrong-owner queue-full queue-stopped queued-cancel running running-cancel)
    foreach(_case IN LISTS _lifecycle_cases)
        add_test(NAME "framework.codeguard_job_lifecycle.${_case}"
            COMMAND umicom-codeguard-job-lifecycle-tests "${_case}")
        set_tests_properties("framework.codeguard_job_lifecycle.${_case}" PROPERTIES
            TIMEOUT 25 LABELS "codeguard;jobs;ownership")
    endforeach()
    set(_root "${CMAKE_CURRENT_BINARY_DIR}/tests/codeguard-jobs")
    file(MAKE_DIRECTORY "${_root}/workspace" "${_root}/empty")
    file(CONFIGURE OUTPUT "${_root}/workspace/sample.c"
        CONTENT "void sample(void) {\n    malloc(4);\n}\n" NEWLINE_STYLE LF)
    set(_native_cases ordinary empty budget missing cancel-before copy-profile relative-root retained-result)
    foreach(_case IN LISTS _native_cases)
        add_test(NAME "framework.codeguard_job_native.${_case}"
            COMMAND umicom-codeguard-job-native-tests "${_case}" "${_root}")
        set_tests_properties("framework.codeguard_job_native.${_case}" PROPERTIES
            WORKING_DIRECTORY "${_root}"
            TIMEOUT 25 LABELS "codeguard;jobs;filesystem")
    endforeach()
endif()
