#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/library_link/CMakeLists.txt
# PURPOSE: Test real editor consumers and the reusable archive verifier.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
add_executable(umicom-editor-link-check ../../examples/editor_workflow/link_components.c)
target_link_libraries(umicom-editor-link-check PRIVATE Umicom::document)
umicom_apply_warnings(umicom-editor-link-check)
umicom_apply_sanitizers(umicom-editor-link-check)
add_executable(umicom-close-progress-contract-test test_close_progress.c)
target_link_libraries(umicom-close-progress-contract-test PRIVATE Umicom::document)
umicom_apply_warnings(umicom-close-progress-contract-test)
umicom_apply_sanitizers(umicom-close-progress-contract-test)
foreach(target umicom-editor-link-check umicom-close-progress-contract-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(${target})
    endif()
endforeach()
add_test(NAME framework.library_link.editor COMMAND umicom-editor-link-check)
set_tests_properties(framework.library_link.editor PROPERTIES TIMEOUT 30 LABELS "framework;editor;link;example")
foreach(case null dirty path readonly counts name phase status empty-ready incomplete cancelled failed host-failure complete capacity)
    add_test(NAME framework.library_link.progress.${case} COMMAND umicom-close-progress-contract-test ${case})
    set_tests_properties(framework.library_link.progress.${case} PROPERTIES TIMEOUT 30 LABELS "framework;editor;link;regression")
endforeach()
install(TARGETS umicom-editor-link-check RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ../../examples/editor_workflow/link_components.c DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/examples/editor_workflow")

if(CMAKE_C_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$" AND NOT MSVC)
    set(underscore OFF)
    if(APPLE OR (WIN32 AND CMAKE_SIZEOF_VOID_P EQUAL 4))
        set(underscore ON)
    endif()
    foreach(case registration-complete registration-missing-file registration-missing-entry
            complete path-spaces missing undefined duplicate data-symbol empty-manifest
            duplicate-manifest invalid-manifest missing-archive missing-tool empty-tool tool-error report-alias all-missing stale-report)
        add_test(NAME framework.library_link.checker.${case}
            COMMAND "${CMAKE_COMMAND}" "-DCASE=${case}" "-DROOT=${CMAKE_CURRENT_BINARY_DIR}/fixtures"
                "-DCHECKER=${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/VerifyLibrarySymbols.cmake"
                "-DCC=${CMAKE_C_COMPILER}" "-DNM=${CMAKE_NM}" "-DGENERATOR=Ninja"
                "-DALLOW_LEADING_UNDERSCORE=${underscore}"
                -P "${CMAKE_CURRENT_SOURCE_DIR}/check_symbol_verifier.cmake")
        set_tests_properties(framework.library_link.checker.${case} PROPERTIES
            TIMEOUT 150 LABELS "framework;build;link;regression")
    endforeach()
    # Test the production archives as well as deliberately broken fixtures.
    foreach(item document workbench)
        if(item STREQUAL "document")
            set(target umicom_document)
        else()
            set(target umicom_ui)
        endif()
        add_test(NAME framework.library_link.archive.${item}
            COMMAND "${CMAKE_COMMAND}" "-DARCHIVE=$<TARGET_FILE:${target}>"
                "-DSYMBOL_FILE=${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/link_contracts/${item}.symbols"
                "-DNM_TOOL=${CMAKE_NM}" "-DREPORT=${CMAKE_CURRENT_BINARY_DIR}/${item}-test-report.json"
                "-DALLOW_LEADING_UNDERSCORE=${underscore}"
                -P "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/VerifyLibrarySymbols.cmake")
        set_tests_properties(framework.library_link.archive.${item} PROPERTIES
            TIMEOUT 90 LABELS "framework;build;link;production-archive")
    endforeach()
endif()

if(TARGET umicom_ui_gtk4)
    add_executable(umicom-editor-native-link-consumer ../../examples/editor_workflow/link_components.c)
    target_compile_definitions(umicom-editor-native-link-consumer PRIVATE UMICOM_CHECK_NATIVE_EDITOR=1)
    target_link_libraries(umicom-editor-native-link-consumer PRIVATE Umicom::ui_gtk4 Umicom::document)
    umicom_apply_warnings(umicom-editor-native-link-consumer)
    umicom_apply_sanitizers(umicom-editor-native-link-consumer)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-editor-native-link-consumer)
    endif()
    # No GTK initialisation, display or skip return code: link and API checks only.
    add_test(NAME framework.library_link.editor_gtk4 COMMAND umicom-editor-native-link-consumer)
    set_tests_properties(framework.library_link.editor_gtk4 PROPERTIES TIMEOUT 30 LABELS "framework;editor;link;gtk4-library")
endif()
