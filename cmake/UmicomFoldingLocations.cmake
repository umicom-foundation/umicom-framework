# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
if(BUILD_TESTING)
    add_executable(umicom-folding-locations-test "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_folding_locations.c")
    target_link_libraries(umicom-folding-locations-test PRIVATE Umicom::developer)
    umicom_apply_warnings(umicom-folding-locations-test)
    umicom_apply_sanitizers(umicom-folding-locations-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-folding-locations-test)
    endif()
    foreach(case lines final-line trailing-newline crlf cr unicode empty-source single-line nested duplicate unordered characters-ignored future-kind null empty-array object null-row missing-start missing-end negative fraction reversed outside partial-invalid start-character-invalid end-character-invalid kind-invalid label-invalid wrong-id error invalid-uri invalid-source embedded-null null-source null-output cancelled owned capacity capacity-over)
        add_test(NAME framework.language_runtime.folding_locations.${case} COMMAND umicom-folding-locations-test ${case})
        set_tests_properties(framework.language_runtime.folding_locations.${case} PROPERTIES TIMEOUT 30
            LABELS "framework;language;locations;ownership;regression")
    endforeach()
endif()

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/syntax-folding.html" DESTINATION "${CMAKE_INSTALL_DATADIR}/doc/umicom/learning" COMPONENT Development)
