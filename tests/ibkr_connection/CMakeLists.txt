# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Resolve test sources beside this list file, not in the deferred
# caller directory. The prior relative-path command is retained for review.
if(FALSE)
add_executable(umicom-ibkr-connection-test test_connection.c)
endif()
add_executable(umicom-ibkr-connection-test "${CMAKE_CURRENT_LIST_DIR}/test_connection.c")
target_include_directories(umicom-ibkr-connection-test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../../src/ibkr_connection")
target_link_libraries(umicom-ibkr-connection-test PRIVATE Umicom::ibkr_connection)
umicom_ibkr_target(umicom-ibkr-connection-test)
foreach(_case IN ITEMS
    policy_defaults
    policy_ports
    policy_live_ack
    policy_no_remote
    policy_read_only
    policy_client_zero
    policy_invalid_mode
    policy_timeout
    policy_account_before_connection
    policy_no_port_attestation
    mapping_nan_quantity
    mapping_infinite_quantity
    mapping_invalid_side
    mapping_invalid_mode
    mapping_nan_price
    mapping_limit_required
    mapping_stop_required
    mapping_policy_bool
    mapping_unterminated_host
    mapping_unterminated_account
    mapping_read_only
    mapping_paper_live
    mapping_valid
    wire_open_once
    wire_connect_timeout
    wire_write_failure
    wire_zero_write
    wire_partial_write
    wire_handshake_version
    wire_handshake_timeout
    wire_before_ready
    wire_fragmented
    wire_ready
    wire_unlisted_account
    wire_backwards_time
    wire_disconnect_stale
    wire_eof
    wire_unknown
    wire_bad_length
    wire_zero_length
    wire_missing_terminator
    wire_farm_message
    wire_backend_lost
    wire_heartbeat_timeout
    wire_accounts_changed
    wire_accounts_reordered
    wire_accounts_duplicate
    wire_outbound_allowlist
    wire_read_once
    wire_request_timeout
    wire_account_filter
    wire_request_id_filter
    wire_bad_position
    wire_unterminated_fields
    wire_empty_snapshot
    wire_utf8_invalid
    wire_value_capacity
    wire_partial_not_complete
    wire_complete_snapshot
    wire_late_rows_ignored
    wire_accounts_trailing_comma
    wire_accounts_empty_interior
    wire_long_diagnostic
    wire_account_capacity
    wire_position_capacity
    wire_snapshot_independence
    wire_frame_budget
    wire_queue_atomic
    numbers
    mutation)
    add_test(NAME framework.ibkr_connection.${_case} COMMAND umicom-ibkr-connection-test ${_case})
    set_tests_properties(framework.ibkr_connection.${_case} PROPERTIES TIMEOUT 30)
endforeach()
add_test(NAME framework.ibkr_connection.policy_lesson COMMAND umicom-broker-profile-example)
add_test(NAME framework.ibkr_connection.cli_self_test COMMAND umicom-broker-connect --self-test)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # Resolve test sources beside this list file, not in the deferred
    # caller directory. The prior relative-path command is retained for review.
    if(FALSE)
    add_executable(umicom-ibkr-loopback-test test_loopback.c)
    endif()
    add_executable(umicom-ibkr-loopback-test "${CMAKE_CURRENT_LIST_DIR}/test_loopback.c")
    target_link_libraries(umicom-ibkr-loopback-test PRIVATE Umicom::ibkr_connection)
    umicom_ibkr_target(umicom-ibkr-loopback-test)
    foreach(_mode IN ITEMS paper live)
        add_test(NAME framework.ibkr_connection.loopback_${_mode} COMMAND umicom-ibkr-loopback-test ${_mode})
        set_tests_properties(framework.ibkr_connection.loopback_${_mode} PROPERTIES TIMEOUT 15)
    endforeach()
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        # Resolve test sources beside this list file, not in the deferred
        # caller directory. The prior relative-path command is retained for review.
        if(FALSE)
        add_executable(umicom-ibkr-allocation-test test_alloc.c)
        endif()
        add_executable(umicom-ibkr-allocation-test "${CMAKE_CURRENT_LIST_DIR}/test_alloc.c")
        target_include_directories(umicom-ibkr-allocation-test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../../src/ibkr_connection")
        target_link_libraries(umicom-ibkr-allocation-test PRIVATE Umicom::ibkr_connection)
        target_link_options(umicom-ibkr-allocation-test PRIVATE -Wl,--wrap=calloc -Wl,--wrap=malloc)
        umicom_ibkr_target(umicom-ibkr-allocation-test)
        add_test(NAME framework.ibkr_connection.allocation_failures COMMAND umicom-ibkr-allocation-test)
    endif()
endif()

# Retain the canonical mapping regression with assertions active in Release.
add_executable(umicom-ibkr-canonical-mapping-test "${CMAKE_CURRENT_LIST_DIR}/../broker_connectivity/test_ibkr_adapter.c")
target_link_libraries(umicom-ibkr-canonical-mapping-test PRIVATE Umicom::ibkr_connection)
umicom_ibkr_target(umicom-ibkr-canonical-mapping-test)
if(MSVC)
    target_compile_options(umicom-ibkr-canonical-mapping-test PRIVATE /UNDEBUG)
else()
    target_compile_options(umicom-ibkr-canonical-mapping-test PRIVATE -UNDEBUG)
endif()
add_test(NAME framework.ibkr_connection.canonical_mapping COMMAND umicom-ibkr-canonical-mapping-test)

# Native transport regression uses an ephemeral loopback listener on both hosts;
# no TWS/Gateway configuration is read and no financial message is transmitted.
add_executable(umicom-ibkr-native-adapter-test "${CMAKE_CURRENT_LIST_DIR}/test_native_adapter.c")
target_link_libraries(umicom-ibkr-native-adapter-test PRIVATE Umicom::ibkr_connection)
if(WIN32)
    target_link_libraries(umicom-ibkr-native-adapter-test PRIVATE ws2_32)
endif()
umicom_ibkr_target(umicom-ibkr-native-adapter-test)
add_test(NAME framework.ibkr_connection.native_nonblocking_adapter COMMAND umicom-ibkr-native-adapter-test)
set_tests_properties(framework.ibkr_connection.native_nonblocking_adapter PROPERTIES TIMEOUT 10)
