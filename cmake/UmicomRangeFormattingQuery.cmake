# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
if(BUILD_TESTING)
    add_executable(umicom-range-formatting-query-test "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_range_formatting_query.c")
    target_link_libraries(umicom-range-formatting-query-test PRIVATE Umicom::developer Umicom::editor Umicom::platform)
    umicom_apply_warnings(umicom-range-formatting-query-test)
    umicom_apply_sanitizers(umicom-range-formatting-query-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-range-formatting-query-test)
    endif()
    foreach(case valid options object missing disabled wrong-provider encoding sync fragmented notification error invalid-edits overlap shutdown-error close-error timeout cancel-before cancel-during invalid-source invalid-options state empty unicode native-invalid read-error write-error escaped-limit range-reversed range-outside range-start-scalar range-end-scalar range-empty range-surrounding caret-independent)
        add_test(NAME framework.language_runtime.range_formatting_query.${case} COMMAND umicom-range-formatting-query-test ${case})
        set_tests_properties(framework.language_runtime.range_formatting_query.${case} PROPERTIES TIMEOUT 30
            LABELS "framework;language;formatting;ownership;regression")
    endforeach()
endif()

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/range-formatting.html" DESTINATION "${CMAKE_INSTALL_DATADIR}/doc/umicom/learning" COMPONENT Development)
