# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Portable desired-state ownership belongs to Debug. JSON and native files
# remain in Developer, which already owns the shared parsing dependencies.
include_guard(GLOBAL)
target_sources(umicom_debug PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/debug/setup.c")
target_link_libraries(umicom_debug PRIVATE Umicom::document)
target_sources(umicom_developer PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug/setup_document.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug/setup_file.c")
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/saved-debug-setups.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/learning" COMPONENT Framework)
if(BUILD_TESTING)
    add_executable(umicom-debug-setup-model-test "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_setup/test_model.c")
    target_link_libraries(umicom-debug-setup-model-test PRIVATE Umicom::developer)
    umicom_apply_warnings(umicom-debug-setup-model-test)
    umicom_apply_sanitizers(umicom-debug-setup-model-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-debug-setup-model-test)
    endif()
    foreach(case copy capture capture-large capacity duplicates bounds invalid unicode)
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/debug-setup/model/${case}")
        add_test(NAME framework.debug_setup.model.${case} COMMAND umicom-debug-setup-model-test ${case})
        set_tests_properties(framework.debug_setup.model.${case} PROPERTIES TIMEOUT 30
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/debug-setup/model/${case}"
            LABELS "framework;debugger;setup;regression")
    endforeach()
    add_executable(umicom-debug-setup-review-test "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_setup/test_review.c")
    target_link_libraries(umicom-debug-setup-review-test PRIVATE Umicom::developer)
    umicom_apply_warnings(umicom-debug-setup-review-test)
    umicom_apply_sanitizers(umicom-debug-setup-review-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-debug-setup-review-test)
    endif()
    foreach(case apply reverse empty lifetime unrelated watch-stale breakpoint-stale owner-stale configuration-stale session-stale controller-stale active)
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/debug-setup/review/${case}")
        add_test(NAME framework.debug_setup.review.${case} COMMAND umicom-debug-setup-review-test ${case})
        set_tests_properties(framework.debug_setup.review.${case} PROPERTIES TIMEOUT 30
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/debug-setup/review/${case}"
            LABELS "framework;debugger;setup;regression")
    endforeach()
    add_executable(umicom-debug-setup-document-test "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_setup/test_document.c")
    target_link_libraries(umicom-debug-setup-document-test PRIVATE Umicom::developer)
    umicom_apply_warnings(umicom-debug-setup-document-test)
    umicom_apply_sanitizers(umicom-debug-setup-document-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-debug-setup-document-test)
    endif()
    foreach(case round-trip maximum escaping unknown duplicate missing format wrong-kind boolean late-invalid embedded-nul trailing truncated line-overflow)
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/debug-setup/document/${case}")
        add_test(NAME framework.debug_setup.document.${case} COMMAND umicom-debug-setup-document-test ${case})
        set_tests_properties(framework.debug_setup.document.${case} PROPERTIES TIMEOUT 30
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/debug-setup/document/${case}"
            LABELS "framework;debugger;setup;regression")
    endforeach()
    add_executable(umicom-debug-setup-file-test "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_setup/test_file.c")
    target_link_libraries(umicom-debug-setup-file-test PRIVATE Umicom::developer)
    umicom_apply_warnings(umicom-debug-setup-file-test)
    umicom_apply_sanitizers(umicom-debug-setup-file-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-debug-setup-file-test)
    endif()
    foreach(case relative missing malformed round-trip no-overwrite)
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/debug-setup/file/${case}")
        add_test(NAME framework.debug_setup.file.${case} COMMAND umicom-debug-setup-file-test ${case})
        set_tests_properties(framework.debug_setup.file.${case} PROPERTIES TIMEOUT 30
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/debug-setup/file/${case}"
            LABELS "framework;debugger;setup;regression")
    endforeach()
endif()
