#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomWorkspaceIdentity.cmake
#
# PURPOSE:
# Register workspace identity exhaustion and atomic discovery regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

if(BUILD_TESTING)
    add_executable(umicom-workspace-identity-tests
        "${CMAKE_CURRENT_LIST_DIR}/../tests/test_workspace_identity.c")
    target_link_libraries(umicom-workspace-identity-tests PRIVATE Umicom::platform)
    target_compile_features(umicom-workspace-identity-tests PRIVATE c_std_23)
    umicom_apply_warnings(umicom-workspace-identity-tests)
    umicom_apply_sanitizers(umicom-workspace-identity-tests)
    set(_identity_root "${CMAKE_CURRENT_BINARY_DIR}/tests/workspace-identity")
    file(MAKE_DIRECTORY "${_identity_root}/project-a" "${_identity_root}/project-b")
    foreach(_leaf IN ITEMS project-a project-b)
        file(CONFIGURE OUTPUT "${_identity_root}/${_leaf}/CMakeLists.txt"
            CONTENT "# Discovery marker only; never configured.\n" NEWLINE_STYLE LF)
    endforeach()
    foreach(_case IN ITEMS open-overflow close-overflow trust-overflow add-overflow remove-overflow
        open-last close-last trust-last add-last remove-last id-overflow id-zero id-last
        duplicate-at-limit missing-at-limit discover-overflow discover-nochange-limit
        discover-multi-last discover-id-overflow discover-id-partial discover-nochange-id-limit
        discover-stale discover-failure session-ids closed invalid)
        add_test(NAME "framework.workspace_identity.${_case}"
            COMMAND umicom-workspace-identity-tests "${_case}" "${_identity_root}")
        set_tests_properties("framework.workspace_identity.${_case}" PROPERTIES
            TIMEOUT 20 LABELS "workspace;identity;ownership")
    endforeach()
endif()
