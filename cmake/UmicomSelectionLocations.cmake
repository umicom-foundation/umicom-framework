# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
if(BUILD_TESTING)
    add_executable(umicom-selection-locations-test "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_selection_locations.c")
    target_link_libraries(umicom-selection-locations-test PRIVATE Umicom::developer)
    umicom_apply_warnings(umicom-selection-locations-test)
    umicom_apply_sanitizers(umicom-selection-locations-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-selection-locations-test)
    endif()
    foreach(case single parents null empty-array multiple object null-row missing-range missing-start reversed negative fraction caret-outside empty-at-caret caret-at-end parent-shrinks parent-null parent-string equal-parent extension owned-uri invalid-uri null-uri invalid-position null-output wrong-id error cancelled depth-limit depth-over)
        add_test(NAME framework.language_runtime.selection_locations.${case} COMMAND umicom-selection-locations-test ${case})
        set_tests_properties(framework.language_runtime.selection_locations.${case} PROPERTIES TIMEOUT 30
            LABELS "framework;language;locations;ownership;regression")
    endforeach()
endif()

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/selection-ranges.html" DESTINATION "${CMAKE_INSTALL_DATADIR}/doc/umicom/learning" COMPONENT Development)
