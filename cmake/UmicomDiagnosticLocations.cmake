# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
if(BUILD_TESTING)
    add_executable(umicom-diagnostic-locations-test "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_diagnostic_locations.c")
    target_link_libraries(umicom-diagnostic-locations-test PRIVATE Umicom::developer Umicom::editor Umicom::platform)
    umicom_apply_warnings(umicom-diagnostic-locations-test)
    umicom_apply_sanitizers(umicom-diagnostic-locations-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-diagnostic-locations-test)
    endif()
    foreach(case primary first-related second-related second-diagnostic no-related empty invalid-diagnostic invalid-location large-index null-owner null-output owned-uri pull)
        add_test(NAME framework.language_runtime.diagnostic_locations.${case} COMMAND umicom-diagnostic-locations-test ${case})
        set_tests_properties(framework.language_runtime.diagnostic_locations.${case} PROPERTIES TIMEOUT 30
            LABELS "framework;language;diagnostic;ownership;regression")
    endforeach()
endif()

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/diagnostic-locations.html" DESTINATION "${CMAKE_INSTALL_DATADIR}/doc/umicom/learning" COMPONENT Development)
