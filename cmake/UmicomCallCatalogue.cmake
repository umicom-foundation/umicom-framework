# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
if(BUILD_TESTING)
    add_executable(umicom-call-catalogue-test "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_call_catalogue.c")
    target_link_libraries(umicom-call-catalogue-test PRIVATE Umicom::developer)
    umicom_apply_warnings(umicom-call-catalogue-test)
    umicom_apply_sanitizers(umicom-call-catalogue-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-call-catalogue-test)
    endif()
    foreach(case item-owned item-null item-empty item-multiple item-unicode item-detail item-deprecated item-unknown-kind item-data-null item-name-empty item-name-whitespace item-kind-zero item-kind-negative item-kind-fraction item-kind-large item-name-number item-detail-null item-uri-relative item-uri-escape item-tags-number item-tag-zero item-tag-string item-reversed item-selection-outside item-negative-coordinate item-large-coordinate item-missing-name item-missing-kind item-missing-uri item-missing-range item-missing-selectionRange item-object item-null-item item-atomic-second item-duplicate-name item-long-name edge-incoming edge-outgoing edge-empty edge-null edge-multiple edge-no-sites edge-repeated-sites edge-empty-site edge-wrong-direction edge-null-edge edge-missing-item edge-missing-sites edge-null-sites edge-bad-site edge-bad-late-site edge-bad-related edge-atomic-second edge-object edge-duplicate-sites cancelled wrong-id error null-output null-root null-uri bad-direction invalid-index items-many items-over sites-many sites-over)
        add_test(NAME framework.language_runtime.call_catalogue.${case} COMMAND umicom-call-catalogue-test ${case})
        set_tests_properties(framework.language_runtime.call_catalogue.${case} PROPERTIES TIMEOUT 30
            LABELS "framework;language;locations;ownership;regression")
    endforeach()
endif()

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/call-hierarchy.html" DESTINATION "${CMAKE_INSTALL_DATADIR}/doc/umicom/learning" COMPONENT Development)
