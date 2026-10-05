# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
function(umicom_attach_completion_review_checks)
    if(NOT TARGET umicom_ui_gtk4 OR NOT BUILD_TESTING OR TARGET umicom-completion-review-gtk4-test)
        return()
    endif()
    add_executable(umicom-completion-review-gtk4-test "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/document/test_completion_review_gtk4.c")
    target_link_libraries(umicom-completion-review-gtk4-test PRIVATE Umicom::ui_gtk4)
    umicom_apply_warnings(umicom-completion-review-gtk4-test)
    umicom_apply_sanitizers(umicom-completion-review-gtk4-test)
    if(WIN32 AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_link_options(umicom-completion-review-gtk4-test PRIVATE -municode)
    endif()
    add_dependencies(umicom-completion-review-gtk4-test umicom-language-process-fixture)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-completion-review-gtk4-test)
    endif()
    foreach(case open apply undo no-preview no-approval stale cancel retained unbind parent-close creation-unbind worker-unbind input-change changed-back private busy invalid-path completion-unbind)
        add_test(NAME framework.document.completion_review.gtk4.${case}
            COMMAND umicom-completion-review-gtk4-test ${case} "$<TARGET_FILE:umicom-language-process-fixture>")
        set_tests_properties(framework.document.completion_review.gtk4.${case} PROPERTIES TIMEOUT 45 SKIP_RETURN_CODE 77
            LABELS "framework;document;language;completion;gtk4;ownership;regression")
    endforeach()
endfunction()
umicom_attach_completion_review_checks()
if(NOT TARGET umicom_ui_gtk4)
    cmake_language(DEFER CALL umicom_attach_completion_review_checks)
endif()

install(FILES
    "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/native-completion-review.html"
    "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/reviewing-source-tool-results.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
