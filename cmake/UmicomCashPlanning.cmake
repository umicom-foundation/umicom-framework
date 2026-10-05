# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Financial assumptions reuse checked money/date owners. JSON and file services
# stay dependency-linked, with no product-specific financial implementation.
include_guard(GLOBAL)
include(GNUInstallDirs)
add_library(umicom_cash_planning STATIC
    "${CMAKE_CURRENT_LIST_DIR}/../src/cash_planning/plan.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/cash_planning/date.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/cash_planning/forecast.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/cash_planning/document.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/cash_planning/report.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/cash_planning/file.c"
 )
add_library(Umicom::cash_planning ALIAS umicom_cash_planning)
set_target_properties(umicom_cash_planning PROPERTIES EXPORT_NAME cash_planning C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_include_directories(umicom_cash_planning PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../include> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(umicom_cash_planning PUBLIC Umicom::finance Umicom::platform PRIVATE Umicom::document Umicom::developer)
umicom_apply_warnings(umicom_cash_planning)
umicom_apply_sanitizers(umicom_cash_planning)
target_link_libraries(umicom_framework INTERFACE Umicom::cash_planning)
install(TARGETS umicom_cash_planning EXPORT UmicomFrameworkTargets ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
install(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/cash_planning" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom COMPONENT Framework)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/cash-planning.html" DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/learning COMPONENT Learning)
if(TARGET umicom_ui_gtk4)
    target_sources(umicom_ui_gtk4 PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/cash_plan_gtk4.c"
        "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/cash_plan_files_gtk4.c")
    target_link_libraries(umicom_ui_gtk4 PUBLIC Umicom::cash_planning)
    install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/ui/gtk4/cash_plan.h" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4 COMPONENT Framework)
endif()
if(BUILD_TESTING)
    add_executable(umicom-cash-plan-model-test "${CMAKE_CURRENT_LIST_DIR}/../tests/cash_planning/test_model.c")
    target_link_libraries(umicom-cash-plan-model-test PRIVATE Umicom::cash_planning)
    umicom_apply_warnings(umicom-cash-plan-model-test)
    umicom_apply_sanitizers(umicom-cash-plan-model-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-cash-plan-model-test)
    endif()
    foreach(case projection empty copy duplicate replace remove capacity invalid currency scale disabled outside same-day order overflow headroom-overflow total-overflow dates date-refusal utf8 unterminated)
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/cash-plan/model/${case}")
        add_test(NAME framework.cash_planning.model.${case} COMMAND umicom-cash-plan-model-test ${case})
        set_tests_properties(framework.cash_planning.model.${case} PROPERTIES TIMEOUT 30
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/cash-plan/model/${case}"
            LABELS "framework;cash-planning;finance;regression")
    endforeach()
    add_executable(umicom-cash-plan-document-test "${CMAKE_CURRENT_LIST_DIR}/../tests/cash_planning/test_document.c")
    target_link_libraries(umicom-cash-plan-document-test PRIVATE Umicom::cash_planning)
    umicom_apply_warnings(umicom-cash-plan-document-test)
    umicom_apply_sanitizers(umicom-cash-plan-document-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-cash-plan-document-test)
    endif()
    foreach(case round-trip maximum unknown duplicate missing format wrong-kind negative invalid-date duplicate-id boolean fractional overflow embedded-nul trailing truncated escaping csv csv-overflow)
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/cash-plan/document/${case}")
        add_test(NAME framework.cash_planning.document.${case} COMMAND umicom-cash-plan-document-test ${case})
        set_tests_properties(framework.cash_planning.document.${case} PROPERTIES TIMEOUT 30
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/cash-plan/document/${case}"
            LABELS "framework;cash-planning;finance;regression")
    endforeach()
    add_executable(umicom-cash-plan-file-test "${CMAKE_CURRENT_LIST_DIR}/../tests/cash_planning/test_file.c")
    target_link_libraries(umicom-cash-plan-file-test PRIVATE Umicom::cash_planning)
    umicom_apply_warnings(umicom-cash-plan-file-test)
    umicom_apply_sanitizers(umicom-cash-plan-file-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-cash-plan-file-test)
    endif()
    foreach(case relative missing malformed round-trip no-overwrite export)
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/cash-plan/file/${case}")
        add_test(NAME framework.cash_planning.file.${case} COMMAND umicom-cash-plan-file-test ${case})
        set_tests_properties(framework.cash_planning.file.${case} PROPERTIES TIMEOUT 30
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/cash-plan/file/${case}"
            LABELS "framework;cash-planning;finance;regression")
    endforeach()
    if(TARGET umicom_ui_gtk4)
        add_executable(umicom-cash-plan-gtk4-test "${CMAKE_CURRENT_LIST_DIR}/../tests/cash_planning/test_gtk4.c")
        target_link_libraries(umicom-cash-plan-gtk4-test PRIVATE Umicom::ui_gtk4)
        umicom_apply_warnings(umicom-cash-plan-gtk4-test)
        umicom_apply_sanitizers(umicom-cash-plan-gtk4-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-cash-plan-gtk4-test)
        endif()
        foreach(case add update remove undo settings currency draft invalid disabled independent retained load approve invalidate close-loading changed-loading)
            file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/cash-plan/gtk4/${case}")
            add_test(NAME framework.cash_planning.gtk4.${case} COMMAND umicom-cash-plan-gtk4-test ${case})
            set_tests_properties(framework.cash_planning.gtk4.${case} PROPERTIES TIMEOUT 30
                WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/cash-plan/gtk4/${case}"
                LABELS "framework;cash-planning;finance;regression;gtk4" SKIP_RETURN_CODE 77)
        endforeach()
    endif()
endif()

# Keep the small portable lesson available without requiring a native display.
if(UMICOM_BUILD_LEARNING_EXAMPLES)
    add_executable(umicom-cash-planning-lesson "${CMAKE_CURRENT_LIST_DIR}/../examples/cash_planning/main.c")
    target_link_libraries(umicom-cash-planning-lesson PRIVATE Umicom::cash_planning)
    umicom_apply_warnings(umicom-cash-planning-lesson)
    umicom_apply_sanitizers(umicom-cash-planning-lesson)
endif()
