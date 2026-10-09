# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# A persistent worker owns protocol I/O; frontends exchange copied source snapshots.
include_guard(GLOBAL)
target_sources(umicom_developer PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/diagnostic_session_wire.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/diagnostic_session_protocol.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/diagnostic_session.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/diagnostic_session_poll.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/diagnostic_monitor.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/diagnostic_monitor_worker.c")
if(TARGET umicom_ui_gtk4)
    target_sources(umicom_ui_gtk4 PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/live_diagnostics_gtk4.c")
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/LIVE_SOURCE_DIAGNOSTICS.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/docs" COMPONENT Framework)
if(NOT BUILD_TESTING)
    return()
endif()
function(umicom_live_diagnostic_test target source library)
    add_executable("${target}" "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/diagnostic_session/${source}")
    target_link_libraries("${target}" PRIVATE "${library}")
    set_target_properties("${target}" PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings("${target}")
    umicom_apply_sanitizers("${target}")
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target("${target}")
    endif()
endfunction()
function(umicom_live_diagnostic_case group target case)
    add_test(NAME framework.live_diagnostics.${group}.${case}
        COMMAND "${target}" "${case}" ${ARGN})
    set_tests_properties(framework.live_diagnostics.${group}.${case} PROPERTIES
        TIMEOUT 60 SKIP_RETURN_CODE 77 LABELS "framework;language;diagnostics;ownership;regression")
endfunction()
umicom_live_diagnostic_test(umicom-live-diagnostic-negotiation-test test_negotiation.c Umicom::developer)
umicom_live_diagnostic_test(umicom-live-diagnostic-updates-test test_updates.c Umicom::developer)
umicom_live_diagnostic_test(umicom-live-diagnostic-failures-test test_failures.c Umicom::developer)
foreach(case full incremental object missing-change closed none encoding duplicate legacy request timeout cancel)
    umicom_live_diagnostic_case(negotiation umicom-live-diagnostic-negotiation-test "${case}")
endforeach()
foreach(case full incremental freshness same stale invalid capacity)
    umicom_live_diagnostic_case(updates umicom-live-diagnostic-updates-test "${case}")
endforeach()
foreach(case write range malformed read fragmented unversioned quiet cancel)
    umicom_live_diagnostic_case(failures umicom-live-diagnostic-failures-test "${case}")
endforeach()
add_executable(umicom-captured-source-navigation-test
    "${CMAKE_CURRENT_LIST_DIR}/../tests/document/test_captured_source_navigation.c")
target_link_libraries(umicom-captured-source-navigation-test PRIVATE Umicom::document)
set_target_properties(umicom-captured-source-navigation-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
umicom_apply_warnings(umicom-captured-source-navigation-test)
umicom_apply_sanitizers(umicom-captured-source-navigation-test)
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-captured-source-navigation-test)
endif()
foreach(case same inactive caret read-only unicode changed shorter renamed split reversed outside unknown arguments)
    umicom_live_diagnostic_case(source umicom-captured-source-navigation-test "${case}")
endforeach()
if(WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Linux")
    umicom_live_diagnostic_test(umicom-live-diagnostic-server-fixture server_fixture.c Umicom::developer)
    umicom_live_diagnostic_test(umicom-live-diagnostic-monitor-test test_monitor.c Umicom::developer)
    add_dependencies(umicom-live-diagnostic-monitor-test umicom-live-diagnostic-server-fixture)
    foreach(case normal delayed unversioned coalesce prestop invalid same request)
        umicom_live_diagnostic_case(monitor umicom-live-diagnostic-monitor-test "${case}"
            "$<TARGET_FILE:umicom-live-diagnostic-server-fixture>")
    endforeach()
    if(TARGET umicom_ui_gtk4)
        umicom_live_diagnostic_test(umicom-live-diagnostic-gtk-test test_gtk4.c Umicom::ui_gtk4)
        add_dependencies(umicom-live-diagnostic-gtk-test umicom-live-diagnostic-server-fixture)
        foreach(case update reentrant revoke document-close hidden close retained denied capture-close navigate navigate-stale navigate-denied navigate-close)
            umicom_live_diagnostic_case(gtk4 umicom-live-diagnostic-gtk-test "${case}"
                "$<TARGET_FILE:umicom-live-diagnostic-server-fixture>")
        endforeach()
    endif()
endif()
