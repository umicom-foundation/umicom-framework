#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomBuildPreflightChecks.cmake
#
# PURPOSE:
#   Register complete-workflow metadata admission before native side effects.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

include_guard(GLOBAL)
target_sources(umicom_toolchain PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/build_preflight.c")
if(BUILD_TESTING)
    add_executable(umicom-build-preflight-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_lifecycle/test_build_preflight.c")
    target_link_libraries(umicom-build-preflight-test PRIVATE Umicom::toolchain)
    set_target_properties(umicom-build-preflight-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-build-preflight-test)
    umicom_apply_sanitizers(umicom-build-preflight-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-build-preflight-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-build-preflight-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/build.c"
            "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/build_logging.c"
            "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/build_preflight.c")
    endif()
    foreach(_case valid preview preview-missing preview-undeclared missing-cmake missing-ctest
            cmake-found ctest-failed empty-cmake empty-ctest kind-cmake kind-ctest
            state-cmake state-ctest unterminated-cmake unterminated-ctest utf8-cmake utf8-ctest unicode-cmake unicode-ctest
            profile-size profile-version profile-count null-profile window-mode environment-count
            environment-name environment-value environment-duplicate late-malformed clean-bad-environment
            run-no-tools command-no-tools test-only configure-only compile-only install-only package-only
            make build-test clean-only invalid-options invalid-request unrelated-slot null-environment)
        add_test(NAME "framework.toolchain.build.preflight.${_case}"
            COMMAND umicom-build-preflight-test "${_case}")
        set_tests_properties("framework.toolchain.build.preflight.${_case}" PROPERTIES
            TIMEOUT 30 LABELS "framework;toolchain;workflow;regression;no-process;no-filesystem")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.toolchain.build.preflight.${_case}"
                TARGETS umicom-build-preflight-test)
        endif()
    endforeach()
endif()
