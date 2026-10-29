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
        contract-init-provider-array
        contract-init-provider-number
        contract-init-provider-duplicate
        contract-init-sync-unknown
        contract-init-late-provider
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

# Private includes participate in change-impact qualification as source inputs.
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umicom_developer FILES
        "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/reply_deadline_private.h"
        "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/reply_wait.inc")
endif()

if(BUILD_TESTING)
    add_executable(umicom-language-reply-deadline-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_reply_deadline.c")
    target_link_libraries(umicom-language-reply-deadline-test PRIVATE Umicom::developer Umicom::platform)
    umicom_apply_warnings(umicom-language-reply-deadline-test)
    umicom_apply_sanitizers(umicom-language-reply-deadline-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-language-reply-deadline-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-language-reply-deadline-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/server_lifecycle.c"
            "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/reply_deadline_private.h"
            "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/reply_wait.inc")
    endif()
    foreach(_boundary IN ITEMS init shutdown query)
        foreach(_case IN ITEMS valid read-expired boundary near zero zero-empty zero-notification fragments)
            add_test(NAME framework.language_runtime.reply_deadline.${_boundary}.${_case}
                COMMAND umicom-language-reply-deadline-test "${_boundary}-${_case}")
            set_tests_properties(framework.language_runtime.reply_deadline.${_boundary}.${_case}
                PROPERTIES TIMEOUT 10 LABELS "framework;language;deadline;regression")
        endforeach()
    endforeach()
    foreach(_case IN ITEMS init-send-expired shutdown-send-expired init-cancel query-cancel query-before)
        add_test(NAME framework.language_runtime.reply_deadline.${_case}
            COMMAND umicom-language-reply-deadline-test "${_case}")
        set_tests_properties(framework.language_runtime.reply_deadline.${_case}
            PROPERTIES TIMEOUT 10 LABELS "framework;language;deadline;regression")
    endforeach()
endif()
