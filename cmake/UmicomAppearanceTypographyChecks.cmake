#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomAppearanceTypographyChecks.cmake
#
# PURPOSE:
#   Register bounded profile admission and native typography regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

include_guard(GLOBAL)
if(BUILD_TESTING AND TARGET umicom_ui)
    add_executable(umicom-appearance-admission-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ui_appearance/test_profile_admission.c")
    target_link_libraries(umicom-appearance-admission-test PRIVATE Umicom::ui)
    umicom_apply_warnings(umicom-appearance-admission-test)
    umicom_apply_sanitizers(umicom-appearance-admission-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-appearance-admission-test)
    endif()
    foreach(case IN ITEMS text numbers families boundaries model)
        add_test(NAME framework.ui.appearance.admission.${case}
            COMMAND umicom-appearance-admission-test "${case}")
        set_tests_properties(framework.ui.appearance.admission.${case} PROPERTIES
            TIMEOUT 20 LABELS "framework;ui;appearance;admission;regression")
    endforeach()
endif()

if(BUILD_TESTING AND TARGET umicom_ui_gtk4)
    add_executable(umicom-appearance-typography-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ui_workstation/test_typography_gtk4.c"
        "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/appearance_typography_private.h")
    target_link_libraries(umicom-appearance-typography-test PRIVATE Umicom::ui_gtk4)
    umicom_apply_warnings(umicom-appearance-typography-test)
    umicom_apply_sanitizers(umicom-appearance-typography-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-appearance-typography-test)
    endif()
    foreach(case IN ITEMS format locale invalid workbench workstation)
        add_test(NAME framework.ui.appearance.typography.${case}
            COMMAND umicom-appearance-typography-test "${case}")
        set_tests_properties(framework.ui.appearance.typography.${case} PROPERTIES
            TIMEOUT 20 SKIP_RETURN_CODE 77 LABELS "framework;ui;gtk4;typography;regression")
    endforeach()
endif()
