# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
if(BUILD_TESTING)
    add_executable(umicom-document-history-selection-test "${CMAKE_CURRENT_LIST_DIR}/../tests/document/test_history_selection.c")
    target_link_libraries(umicom-document-history-selection-test PRIVATE Umicom::document)
    umicom_apply_warnings(umicom-document-history-selection-test)
    umicom_apply_sanitizers(umicom-document-history-selection-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-document-history-selection-test)
    endif()
    foreach(case insert selection delete unicode crlf first-line end empty duplicate indent pending-typing moved-before-undo moved-before-redo read-only invalid-text no-change targeted-other-tab eviction redo-cleared fallback-unicode fallback-crlf)
        add_test(NAME framework.document.history_selection.${case} COMMAND umicom-document-history-selection-test ${case})
        set_tests_properties(framework.document.history_selection.${case} PROPERTIES TIMEOUT 30 LABELS "framework;document;history;selection;regression")
    endforeach()
endif()

include("${CMAKE_CURRENT_LIST_DIR}/UmicomDocumentTypingHistory.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomNativeTypingHistory.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomSourceSetHistory.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomReloadHistoryPositions.cmake")

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/document-history.html" DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/umicom/learning COMPONENT Development)
