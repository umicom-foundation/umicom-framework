# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
if(BUILD_TESTING)
    add_executable(umicom-document-source-request-test "${CMAKE_CURRENT_LIST_DIR}/../tests/document/test_source_request.c")
    target_link_libraries(umicom-document-source-request-test PRIVATE Umicom::document)
    umicom_apply_warnings(umicom-document-source-request-test)
    umicom_apply_sanitizers(umicom-document-source-request-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-document-source-request-test)
    endif()
    foreach(case capture empty owned apply delete unchanged undo redo pending-typing stale round-trip caret selection read-only closed other-owner other-tab approval revision restage unicode invalid-utf8 nul cursor-utf8 cursor-crlf limits after-apply arguments)
        add_test(NAME framework.document.source_request.${case} COMMAND umicom-document-source-request-test ${case})
        set_tests_properties(framework.document.source_request.${case} PROPERTIES TIMEOUT 30 LABELS "framework;document;source;review;regression")
    endforeach()
endif()

include("${CMAKE_CURRENT_LIST_DIR}/UmicomDocumentSourceInspection.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomDocumentSourceNavigation.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomDocumentNavigationLines.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomDocumentStoreTextChanges.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomDocumentViewTextBatch.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomDocumentSourceBatch.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomDocumentReplacementSet.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomDocumentSourceWorkspace.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomDocumentSnippets.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomDelimiterNavigation.cmake")
