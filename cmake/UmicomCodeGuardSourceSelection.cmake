#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomCodeGuardSourceSelection.cmake
#
# PURPOSE:
# Register real scan-profile selection and precise pattern failure checks.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
if(BUILD_TESTING)
    add_executable(umicom-codeguard-profile-selection-tests
        "${CMAKE_CURRENT_LIST_DIR}/../tests/codeguard/test_profile_selection.c")
    add_executable(umicom-codeguard-pattern-failure-tests
        "${CMAKE_CURRENT_LIST_DIR}/../tests/codeguard/test_pattern_failures.c")
    foreach(_target IN ITEMS umicom-codeguard-profile-selection-tests
        umicom-codeguard-pattern-failure-tests)
        target_link_libraries(${_target} PRIVATE Umicom::codeguard)
        target_compile_features(${_target} PRIVATE c_std_23)
        umicom_apply_warnings(${_target})
        umicom_apply_sanitizers(${_target})
    endforeach()
    # Isolated fixtures make selected languages and generated directories
    # observable without reading or altering a developer's workspace files.
    set(_root "${CMAKE_CURRENT_BINARY_DIR}/tests/codeguard-profile-selection")
    foreach(_path IN ITEMS main.c lib.cpp api.h note.txt build/generated.c
        _CPack_Packages/pkg.c third_party/vendor.c node_modules/pkg.c
        codeguard/rules/rule.c .git/ignored.c third_party/build/vendor.c
        build/third_party/vendor.c build/codeguard/rules/rule.c)
        get_filename_component(_parent "${_root}/${_path}" DIRECTORY)
        file(MAKE_DIRECTORY "${_parent}")
        file(WRITE "${_root}/${_path}" "int example(void) { return 1; }\n")
    endforeach()
    foreach(_case IN ITEMS default generated c-only cpp-only headers-only none
        generated-none nonzero direct-c direct-disabled-c direct-disabled-cpp
        direct-disabled-header direct-generated duplicates-default
        duplicates-without-c duplicates-none duplicates-generated)
        add_test(NAME "framework.codeguard_profile_selection.${_case}"
            COMMAND umicom-codeguard-profile-selection-tests "${_case}" "${_root}")
        set_tests_properties("framework.codeguard_profile_selection.${_case}" PROPERTIES
            TIMEOUT 30 LABELS "codeguard;profile;filesystem")
    endforeach()
    foreach(_case IN ITEMS arguments ordinary raw-only code-only include no-match
        allocation capacity io-status second-failure)
        add_test(NAME "framework.codeguard_pattern_failure.${_case}"
            COMMAND umicom-codeguard-pattern-failure-tests "${_case}")
        set_tests_properties("framework.codeguard_pattern_failure.${_case}" PROPERTIES
            TIMEOUT 20 LABELS "codeguard;fault-injection")
    endforeach()
endif()
