#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomCodeGuardPathPolicy.cmake
#
# PURPOSE:
# Register shared relative-path and recursive CodeGuard workspace selection checks.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
if(BUILD_TESTING)
    add_executable(umicom-path-relative-current-tests
        "${CMAKE_CURRENT_LIST_DIR}/../tests/test_path_relative_current.c")
    target_link_libraries(umicom-path-relative-current-tests PRIVATE Umicom::platform)
    add_executable(umicom-codeguard-path-policy-tests
        "${CMAKE_CURRENT_LIST_DIR}/../tests/codeguard/test_path_policy.c")
    target_link_libraries(umicom-codeguard-path-policy-tests PRIVATE Umicom::codeguard)
    add_executable(umicom-codeguard-workspace-exclusions-tests
        "${CMAKE_CURRENT_LIST_DIR}/../tests/codeguard/test_workspace_exclusions.c")
    target_link_libraries(umicom-codeguard-workspace-exclusions-tests PRIVATE Umicom::codeguard)
    foreach(_target IN ITEMS umicom-path-relative-current-tests
        umicom-codeguard-path-policy-tests umicom-codeguard-workspace-exclusions-tests)
        target_compile_features(${_target} PRIVATE c_std_23)
        umicom_apply_warnings(${_target})
        umicom_apply_sanitizers(${_target})
    endforeach()
    foreach(_case IN ITEMS dot-child dot-prefixed empty-base collapsed-base dot-dot empty-empty child-dot child-empty nested-dot child-collapsed ordinary-child ordinary-parent parent-base nested-parent-base sibling-prefix mixed-separators unicode root-child arguments capacity)
        add_test(NAME "framework.path_relative_current.${_case}"
            COMMAND umicom-path-relative-current-tests "${_case}")
        set_tests_properties("framework.path_relative_current.${_case}" PROPERTIES
            TIMEOUT 15 LABELS "platform;path;codeguard")
    endforeach()
    foreach(_case IN ITEMS build-ancestor vendor-ancestor rules-ancestor template-ancestor dot parent-root collapsed-root top-build nested-build git cpack node vendor rules nonadjacent-rules build-prefix build-file rules-prefix template nested-template template-prefix template-nonadjacent mixed unicode outside-prefix outside-parent same-root platform-case arguments)
        add_test(NAME "framework.codeguard_path_policy.${_case}"
            COMMAND umicom-codeguard-path-policy-tests "${_case}")
        set_tests_properties("framework.codeguard_path_policy.${_case}" PROPERTIES
            TIMEOUT 15 LABELS "codeguard;path")
    endforeach()
    # An intentionally excluded ancestor outside the chosen workspace must not
    # hide source inside it. Fixtures are build-local and contain no user files.
    set(_work "${CMAKE_CURRENT_BINARY_DIR}/tests/codeguard-path-policy")
    set(_root "${_work}/build/projects/app")
    set(_eligible main.c src/feature.c build-tools/feature.c
        codeguard/rules-extra/feature.c codeguard/other/rules/feature.c
        templates/workspace/template.c templates/workspace-extra/feature.c
        templates/other/workspace/feature.c)
    set(_excluded build/generated.c sub/build/generated.c third_party/vendor.c
        node_modules/package.c _CPack_Packages/package.c codeguard/rules/rule.c .git/fixture.c)
    foreach(_path IN LISTS _eligible _excluded)
        get_filename_component(_parent "${_root}/${_path}" DIRECTORY)
        file(MAKE_DIRECTORY "${_parent}")
        file(WRITE "${_root}/${_path}" "int example(void) { return 1; }\n")
    endforeach()
    foreach(_case IN ITEMS absolute relative relative-trailing dot explicit-build-root direct-file)
        add_test(NAME "framework.codeguard_workspace_exclusions.${_case}"
            COMMAND umicom-codeguard-workspace-exclusions-tests "${_case}" "${_root}")
        if(_case STREQUAL "dot")
            set(_cwd "${_root}")
        else()
            set(_cwd "${_work}")
        endif()
        set_tests_properties("framework.codeguard_workspace_exclusions.${_case}" PROPERTIES
            WORKING_DIRECTORY "${_cwd}" TIMEOUT 30 LABELS "codeguard;path;filesystem")
    endforeach()
endif()
