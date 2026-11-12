#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomLauncherAdmissionChecks.cmake
#
# PURPOSE:
#   Register real finance launcher admission checks after optional GTK components attach.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

include_guard(GLOBAL)
function(umicom_attach_launcher_admission_checks)
    if(NOT BUILD_TESTING OR NOT TARGET umicom_ui_gtk4 OR
       NOT TARGET Umicom::cash_planning OR NOT TARGET Umicom::bank_feed)
        return()
    endif()
    # Treasury adapters attach through deferred component composition. Qualify
    # this combined surface only when all four implementations are present.
    get_target_property(_forward umicom_ui_gtk4 UMICOM_FORWARD_PRACTICE_ATTACHED)
    get_target_property(_accounting umicom_ui_gtk4 UMICOM_ACCOUNTING_PRACTICE_ATTACHED)
    if(NOT _forward OR NOT _accounting OR TARGET umicom-launcher-admission-gtk4-test)
        return()
    endif()
    add_executable(umicom-launcher-admission-gtk4-test
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/ui/test_launcher_admission_gtk4.c")
    target_link_libraries(umicom-launcher-admission-gtk4-test PRIVATE Umicom::ui_gtk4)
    set_target_properties(umicom-launcher-admission-gtk4-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-launcher-admission-gtk4-test)
    umicom_apply_sanitizers(umicom-launcher-admission-gtk4-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-launcher-admission-gtk4-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-launcher-admission-gtk4-test FILES
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../adapters/gtk4/window_removal_private.h")
    endif()
    foreach(_kind IN ITEMS forward accounting cash bank)
        foreach(_case IN ITEMS disabled-self disabled-ancestor detached retired-parent reparent-disabled reenabled active)
            set(_test "framework.gtk4.launcher_admission.${_kind}.${_case}")
            add_test(NAME "${_test}" COMMAND umicom-launcher-admission-gtk4-test "${_kind}" "${_case}")
            set_tests_properties("${_test}" PROPERTIES TIMEOUT 45 SKIP_RETURN_CODE 77
                ENVIRONMENT "G_DEBUG=fatal-warnings" LABELS "framework;finance;gtk4;lifetime;regression")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners("${_test}" TARGETS umicom-launcher-admission-gtk4-test)
            endif()
        endforeach()
    endforeach()
endfunction()
cmake_language(DEFER CALL umicom_attach_launcher_admission_checks)
