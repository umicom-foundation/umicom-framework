# Umicom Foundation | Sammy Hegab | MIT
add_executable(umicom-market-tape-test test_tape.c)
target_link_libraries(umicom-market-tape-test PRIVATE Umicom::market_tape)
umicom_market_tape_target(umicom-market-tape-test)
set(_cases config empty invalid_instrument capacity duplicate_instrument unknown unopened
    waiting begin_validation bad_packets nonfinite_quote crossed_quote negative_time future_time
    identity_mismatch bad_trade depth_sort depth_duplicate depth_crossed depth_count depth_nonfinite
    depth_empty depth_valid sequence_max gap_first concurrency bars_ohlcv bars_boundary bars_empty_time
    volume_overflow time_overflow ring_retention snapshot_lifetime bar_reference quote duplicate
    old_generation receive_regression event_regression invalid_no_sequence_consumption gap_latch
    reset disconnect stale_threshold lane_freshness read_clock selected_context
    quality_0 quality_1 quality_2 quality_3 quality_4 quality_5 quality_6)
foreach(_case IN LISTS _cases)
    add_test(NAME framework.market_tape.${_case} COMMAND umicom-market-tape-test ${_case})
    set_tests_properties(framework.market_tape.${_case} PROPERTIES TIMEOUT 30 LABELS "market-tape;native")
endforeach()
add_test(NAME framework.market_tape.self_test COMMAND umicom-market-tape --self-test)
add_test(NAME framework.market_tape.practice_demo COMMAND umicom-market-tape demo)
add_test(NAME framework.market_tape.lesson COMMAND umicom-market-tape-example)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_executable(umicom-market-tape-alloc-test test_alloc.c)
    target_link_libraries(umicom-market-tape-alloc-test PRIVATE Umicom::market_tape)
    target_link_options(umicom-market-tape-alloc-test PRIVATE -Wl,--wrap=calloc)
    umicom_market_tape_target(umicom-market-tape-alloc-test)
    add_test(NAME framework.market_tape.allocation_failures COMMAND umicom-market-tape-alloc-test)
endif()
add_executable(umicom-market-tape-edge-test test_edges.c)
target_link_libraries(umicom-market-tape-edge-test PRIVATE Umicom::market_tape)
umicom_market_tape_target(umicom-market-tape-edge-test)
foreach(_case practice_controls practice_bad_input timestamp_zero custom_interval gap_newer_timestamp
        snapshot_not_borrowed clock_is_projection depth_lane_independent delayed_source
        idempotent_selection packet_mutation)
    add_test(NAME framework.market_tape.${_case} COMMAND umicom-market-tape-edge-test ${_case})
    set_tests_properties(framework.market_tape.${_case} PROPERTIES TIMEOUT 30 LABELS "market-tape;native")
endforeach()
