# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Historical presentation depends on the existing chart module; the broker
# transport itself stays usable by command-line and minimal native consumers.
include_guard(GLOBAL)
if(NOT TARGET Umicom::chart)
    return()
endif()
add_library(umicom_ibkr_historical_chart STATIC
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/historical_chart.c")
add_library(Umicom::ibkr_historical_chart ALIAS umicom_ibkr_historical_chart)
set_target_properties(umicom_ibkr_historical_chart PROPERTIES EXPORT_NAME ibkr_historical_chart)
target_link_libraries(umicom_ibkr_historical_chart PUBLIC Umicom::ibkr_connection Umicom::chart)
umicom_ibkr_target(umicom_ibkr_historical_chart)
install(TARGETS umicom_ibkr_historical_chart EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
if(BUILD_TESTING)
    add_executable(umicom-ibkr-historical_chart-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ibkr_connection/test_historical_chart.c")
    target_include_directories(umicom-ibkr-historical_chart-test PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection")
    target_link_libraries(umicom-ibkr-historical_chart-test PRIVATE Umicom::ibkr_historical_chart)
    umicom_ibkr_target(umicom-ibkr-historical_chart-test)
    foreach(case candle scene single pan bad-range no-output pending age disconnect precision missing-volume empty owned failure-preserves)
        add_test(NAME framework.ibkr_historical_chart.${case} COMMAND umicom-ibkr-historical_chart-test ${case})
        set_tests_properties(framework.ibkr_historical_chart.${case} PROPERTIES TIMEOUT 20
            LABELS "framework;broker;historical-data;chart;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-ibkr-historical_chart-test)
    endif()
endif()

# The existing optional chart owner also serves rolling broker series.
target_sources(umicom_ibkr_historical_chart PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/bar_chart.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/realtime_chart.c")

if(BUILD_TESTING)
    add_executable(umicom-ibkr-realtime_chart-test "${CMAKE_CURRENT_LIST_DIR}/../tests/ibkr_connection/test_realtime_chart.c")
    target_include_directories(umicom-ibkr-realtime_chart-test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection")
    target_link_libraries(umicom-ibkr-realtime_chart-test PRIVATE Umicom::ibkr_historical_chart)
    umicom_ibkr_target(umicom-ibkr-realtime_chart-test)
    foreach(case candle scene single pan pending age cancelled disconnect precision missing-volume bad-range no-output owned frozen)
        add_test(NAME framework.ibkr_realtime_chart.${case} COMMAND umicom-ibkr-realtime_chart-test ${case})
        set_tests_properties(framework.ibkr_realtime_chart.${case} PROPERTIES TIMEOUT 20
            LABELS "framework;broker;streaming-bars;chart;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-ibkr-realtime_chart-test)
    endif()
endif()
