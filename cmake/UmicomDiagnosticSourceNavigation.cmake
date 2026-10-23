# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# File fixtures have separate working folders so parallel tests cannot create ambiguity for each other.
include_guard(GLOBAL)
if(NOT BUILD_TESTING)
    return()
endif()
function(umicom_diagnostic_source_target target source library)
    add_executable("${target}" "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../${source}")
    target_link_libraries("${target}" PRIVATE "${library}")
    set_target_properties("${target}" PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings("${target}")
    umicom_apply_sanitizers("${target}")
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target("${target}")
    endif()
endfunction()
umicom_diagnostic_source_target(umicom-diagnostic-location-test tests/compiler_stream/test_source_location.c Umicom::build)
umicom_diagnostic_source_target(umicom-path-root-test tests/test_path_root_containment.c Umicom::platform)
add_test(NAME framework.path.root_containment COMMAND umicom-path-root-test)
set_tests_properties(framework.path.root_containment PROPERTIES TIMEOUT 30 LABELS "framework;path;regression")
set(_umicom_source_cases source build absolute ambiguous same-root no-line uri pseudo unterminated missing escape generated-source bounds)
if(WIN32)
    list(APPEND _umicom_source_cases drive-relative alternate-stream network)
endif()
foreach(case IN LISTS _umicom_source_cases)
    set(directory "${CMAKE_CURRENT_BINARY_DIR}/diagnostic-source-fixtures/${case}")
    file(MAKE_DIRECTORY "${directory}")
    add_test(NAME framework.diagnostic_source.${case} COMMAND umicom-diagnostic-location-test "${case}")
    set_tests_properties(framework.diagnostic_source.${case} PROPERTIES
        WORKING_DIRECTORY "${directory}" TIMEOUT 30 LABELS "framework;build;diagnostics;source;regression")
endforeach()
if(TARGET Umicom::ui_gtk4)
    umicom_diagnostic_source_target(umicom-diagnostic-source-gtk-test tests/compiler_stream/test_source_gtk4.c Umicom::ui_gtk4)
    foreach(case cursor pending-page retained close stale)
        add_test(NAME framework.diagnostic_source.gtk4.${case} COMMAND umicom-diagnostic-source-gtk-test "${case}")
        set_tests_properties(framework.diagnostic_source.gtk4.${case} PROPERTIES
            TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;build;diagnostics;source;gtk4;ownership")
    endforeach()
endif()

# Use the real document coordinator and independent build-owned fixture folders.
umicom_diagnostic_source_target(umicom-diagnostic-navigation-workflow-test
    tests/test_diagnostic_navigation_workflow.c Umicom::diagnostic_ui)
foreach(case next previous all-missing missing-line dirty no-wrap invalid-base beyond-page history history-stale-line missing-line-only)
    set(directory "${CMAKE_CURRENT_BINARY_DIR}/diagnostic-navigation-fixtures/${case}")
    file(MAKE_DIRECTORY "${directory}")
    add_test(NAME framework.diagnostic_navigation.${case}
        COMMAND umicom-diagnostic-navigation-workflow-test "${case}" "${directory}")
    set_tests_properties(framework.diagnostic_navigation.${case} PROPERTIES
        TIMEOUT 30 LABELS "framework;document;diagnostics;source;regression")
endforeach()

# Captured diagnostic pages remain owned across native model/filter callbacks.
umicom_diagnostic_source_target(umicom-diagnostic-page-test tests/test_diagnostic_page.c Umicom::diagnostic_ui)
foreach(case empty pagination offset beyond-end filter resolved severity-order summary model-change model-clear model-identity query-change query-equivalence invalid-text invalid-limit arguments)
    add_test(NAME framework.diagnostic_page.${case} COMMAND umicom-diagnostic-page-test "${case}")
    set_tests_properties(framework.diagnostic_page.${case} PROPERTIES
        TIMEOUT 30 LABELS "framework;diagnostics;presentation;ownership;regression")
endforeach()

# Page navigation is a separate contract from arbitrary-offset diagnostic queries.
umicom_diagnostic_source_target(umicom-diagnostic-page-navigation-test
    tests/test_diagnostic_page_navigation.c Umicom::diagnostic_ui)
foreach(case empty first middle last exact-boundary unaligned huge-offset default-limit single-row filter resolved severity shrink clear retained invalid-limit invalid-query arguments arbitrary-offset)
    add_test(NAME framework.diagnostic_page.navigation.${case}
        COMMAND umicom-diagnostic-page-navigation-test "${case}")
    set_tests_properties(framework.diagnostic_page.navigation.${case} PROPERTIES
        TIMEOUT 30 LABELS "framework;diagnostics;presentation;navigation;regression")
endforeach()

# Validate producer records before model queries consume their inline strings.
umicom_diagnostic_source_target(umicom-diagnostic-text-validation-test
    tests/test_diagnostic_text_validation.c Umicom::diagnostic_ui)
foreach(case unterminated-id boundary-id unterminated-source boundary-source unterminated-code boundary-code unterminated-message boundary-message unterminated-detail boundary-detail unterminated-uri boundary-uri unterminated-owner boundary-owner unterminated-tags boundary-tags all-terminated message-buffer encoding)
    add_test(NAME framework.diagnostic_text.${case} COMMAND umicom-diagnostic-text-validation-test "${case}")
    set_tests_properties(framework.diagnostic_text.${case} PROPERTIES
        TIMEOUT 30 LABELS "framework;diagnostics;bounds;regression")
endforeach()
