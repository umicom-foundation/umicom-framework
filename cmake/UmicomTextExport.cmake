# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# File work belongs to the shared platform owner, never individual applications.
if(NOT TARGET umicom_ui_gtk4)
    return()
endif()
include_guard(GLOBAL)
include("${CMAKE_CURRENT_LIST_DIR}/UmicomPathPicker.cmake")
target_sources(umicom_ui_gtk4 PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/text_export_gtk4.c")
target_link_libraries(umicom_ui_gtk4 PRIVATE Umicom::platform)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/ui/gtk4/text_export.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/guides/saving-displayed-reports.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/docs/guides" COMPONENT Framework)
if(BUILD_TESTING)
    foreach(kind IN ITEMS native failures retirement)
        add_executable(umicom-text-export-${kind}-test
            "${CMAKE_CURRENT_LIST_DIR}/../tests/text_export/test_${kind}.c")
        target_link_libraries(umicom-text-export-${kind}-test PRIVATE Umicom::ui_gtk4 Umicom::platform)
        set_target_properties(umicom-text-export-${kind}-test PROPERTIES
            C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(umicom-text-export-${kind}-test)
        umicom_apply_sanitizers(umicom-text-export-${kind}-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-text-export-${kind}-test)
        endif()
        add_test(NAME framework.text_export.${kind} COMMAND umicom-text-export-${kind}-test)
        set_tests_properties(framework.text_export.${kind} PROPERTIES
            TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;gtk4;files;reports;ownership")
    endforeach()
endif()

if(BUILD_TESTING)
    add_executable(umi-text-export-admission-test "${CMAKE_CURRENT_LIST_DIR}/../tests/text_export/test_admission.c")
    target_link_libraries(umi-text-export-admission-test PRIVATE Umicom::ui_gtk4)
    set_target_properties(umi-text-export-admission-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umi-text-export-admission-test)
    umicom_apply_sanitizers(umi-text-export-admission-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umi-text-export-admission-test)
    endif()
    foreach(case normal constructor denied stale callback-reentry callback-retire callback-updates feedback-refusal feedback-completion feedback-retire source-gone invalid-path retired accepted-snapshot)
        set(test "framework.text_export.admission.${case}")
        set(directory "${CMAKE_CURRENT_BINARY_DIR}/qualification/framework.text_export.admission/${case}")
        file(MAKE_DIRECTORY "${directory}")
        add_test(NAME ${test} COMMAND umi-text-export-admission-test ${case} "${directory}")
        set_tests_properties(${test} PROPERTIES TIMEOUT 45 SKIP_RETURN_CODE 77
            WORKING_DIRECTORY "${directory}" ENVIRONMENT "G_DEBUG=fatal-warnings"
            LABELS "framework;gtk4;publication;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS umi-text-export-admission-test)
        endif()
    endforeach()
endif()
