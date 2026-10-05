# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
function(umicom_attach_source_diagnostics_checks)
    if(NOT TARGET umicom_ui_gtk4 OR NOT BUILD_TESTING OR TARGET umicom-source-diagnostics-gtk4-test)
        return()
    endif()
    add_executable(umicom-source-diagnostics-gtk4-test "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/document/test_source_diagnostics_gtk4.c")
    target_link_libraries(umicom-source-diagnostics-gtk4-test PRIVATE Umicom::ui_gtk4)
    umicom_apply_warnings(umicom-source-diagnostics-gtk4-test)
    umicom_apply_sanitizers(umicom-source-diagnostics-gtk4-test)
    if(WIN32 AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_link_options(umicom-source-diagnostics-gtk4-test PRIVATE -municode)
    endif()
    add_dependencies(umicom-source-diagnostics-gtk4-test umicom-language-process-fixture)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-source-diagnostics-gtk4-test)
    endif()
    foreach(case open read private empty selection multiline cancel retained unbind parent-close creation-unbind worker-unbind input-change changed-back busy invalid-path stale invalidate-output apply read-only open-result choices literal related selection-stale open-unbind invalid-selection unversioned wrong-uri publication-invalidate choice-invalidate)
        add_test(NAME framework.document.source_diagnostics.gtk4.${case}
            COMMAND umicom-source-diagnostics-gtk4-test ${case} "$<TARGET_FILE:umicom-language-process-fixture>")
        set_tests_properties(framework.document.source_diagnostics.gtk4.${case} PROPERTIES TIMEOUT 45 SKIP_RETURN_CODE 77
            LABELS "framework;document;language;source-diagnostics;gtk4;ownership;regression")
    endforeach()
endfunction()
umicom_attach_source_diagnostics_checks()
if(NOT TARGET umicom_ui_gtk4)
    cmake_language(DEFER CALL umicom_attach_source_diagnostics_checks)
endif()

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/source-diagnostics.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/doc/umicom/learning" COMPONENT Development)

include("${CMAKE_CURRENT_LIST_DIR}/UmicomDiagnosticLocationsPanel.cmake")
