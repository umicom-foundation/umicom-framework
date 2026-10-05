# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
if(BUILD_TESTING)
    add_executable(umicom-action-kind-filter-test "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_action_kind_filter.c")
    target_link_libraries(umicom-action-kind-filter-test PRIVATE Umicom::developer Umicom::editor Umicom::platform)
    umicom_apply_warnings(umicom-action-kind-filter-test)
    umicom_apply_sanitizers(umicom-action-kind-filter-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-action-kind-filter-test)
    endif()
    foreach(case all quickfix refactor organize fix-all descendants prefix-boundary missing-kind empty-kind unrelated empty null independent original-json disabled command deferred preferred invalid-filter null-source null-output name-invalid name-null cancelled capacity order)
        add_test(NAME framework.language_runtime.action_kind_filter.${case} COMMAND umicom-action-kind-filter-test ${case})
        set_tests_properties(framework.language_runtime.action_kind_filter.${case} PROPERTIES TIMEOUT 30
            LABELS "framework;language;navigation;ownership;regression")
    endforeach()
endif()

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/action-families.html" DESTINATION "${CMAKE_INSTALL_DATADIR}/doc/umicom/learning" COMPONENT Development)
