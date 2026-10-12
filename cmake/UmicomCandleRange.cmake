# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Portable measurements and native gestures share one chart implementation.
include_guard(GLOBAL)
if(BUILD_TESTING)
    foreach(name candle_range range_capture range_report)
        add_executable(umicom-${name}-test "${CMAKE_CURRENT_LIST_DIR}/../tests/chart/test_${name}.c")
        target_link_libraries(umicom-${name}-test PRIVATE Umicom::chart)
        umicom_apply_warnings(umicom-${name}-test)
        umicom_apply_sanitizers(umicom-${name}-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-${name}-test)
        endif()
    endforeach()
    foreach(case values reverse single zero-open negative-open extreme-time large-price percent-overflow
            bounds empty capacity nan infinite negative-volume ohlc duplicate unsorted outside-corrupt
            volume-overflow range-overflow change-overflow null)
        add_test(NAME framework.chart.candle_range.${case} COMMAND umicom-candle_range-test ${case})
        set_tests_properties(framework.chart.candle_range.${case} PROPERTIES
            TIMEOUT 20 LABELS "framework;chart;regression")
    endforeach()
    foreach(case empty owned reverse tie window preserve bounds replace clear rows)
        add_test(NAME framework.chart.range_capture.${case} COMMAND umicom-range_capture-test ${case})
        set_tests_properties(framework.chart.range_capture.${case} PROPERTIES
            TIMEOUT 20 LABELS "framework;chart;ownership;regression")
    endforeach()
    foreach(case values zero-open pending formula control long empty-id locale owned)
        add_test(NAME framework.chart.range_report.${case} COMMAND umicom-range_report-test ${case})
        set_tests_properties(framework.chart.range_report.${case} PROPERTIES
            TIMEOUT 20 SKIP_RETURN_CODE 77 LABELS "framework;chart;csv;regression")
    endforeach()
endif()
if(BUILD_TESTING AND TARGET umicom_ui_gtk4)
    add_executable(umicom-candle-range-gtk-test "${CMAKE_CURRENT_LIST_DIR}/../tests/chart/test_candle_range_gtk4.c")
    target_link_libraries(umicom-candle-range-gtk-test PRIVATE Umicom::ui_gtk4)
    umicom_apply_warnings(umicom-candle-range-gtk-test)
    umicom_apply_sanitizers(umicom-candle-range-gtk-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-candle-range-gtk-test)
    endif()
    foreach(case visible copy gesture rows invalid-rows snapshot pending-update clear timeframe instrument
            escape projection outside retained detached reentrant-detach)
        add_test(NAME framework.chart.candle_range.gtk.${case} COMMAND umicom-candle-range-gtk-test ${case})
        set_tests_properties(framework.chart.candle_range.gtk.${case} PROPERTIES
            TIMEOUT 20 SKIP_RETURN_CODE 77 LABELS "framework;chart;gtk4;ownership;regression")
    endforeach()
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/measure-chart-candles.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
