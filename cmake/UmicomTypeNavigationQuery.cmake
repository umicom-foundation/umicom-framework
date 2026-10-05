# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
if(BUILD_TESTING)
    add_executable(umicom-type-navigation-query-test "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_type_navigation_query.c")
    target_link_libraries(umicom-type-navigation-query-test PRIVATE Umicom::developer Umicom::editor Umicom::platform)
    umicom_apply_warnings(umicom-type-navigation-query-test)
    umicom_apply_sanitizers(umicom-type-navigation-query-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-type-navigation-query-test)
    endif()
    foreach(case type-valid type-object type-link type-origin-outside type-self-outside type-self-surrogate type-external type-missing type-disabled type-wrong-provider type-encoding type-sync type-fragmented type-notification type-error type-invalid-result type-shutdown-error type-close-error type-timeout type-cancel-before type-cancel-during type-invalid-source type-invalid-caret type-invalid-kind type-invalid-declaration type-state type-empty type-unicode type-native-invalid type-read-error type-write-error type-wrong-id implementation-valid implementation-object implementation-link implementation-origin-outside implementation-self-outside implementation-self-surrogate implementation-external implementation-missing implementation-disabled implementation-wrong-provider implementation-encoding implementation-sync implementation-fragmented implementation-notification implementation-error implementation-invalid-result implementation-shutdown-error implementation-close-error implementation-timeout implementation-cancel-before implementation-cancel-during implementation-invalid-source implementation-invalid-caret implementation-invalid-kind implementation-invalid-declaration implementation-state implementation-empty implementation-unicode implementation-native-invalid implementation-read-error implementation-write-error implementation-wrong-id)
        add_test(NAME framework.language_runtime.type_navigation_query.${case} COMMAND umicom-type-navigation-query-test ${case})
        set_tests_properties(framework.language_runtime.type_navigation_query.${case} PROPERTIES TIMEOUT 30
            LABELS "framework;language;navigation;ownership;regression")
    endforeach()
endif()

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/type-navigation.html" DESTINATION "${CMAKE_INSTALL_DATADIR}/doc/umicom/learning" COMPONENT Development)
