# Umicom Framework
# File: cmake/UmicomEventCalendarChecks.cmake
# PURPOSE:
#   Register independent schema and native calendar regression checks.
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
# LICENCE:
# MIT

if(BUILD_TESTING AND TARGET Umicom::trading_ui AND NOT TARGET umicom-event-calendar-tests)
    add_executable(umicom-event-calendar-tests
        "${UMICOM_TRADING_WORKSTATION_ROOT}/tests/event_calendar/test_event_calendar.c")
    target_link_libraries(umicom-event-calendar-tests PRIVATE Umicom::trading_ui)
    umicom_apply_warnings(umicom-event-calendar-tests)
    umicom_apply_sanitizers(umicom-event-calendar-tests)
    foreach(case ordering missing-values earnings date-range country-currency search importance
            pages invalid-filter leap-date invalid-time observed-at identity units unknown-enum
            duplicate source-identity schema quoted-title control-text overlong header-only
            byte-bound owned-copy row-bound)
        add_test(NAME "framework.event_calendar.${case}" COMMAND umicom-event-calendar-tests "${case}")
        set_tests_properties("framework.event_calendar.${case}" PROPERTIES
            TIMEOUT 30 LABELS "framework;trading;calendar;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-event-calendar-tests)
    endif()
endif()

if(BUILD_TESTING AND TARGET Umicom::ui_gtk4 AND TARGET Umicom::trading_ui AND NOT TARGET umicom-event-calendar-native-tests)
    add_executable(umicom-event-calendar-native-tests
        "${UMICOM_TRADING_WORKSTATION_ROOT}/tests/event_calendar/test_event_calendar_gtk4.c")
    target_link_libraries(umicom-event-calendar-native-tests PRIVATE Umicom::ui_gtk4)
    umicom_apply_warnings(umicom-event-calendar-native-tests)
    umicom_apply_sanitizers(umicom-event-calendar-native-tests)
    foreach(case filter kind invalid-filter invalid-import quote-refresh details retained detached
            today clear-filters search-enter)
        add_test(NAME "framework.event_calendar.native.${case}" COMMAND umicom-event-calendar-native-tests "${case}")
        set_tests_properties("framework.event_calendar.native.${case}" PROPERTIES
            TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;trading;calendar;gtk4;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-event-calendar-native-tests)
    endif()
endif()

# File-import exercises use isolated fixtures below the owner's build directory.
if(BUILD_TESTING AND TARGET Umicom::ui_gtk4 AND TARGET Umicom::trading_ui AND NOT TARGET umicom-event-calendar-file-tests)
    add_executable(umicom-event-calendar-file-tests
        "${UMICOM_TRADING_WORKSTATION_ROOT}/tests/event_calendar/test_event_calendar_file_gtk4.c")
    target_link_libraries(umicom-event-calendar-file-tests PRIVATE Umicom::ui_gtk4)
    umicom_apply_warnings(umicom-event-calendar-file-tests)
    umicom_apply_sanitizers(umicom-event-calendar-file-tests)
    foreach(case valid invalid missing limit cancel detached destroyed nonlocal)
        add_test(NAME "framework.event_calendar.file.${case}" COMMAND umicom-event-calendar-file-tests
            "${case}" "${CMAKE_CURRENT_BINARY_DIR}/event-calendar-fixtures")
        set_tests_properties("framework.event_calendar.file.${case}" PROPERTIES
            TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;trading;calendar;gtk4;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-event-calendar-file-tests)
    endif()
endif()
