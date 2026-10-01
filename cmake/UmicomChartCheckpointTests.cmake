# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Regression cases exercise persistence contracts, not generated file counts.
include_guard(GLOBAL)
if(NOT BUILD_TESTING)
    return()
endif()
function(umicom_chart_checkpoint_test group source library)
    set(target "umicom-chart-checkpoint-${group}-test")
    add_executable(${target} "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/chart_checkpoint/${source}")
    target_link_libraries(${target} PRIVATE ${library})
    umicom_apply_warnings(${target})
    umicom_apply_sanitizers(${target})
    foreach(case IN LISTS ARGN)
        add_test(NAME "framework.chart_checkpoint.${group}.${case}" COMMAND ${target} ${case})
        set_tests_properties("framework.chart_checkpoint.${group}.${case}" PROPERTIES
            TIMEOUT 120 SKIP_RETURN_CODE 77 LABELS "framework;chart;persistence;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(${target})
    endif()
endfunction()
umicom_chart_checkpoint_test(document test_document.c Umicom::chart
    ownership replace empty collision invalid capacity)
umicom_chart_checkpoint_test(codec test_codec.c Umicom::chart_checkpoint
    exact duplicate nul numeric metadata)
umicom_chart_checkpoint_test(store test_store.c Umicom::chart_checkpoint
    conflict transaction scope rollback recovery digest double-corruption missing-primary empty)
umicom_chart_checkpoint_test(trading test_trading.c Umicom::trading_chart_persistence
    restore stale-view stale-drawing stale-storage preview-identity new-session rebind unsupported restored-identity)
target_link_libraries(umicom-chart-checkpoint-trading-test PRIVATE Umicom::trading_ui)
umicom_chart_checkpoint_test(sqlite test_sqlite.c Umicom::chart_checkpoint)
foreach(case restart rollback automatic-abort capacity)
    add_test(NAME "framework.chart_checkpoint.sqlite.${case}"
        COMMAND umicom-chart-checkpoint-sqlite-test ${case} "${CMAKE_CURRENT_BINARY_DIR}/chart-checkpoint-${case}.sqlite3")
    set_tests_properties("framework.chart_checkpoint.sqlite.${case}" PROPERTIES
        TIMEOUT 120 SKIP_RETURN_CODE 77 LABELS "framework;chart;sqlite;regression")
endforeach()
if(TARGET Umicom::ui_gtk4)
    umicom_chart_checkpoint_test(native test_native.c Umicom::ui_gtk4
        restore selection unbind retained-root retained-body)
endif()
