# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
target_sources(umicom_document PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/document/snippet_review.c")

if(BUILD_TESTING)
    add_executable(umicom-document-snippet-test "${CMAKE_CURRENT_LIST_DIR}/../tests/document/test_snippet_review.c")
    target_link_libraries(umicom-document-snippet-test PRIVATE Umicom::document)
    umicom_apply_warnings(umicom-document-snippet-test)
    umicom_apply_sanitizers(umicom-document-snippet-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-document-snippet-test)
    endif()
    foreach(case capture insert selection empty linked unicode multiline final-stop no-final first-final literal empty-value choices owned template-change value-change prepare-again invalid-value invalid-template stale caret selection-change read-only create-read-only closed other-owner other-tab approval revision no-template unprepared cancel-template cancel-value cancel-prepare after-apply undo redo pending-typing arguments)
        add_test(NAME framework.document.snippet.${case} COMMAND umicom-document-snippet-test ${case})
        set_tests_properties(framework.document.snippet.${case} PROPERTIES TIMEOUT 30 LABELS "framework;document;snippet;review;regression")
    endforeach()
endif()

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/snippet-review.html" DESTINATION "${CMAKE_INSTALL_DATADIR}/doc/umicom/learning" COMPONENT Development)
