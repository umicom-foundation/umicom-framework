#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomUiStatePublicationChecks.cmake
#
# PURPOSE:
#   Qualify compound accessibility and theme publication independently of native GUI rendering.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------


include_guard(GLOBAL)
if(BUILD_TESTING)
    foreach(case IN ITEMS accessibility theme)
        set(target "umicom-ui-${case}-publication-test")
        add_executable("${target}"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ui_components/test_${case}_publication.c")
        target_link_libraries("${target}" PRIVATE Umicom::ui Umicom::ui_components)
        set_target_properties("${target}" PROPERTIES
            C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings("${target}")
        umicom_apply_sanitizers("${target}")
        add_test(NAME "framework.ui.${case}.publication" COMMAND "${target}")
        set_tests_properties("framework.ui.${case}.publication" PROPERTIES
            TIMEOUT 30 LABELS "framework;ui;ownership;memory-only")
    endforeach()
endif()

# Keep admission cases distinct from the existing publication selectors so
# a focused build never silently selects unrelated, unbuilt executables.
if(BUILD_TESTING)
    foreach(validation_case IN ITEMS theme_profile_admission editor_theme_admission editor_theme_resolution)
        set(target "umicom-ui-${validation_case}-test")
        add_executable("${target}"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ui_components/test_${validation_case}.c")
        target_link_libraries("${target}" PRIVATE Umicom::ui)
        set_target_properties("${target}" PROPERTIES
            C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings("${target}")
        umicom_apply_sanitizers("${target}")
        add_test(NAME "framework.ui.validation.${validation_case}" COMMAND "${target}")
        set_tests_properties("framework.ui.validation.${validation_case}" PROPERTIES
            TIMEOUT 30 LABELS "framework;ui;theme;validation;memory-only")
    endforeach()
endif()

# Token storage is toolkit-neutral and has its own focused selection. Keeping
# this distinct preserves the previously documented profile/editor selectors.
if(BUILD_TESTING)
    add_executable(umicom-ui-theme-token-storage-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ui_components/test_theme_token_storage.c")
    target_link_libraries(umicom-ui-theme-token-storage-test PRIVATE Umicom::ui)
    set_target_properties(umicom-ui-theme-token-storage-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ui-theme-token-storage-test)
    umicom_apply_sanitizers(umicom-ui-theme-token-storage-test)
    add_test(NAME framework.ui.tokens.storage COMMAND umicom-ui-theme-token-storage-test)
    set_tests_properties(framework.ui.tokens.storage PROPERTIES
        TIMEOUT 30 LABELS "framework;ui;theme;storage;memory-only")
endif()
