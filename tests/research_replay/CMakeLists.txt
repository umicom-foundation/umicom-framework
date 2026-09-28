# Umicom Foundation | Sammy Hegab | MIT
# Resolve test sources beside this list file, not in the deferred
# caller directory. The prior relative-path command is retained for review.
if(FALSE)
add_executable(umicom-research-replay-test test_replay.c
    "${CMAKE_CURRENT_LIST_DIR}/../../examples/research_replay/strategy.c")
endif()
add_executable(umicom-research-replay-test "${CMAKE_CURRENT_LIST_DIR}/test_replay.c"
    "${CMAKE_CURRENT_LIST_DIR}/../../examples/research_replay/strategy.c")
target_include_directories(umicom-research-replay-test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../../examples/research_replay")
target_link_libraries(umicom-research-replay-test PRIVATE Umicom::research_replay)
umicom_research_target(umicom-research-replay-test)
foreach(_case IN ITEMS
    invalid_arguments
    config_finite
    identity
    sequence
    timestamp
    copied_input
    next_observation
    latency
    commission
    slippage
    short
    reversal
    liquidity
    cancel
    callback_error
    invalid_decision
    reentry
    end_of_data
    open_position
    step_budget
    numeric_failure
    timestamp_boundary
    limit
    pending_cancel
    fingerprint
    determinism
    no_future
    backtest_nonfinite
    backtest_overflow
    backtest_snapshot
    backtest_legacy
    csv_valid
    csv_reject
    csv_limits
    independent_reference
    parser_mutations)
    add_test(NAME "framework.research_replay.${_case}" COMMAND umicom-research-replay-test "${_case}")
    set_tests_properties("framework.research_replay.${_case}" PROPERTIES TIMEOUT 30)
endforeach()
add_test(NAME framework.research_replay.cli_demo COMMAND umicom-research-replay demo)
add_test(NAME framework.research_replay.cli_csv COMMAND umicom-research-replay csv "${CMAKE_CURRENT_LIST_DIR}/../../examples/research_replay/quotes.csv")
add_test(NAME framework.research_replay.cli_sweep COMMAND umicom-research-replay sweep)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    # Resolve test sources beside this list file, not in the deferred
    # caller directory. The prior relative-path command is retained for review.
    if(FALSE)
    add_executable(umicom-research-allocation-test test_alloc.c "${CMAKE_CURRENT_LIST_DIR}/../../examples/research_replay/strategy.c")
    endif()
    add_executable(umicom-research-allocation-test "${CMAKE_CURRENT_LIST_DIR}/test_alloc.c" "${CMAKE_CURRENT_LIST_DIR}/../../examples/research_replay/strategy.c")
    target_include_directories(umicom-research-allocation-test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../../examples/research_replay")
    target_link_libraries(umicom-research-allocation-test PRIVATE Umicom::research_replay)
    target_link_options(umicom-research-allocation-test PRIVATE -Wl,--wrap=calloc)
    umicom_research_target(umicom-research-allocation-test)
    add_test(NAME framework.research_replay.allocation_failure COMMAND umicom-research-allocation-test)
endif()

add_test(NAME framework.research_replay.cli_self_test COMMAND umicom-research-replay --self-test)
