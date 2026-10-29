# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
if(BUILD_TESTING)
    add_executable(umicom-language-lifecycle-test "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_server_lifecycle.c")
    target_link_libraries(umicom-language-lifecycle-test PRIVATE Umicom::developer Umicom::platform)
    umicom_apply_warnings(umicom-language-lifecycle-test)
    umicom_apply_sanitizers(umicom-language-lifecycle-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-language-lifecycle-test)
    endif()
    foreach(case valid zero fragmented notification wrong-id request-id cancel-before cancel-during timeout write-error read-error server-error invalid-result receive-atomic receive-oversize receive-deadline receive-buffered-error receive-buffered-pair attach-starting attach-ready attach-duplicate attach-root attach-failed configured-disabled configured-invalid configured-cancel shutdown-ok shutdown-timeout shutdown-write shutdown-error create-root-long create-id-long create-invalid-profile create-ownership configured-preserved
        contract-init-version-missing
        contract-init-version-wrong
        contract-init-version-duplicate
        contract-init-id-duplicate
        contract-init-result-duplicate
        contract-init-result-error
        contract-init-result-missing
        contract-init-error-shape
        contract-init-capabilities-null
        contract-init-capabilities-array
        contract-init-capabilities-duplicate
        contract-init-capabilities-missing
        contract-init-extension
        contract-shutdown-object
        contract-shutdown-false
        contract-shutdown-string
        contract-shutdown-result-missing
        contract-shutdown-result-duplicate
        contract-shutdown-error-shape
        contract-shutdown-result-error
        contract-shutdown-extension)
        add_test(NAME framework.language_runtime.lifecycle.${case} COMMAND umicom-language-lifecycle-test ${case})
        set_tests_properties(framework.language_runtime.lifecycle.${case} PROPERTIES TIMEOUT 30
            LABELS "framework;language;lifecycle;ownership;regression")
    endforeach()
endif()

# Keep lifecycle qualification available to independent installed-SDK clients.
# These are console replay examples; they neither launch nor trust an external tool.
foreach(_language_example IN ITEMS language_lifecycle protocol_frames)
    install(FILES "${CMAKE_CURRENT_LIST_DIR}/../examples/${_language_example}/main.c"
        "${CMAKE_CURRENT_LIST_DIR}/../examples/${_language_example}/CMakeLists.txt"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/examples/${_language_example}"
        COMPONENT FrameworkSDK)
endforeach()
