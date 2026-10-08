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

# The fixture drives the production socket framing and quote decoder without
# contacting a broker or asserting that external data permissions are present.
add_executable(umicom-ibkr-quote-test "${CMAKE_CURRENT_LIST_DIR}/test_quotes.c")
target_include_directories(umicom-ibkr-quote-test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../../src/ibkr_connection")
target_link_libraries(umicom-ibkr-quote-test PRIVATE Umicom::ibkr_connection)
umicom_ibkr_target(umicom-ibkr-quote-test)
foreach(_case IN ITEMS wire values freshness sentinels frozen delayed unknown-type malformed capacity queue-failure cancel-reuse refusal invalid field-versions)
    add_test(NAME framework.ibkr_connection.quote.${_case} COMMAND umicom-ibkr-quote-test ${_case})
    set_tests_properties(framework.ibkr_connection.quote.${_case} PROPERTIES
        TIMEOUT 20 LABELS "framework;ibkr;market-data;read-only;regression")
endforeach()

if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-ibkr-quote-test)
endif()

# Reports use invented snapshots and owned temporary files; no socket opens.
add_executable(umicom-ibkr-observation-export-test "${CMAKE_CURRENT_LIST_DIR}/test_observation_export.c")
target_link_libraries(umicom-ibkr-observation-export-test PRIVATE Umicom::ibkr_connection Umicom::platform)
umicom_ibkr_target(umicom-ibkr-observation-export-test)
foreach(case account-limit attested backwards-capture disconnected duplicate-account early-position empty formula future-summary independent invalid-utf8 live-label maximum no-request not-ready partial position-limit quoted simulation unicode unlisted unterminated valid value-limit writable write-cancel write-existing write-new)
    add_test(NAME framework.ibkr_observations.${case} COMMAND umicom-ibkr-observation-export-test ${case})
    set_tests_properties(framework.ibkr_observations.${case} PROPERTIES TIMEOUT 30 LABELS "framework;broker;csv;regression")
endforeach()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-ibkr-observation-export-test)
endif()

# Position choice tests use only invented snapshots, without sockets or orders.
add_executable(umicom-ibkr-position-review-test "${CMAKE_CURRENT_LIST_DIR}/test_position_review.c")
target_link_libraries(umicom-ibkr-position-review-test PRIVATE Umicom::ibkr_connection)
umicom_ibkr_target(umicom-ibkr-position-review-test)
foreach(case valid no-request unlisted duplicate-account unterminated too-many partial disconnected attested writable empty bad-id id-overflow no-exchange combination exchange-control changed-row owned changed-request changed-completion changed-account changed-environment stale)
    add_test(NAME framework.ibkr_positions.review.${case} COMMAND umicom-ibkr-position-review-test ${case})
    set_tests_properties(framework.ibkr_positions.review.${case} PROPERTIES TIMEOUT 20 LABELS "framework;broker;positions;regression")
endforeach()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-ibkr-position-review-test)
endif()

# Contract lookups exercise byte framing through injected I/O only.
add_executable(umicom-ibkr-contract-details-test "${CMAKE_CURRENT_LIST_DIR}/test_contract_details.c")
target_include_directories(umicom-ibkr-contract-details-test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../../src/ibkr_connection")
target_link_libraries(umicom-ibkr-contract-details-test PRIVATE Umicom::ibkr_connection)
umicom_ibkr_target(umicom-ibkr-contract-details-test)
foreach(_case wire valid old-protocol invalid queue-full shared-identity busy timeout refusal abandon
    late-error bond foreign malformed numeric invalid-utf8 wrong-contract long-name duplicate disconnected)
    add_test(NAME framework.ibkr_contract_details.${_case} COMMAND umicom-ibkr-contract-details-test ${_case})
    set_tests_properties(framework.ibkr_contract_details.${_case} PROPERTIES TIMEOUT 20
        LABELS "framework;ibkr;contract;read-only;regression")
endforeach()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-ibkr-contract-details-test)
endif()

# Pairing and price-rule transport checks use invented metadata only.
foreach(group contract_route market_rule)
    add_executable(umicom-ibkr-${group}-test "${CMAKE_CURRENT_LIST_DIR}/test_${group}.c")
    target_include_directories(umicom-ibkr-${group}-test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../../src/ibkr_connection")
    target_link_libraries(umicom-ibkr-${group}-test PRIVATE Umicom::ibkr_connection)
    umicom_ibkr_target(umicom-ibkr-${group}-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-ibkr-${group}-test)
    endif()
endforeach()
set(contract_route_cases valid partial stale failed unknown case-sensitive empty zero unequal duplicate interior-empty overflow unterminated)
set(market_rule_cases wire valid invalid queue-full busy capacity timeout foreign empty bad-count bad-order bad-increment precision scientific duplicate disconnected)
foreach(group contract_route market_rule)
    foreach(case IN LISTS ${group}_cases)
        add_test(NAME framework.ibkr_${group}.${case} COMMAND umicom-ibkr-${group}-test ${case})
        set_tests_properties(framework.ibkr_${group}.${case} PROPERTIES TIMEOUT 20
            LABELS "framework;broker;price-increment;read-only;regression")
    endforeach()
endforeach()

# Captures and comparisons use invented reports; these checks never open TWS.
foreach(group execution_capture execution_review)
    add_executable(umicom-ibkr-${group}-test "${CMAKE_CURRENT_LIST_DIR}/test_${group}.c")
    target_include_directories(umicom-ibkr-${group}-test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../../src/ibkr_connection")
    target_link_libraries(umicom-ibkr-${group}-test PRIVATE Umicom::ibkr_connection)
    umicom_ibkr_target(umicom-ibkr-${group}-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-ibkr-${group}-test)
    endif()
endforeach()
set(execution_capture_cases wire valid duplicate correction unlisted queue-full unsupported busy shared-identity timeout abandon late-request refusal disconnected empty foreign unsolicited invalid-utf8 negative precision bad-side bad-count capacity duplicate-conflict)
set(execution_review_cases valid partial stale failed foreign-account correction combo-id asset negative-price below above scale overflow duplicate bad-count zero-permanent other-order missing unterminated)

foreach(group execution_capture execution_review)
    foreach(case IN LISTS ${group}_cases)
        add_test(NAME framework.ibkr_${group}.${case} COMMAND umicom-ibkr-${group}-test ${case})
        set_tests_properties(framework.ibkr_${group}.${case} PROPERTIES
            TIMEOUT 20 LABELS "framework;broker;executions;read-only;regression")
    endforeach()
endforeach()

# Fees have separate delivery and no request ID; missing values are not zero.
add_executable(umicom-ibkr-execution-commission-test "${CMAKE_CURRENT_LIST_DIR}/test_execution_commission.c")
target_include_directories(umicom-ibkr-execution-commission-test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../../src/ibkr_connection")
target_link_libraries(umicom-ibkr-execution-commission-test PRIVATE Umicom::ibkr_connection)
umicom_ibkr_target(umicom-ibkr-execution-commission-test)
foreach(case valid after-end duplicate unknown rebate expired early missing precision unrepresentable mixed-currency conflicting changed-currency corrupt-value corrupt-currency overflow)
    add_test(NAME framework.ibkr_execution_commission.${case} COMMAND umicom-ibkr-execution-commission-test ${case})
    set_tests_properties(framework.ibkr_execution_commission.${case} PROPERTIES
        TIMEOUT 20 LABELS "framework;broker;executions;commission;read-only;regression")
endforeach()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-ibkr-execution-commission-test)
endif()

# Observation protocol checks use an injected transport and invented account data.
foreach(group pnl symbol_search)
    add_executable(umicom-ibkr-${group}-test "${CMAKE_CURRENT_LIST_DIR}/test_${group}.c")
    target_include_directories(umicom-ibkr-${group}-test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../../src/ibkr_connection")
    target_link_libraries(umicom-ibkr-${group}-test PRIVATE Umicom::ibkr_connection)
    umicom_ibkr_target(umicom-ibkr-${group}-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-ibkr-${group}-test)
    endif()
endforeach()
set(pnl_cases wire single-wire single-cancel-wire cancel-wire cancel-full unlisted all-account
    invalid-text unterminated contract-range unsupported not-ready queue-full id-exhausted backward
    duplicate different-scope capacity shared-identity alias-reuse waiting refusal cancel-late revision-exhausted
    single valid unset empty precision scientific malformed nan invalid-utf8 wrong-kind missing-field foreign age disconnect)
set(symbol_search_cases wire empty spaces control invalid-pattern long-pattern queue-full unsupported backward
    id-exhausted not-ready legacy pending paced timeout alias-pattern refusal abandon late-request late-error
    no-results foreign bad-id bad-count bad-derivatives invalid-utf8 empty-symbol long-description truncated
    valid duplicate disconnect)
foreach(group pnl symbol_search)
    foreach(case IN LISTS ${group}_cases)
        add_test(NAME framework.ibkr_${group}.${case} COMMAND umicom-ibkr-${group}-test ${case})
        set_tests_properties(framework.ibkr_${group}.${case} PROPERTIES TIMEOUT 20
            LABELS "framework;broker;observations;read-only;regression")
    endforeach()
endforeach()

# Positional book tests include broker reset 317 and keep all traffic in a fake transport.
add_executable(umicom-ibkr-depth-test "${CMAKE_CURRENT_LIST_DIR}/test_market_depth.c")
target_include_directories(umicom-ibkr-depth-test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../../src/ibkr_connection")
target_link_libraries(umicom-ibkr-depth-test PRIVATE Umicom::ibkr_connection)
umicom_ibkr_target(umicom-ibkr-depth-test)
set(depth_cases zero-id zero-rows large-rows missing-exchange route-mismatch combo bad-strike bad-multiplier
    bad-right unterminated invalid-utf8 queue-full not-ready unsupported id-exhausted descriptors wire smart-wire
    duplicate capacity cancel-wire smart-cancel cancel-full alias-reuse waiting cancel-late revision-exhausted
    negative-price precision unset scientific negative-size gap update-empty bad-side wrong-smart maker smart foreign
    reset halt reset-exhausted disconnect row-age full-insert update delete bad-delete)
foreach(case IN LISTS depth_cases)
    add_test(NAME framework.ibkr_depth.${case} COMMAND umicom-ibkr-depth-test ${case})
    set_tests_properties(framework.ibkr_depth.${case} PROPERTIES TIMEOUT 20
        LABELS "framework;broker;depth;read-only;regression")
endforeach()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-ibkr-depth-test)
endif()

add_executable(umicom-ibkr-observation-session-test "${CMAKE_CURRENT_LIST_DIR}/test_observation_session.c")
target_include_directories(umicom-ibkr-observation-session-test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../../src/ibkr_connection")
target_link_libraries(umicom-ibkr-observation-session-test PRIVATE Umicom::ibkr_connection)
umicom_ibkr_target(umicom-ibkr-observation-session-test)
foreach(case interleaved no-orders cancel-one fragmented-read fragmented-write depth-reset replaced-search disconnect)
    add_test(NAME framework.ibkr_observation_session.${case} COMMAND umicom-ibkr-observation-session-test ${case})
    set_tests_properties(framework.ibkr_observation_session.${case} PROPERTIES TIMEOUT 20
        LABELS "framework;broker;observations;integration;read-only;regression")
endforeach()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-ibkr-observation-session-test)
endif()

# Order recovery is exercised only through an injected local transport.
set(order_recovery_cases not-ready invalid-scope backward queue-full unsolicited-end wire all-wire busy scope-change timeout late-end own-client all-clients malformed-end empty no-repeat age disconnect duplicate-end)
set(order_status_cases valid duplicate duplicate-overflow correction filled inactive unknown-state precision scientific revision-overflow other-client negative-id permanent-upgrade conflicting-permanent status-before-open status-age capacity invalid-copy negative-fill nan bad-client bad-id bad-permanent missing-field invalid-utf8 unset negative-price unbound unbound-missing)
set(open_order_fields_cases valid unicode negative-price unset precision bad-quantity bad-contract bad-client bad-utf8 bad-price raw-field raw-short raw-query raw-empty raw-invalid disconnect duplicate replace truncated)
set(order_identity_cases unconnected ready lower higher observed negative exhausted other-client disconnect bad-next no-mutations execution-floor)
set(order_session_cases fragmented-read fragmented-write interleaved disconnect late-status)

foreach(group order_recovery order_status open_order_fields order_identity order_session)
    add_executable(umicom-ibkr-${group}-test "${CMAKE_CURRENT_LIST_DIR}/test_${group}.c")
    target_include_directories(umicom-ibkr-${group}-test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../../src/ibkr_connection")
    target_link_libraries(umicom-ibkr-${group}-test PRIVATE Umicom::ibkr_connection)
    umicom_ibkr_target(umicom-ibkr-${group}-test)
    foreach(case IN LISTS ${group}_cases)
        add_test(NAME framework.ibkr_${group}.${case} COMMAND umicom-ibkr-${group}-test ${case})
        set_tests_properties(framework.ibkr_${group}.${case} PROPERTIES TIMEOUT 20
            LABELS "framework;broker;orders;read-only;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-ibkr-${group}-test)
    endif()
endforeach()

# Completed history stays isolated from current-order and execution capture ownership.
set(completed_capture_cases not-ready backward unsupported queue-full unsolicited no-mutations wire all-wire fragmented-write busy timeout late-end malformed-end empty age disconnect duplicate-end late-row no-repeat invalid-copy fragmented-read)
set(completed_layout_cases valid legacy peg-threshold precision scientific negative-price scale-unset unicode truncated extra-field delta-order combo leg-prices smart-tags scale hedge delta-contract algorithm peg-benchmark condition-price condition-time condition-margin condition-execution condition-volume condition-percent condition-unknown condition-connector negative-quantity nan-price bad-flag bad-permanent bad-contract bad-utf8 missing-account negative-filled combo-limit tag-limit condition-limit scale-inexact)
set(completed_store_cases duplicate duplicate-overflow conflict capacity age disconnect raw raw-short raw-query raw-invalid raw-empty invalid-copy)
set(completed_review_cases cancelled-partial below above missing-fee conflicting-fee mixed-fees correction account zero precision unsupported contract side currency completed-pending execution-pending wrong-request age disconnect)

foreach(group completed_capture completed_layout completed_store completed_review)
    add_executable(umicom-ibkr-${group}-test "${CMAKE_CURRENT_LIST_DIR}/test_${group}.c")
    target_include_directories(umicom-ibkr-${group}-test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../../src/ibkr_connection")
    target_link_libraries(umicom-ibkr-${group}-test PRIVATE Umicom::ibkr_connection)
    umicom_ibkr_target(umicom-ibkr-${group}-test)
    foreach(case IN LISTS ${group}_cases)
        add_test(NAME framework.ibkr_${group}.${case} COMMAND umicom-ibkr-${group}-test ${case})
        set_tests_properties(framework.ibkr_${group}.${case} PROPERTIES TIMEOUT 20
            LABELS "framework;broker;completed-orders;read-only;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-ibkr-${group}-test)
    endif()
endforeach()

set(completed_report_cases valid no-request null backward zero-age empty partial timeout age disconnect
    all-scope precision unicode quoted formula owned)
add_executable(umicom-ibkr-completed_report-test "${CMAKE_CURRENT_LIST_DIR}/test_completed_report.c")
target_include_directories(umicom-ibkr-completed_report-test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../../src/ibkr_connection")
target_link_libraries(umicom-ibkr-completed_report-test PRIVATE Umicom::ibkr_connection)
umicom_ibkr_target(umicom-ibkr-completed_report-test)
foreach(case IN LISTS completed_report_cases)
    add_test(NAME framework.ibkr_completed_report.${case} COMMAND umicom-ibkr-completed_report-test ${case})
    set_tests_properties(framework.ibkr_completed_report.${case} PROPERTIES TIMEOUT 20
        LABELS "framework;broker;completed-orders;csv;regression")
endforeach()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-ibkr-completed_report-test)
endif()

# Finite historical data stays independent of quote and order ownership.
set(historical_query_cases valid sizes kinds null contract-zero contract-overflow route-empty route-long duration-zero duration-long too-many-bars size kind utc leap invalid-leap invalid-month invalid-day invalid-hour invalid-minute invalid-second local-zone end-long)
set(historical_capture_cases disconnect-pending not-ready backward unsupported queue-full no-mutations wire fragmented-write busy cancel cancel-full late-cancel pacing replace replace-full timeout late-timeout provider-error empty age disconnect invalid-copy fragmented-read request-overflow old-request)
set(historical_decode_cases valid negative-price scientific precision missing-volume midpoint nan bad-time time-overflow high-below-close low-above-open negative-volume bad-count missing-field extra-field bad-range duplicate-time reversed-time foreign unsolicited duplicate capacity)
foreach(group historical_query historical_capture historical_decode)
    add_executable(umicom-ibkr-${group}-test "${CMAKE_CURRENT_LIST_DIR}/test_${group}.c")
    target_include_directories(umicom-ibkr-${group}-test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../../src/ibkr_connection")
    target_link_libraries(umicom-ibkr-${group}-test PRIVATE Umicom::ibkr_connection)
    umicom_ibkr_target(umicom-ibkr-${group}-test)
    foreach(case IN LISTS ${group}_cases)
        add_test(NAME framework.ibkr_${group}.${case} COMMAND umicom-ibkr-${group}-test ${case})
        set_tests_properties(framework.ibkr_${group}.${case} PROPERTIES TIMEOUT 20
            LABELS "framework;broker;historical-data;chart;read-only;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-ibkr-${group}-test)
    endif()
endforeach()

set(historical_report_cases valid no-request null backward zero-age pending cancelled timeout failed age disconnect empty precision missing-volume quoted formula owned metadata-every-row)
add_executable(umicom-ibkr-historical_report-test "${CMAKE_CURRENT_LIST_DIR}/test_historical_report.c")
target_include_directories(umicom-ibkr-historical_report-test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../../src/ibkr_connection")
target_link_libraries(umicom-ibkr-historical_report-test PRIVATE Umicom::ibkr_connection)
umicom_ibkr_target(umicom-ibkr-historical_report-test)
foreach(case IN LISTS historical_report_cases)
    add_test(NAME framework.ibkr_historical_report.${case} COMMAND umicom-ibkr-historical_report-test ${case})
    set_tests_properties(framework.ibkr_historical_report.${case} PROPERTIES TIMEOUT 20
        LABELS "framework;broker;historical-data;csv;regression")
endforeach()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-ibkr-historical_report-test)
endif()

# Streaming bar coverage uses only the injected transport.
set(realtime_query_cases valid kinds null zero-contract overflow-contract empty-route long-route control-route bad-kind)
set(realtime_subscription_cases not-ready backward unsupported null-output wire midpoint-wire outside-hours fragmented-write no-mutations queue-full request-overflow cancel cancel-full late-cancel pacing shared-history shared-stream duplicate capacity reuse old-request failed-cancel close-before-first)
set(realtime_decode_cases valid negative-price scientific precision missing-volume midpoint bid ask nan bad-time time-overflow high-below-close low-above-open bad-volume bad-count missing-field extra-field foreign bad-version fragmented-read)
set(realtime_window_cases rolling gap duplicate duplicate-stale conflict out-of-order received-overflow duplicate-overflow gap-overflow age-boundary age no-bars cancel-retained disconnect-retained bust provider-error reset independent invalid-copy invalid-bar)
foreach(group realtime_query realtime_subscription realtime_decode realtime_window)
    add_executable(umicom-ibkr-${group}-test "${CMAKE_CURRENT_LIST_DIR}/test_${group}.c")
    target_include_directories(umicom-ibkr-${group}-test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../../src/ibkr_connection")
    target_link_libraries(umicom-ibkr-${group}-test PRIVATE Umicom::ibkr_connection)
    umicom_ibkr_target(umicom-ibkr-${group}-test)
    foreach(case IN LISTS ${group}_cases)
        add_test(NAME framework.ibkr_${group}.${case} COMMAND umicom-ibkr-${group}-test ${case})
        set_tests_properties(framework.ibkr_${group}.${case} PROPERTIES TIMEOUT 20
            LABELS "framework;broker;streaming-bars;read-only;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-ibkr-${group}-test)
    endif()
endforeach()

set(realtime_report_cases valid pending cancelled failed age disconnect precision missing-volume quoted formula owned frozen metadata-every-row retention no-request null zero-age backward)

add_executable(umicom-ibkr-realtime_report-test "${CMAKE_CURRENT_LIST_DIR}/test_realtime_report.c")
target_include_directories(umicom-ibkr-realtime_report-test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../../src/ibkr_connection")
target_link_libraries(umicom-ibkr-realtime_report-test PRIVATE Umicom::ibkr_connection)
umicom_ibkr_target(umicom-ibkr-realtime_report-test)
foreach(case IN LISTS realtime_report_cases)
    add_test(NAME framework.ibkr_realtime_report.${case} COMMAND umicom-ibkr-realtime_report-test ${case})
    set_tests_properties(framework.ibkr_realtime_report.${case} PROPERTIES TIMEOUT 20
        LABELS "framework;broker;streaming-bars;csv;regression")
endforeach()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-ibkr-realtime_report-test)
endif()

# Discovery regression cases use injected I/O and invented instruments.
set(scanner_query_cases valid null rows-zero rows-overflow instrument location scan unterminated control negative-price price-range cap-range coupon-range precision nan fraction-volume volume-overflow leap bad-leap date-range filter-count filter-injection filter-value filter-duplicate filter-capacity)
set(scanner_subscription_cases not-ready backward null-output unsupported queue-full overflow wire filters fragmented pacing capacity cancel reuse cancel-full disconnect no-mutations)
set(discovery_identity_cases history-first stream-first)
set(scanner_decode_cases valid empty bad-version count capacity rank contract symbol utf8 type number precision exchange control short extra foreign fragmented refresh generation cancelled provider-error age atomic generation-overflow)
set(option_chain_cases wire not-ready invalid-contract invalid-symbol invalid-exchange unsupported backward queue-full overflow busy abandon late-abandon timeout late-timeout empty disconnect provider-error partial complete underlying exchange class utf8 expiries date duplicate-expiry strikes number precision negative duplicate-strike short extra duplicate-chain foreign fragmented reuse)
set(option_contract_cases wire put future pending-chain abandoned chain-index expiry-index strike-index invalid-right empty-currency precision unsupported queue-full busy complete empty ambiguous symbol type expiry strike right currency class multiplier)
foreach(group scanner_query scanner_subscription discovery_identity scanner_decode option_chain option_contract)
    add_executable(umicom-ibkr-${group}-test "${CMAKE_CURRENT_LIST_DIR}/test_${group}.c")
    target_include_directories(umicom-ibkr-${group}-test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../../src/ibkr_connection")
    target_link_libraries(umicom-ibkr-${group}-test PRIVATE Umicom::ibkr_connection)
    umicom_ibkr_target(umicom-ibkr-${group}-test)
    foreach(case IN LISTS ${group}_cases)
        add_test(NAME framework.ibkr_${group}.${case} COMMAND umicom-ibkr-${group}-test ${case})
        set_tests_properties(framework.ibkr_${group}.${case} PROPERTIES TIMEOUT 20
            LABELS "framework;broker;discovery;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-ibkr-${group}-test)
    endif()
endforeach()

set(discovery_report_cases scanner scanner-pending scanner-cancelled scanner-owned scanner-frozen scanner-filter scanner-formula scanner-quoted scanner-age option option-partial option-owned option-abandoned option-disconnect option-precision option-separate-sets invalid no-request)

add_executable(umicom-ibkr-discovery_report-test "${CMAKE_CURRENT_LIST_DIR}/test_discovery_report.c")
target_include_directories(umicom-ibkr-discovery_report-test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../../src/ibkr_connection")
target_link_libraries(umicom-ibkr-discovery_report-test PRIVATE Umicom::ibkr_connection)
umicom_ibkr_target(umicom-ibkr-discovery_report-test)
foreach(case IN LISTS discovery_report_cases)
    add_test(NAME framework.ibkr_discovery_report.${case} COMMAND umicom-ibkr-discovery_report-test ${case})
    set_tests_properties(framework.ibkr_discovery_report.${case} PROPERTIES TIMEOUT 20 LABELS "framework;broker;discovery;csv;regression")
endforeach()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-ibkr-discovery_report-test)
endif()

# Large catalogue responses use fake transport, never a broker session.
set(scanner_catalog_cases wire not-ready null backward protocol queue-full pending repeat valid whitespace unicode inert empty version extra utf8 control duplicate unsolicited timeout late-timeout close-before close-after copy-small copy-null retained)
set(catalog_transport_cases large fragmented trailing-frame wrong-message no-request too-large zero-length close-partial timeout-partial frame-overflow read-error)

foreach(group scanner_catalog catalog_transport)
    add_executable(umicom-ibkr-${group}-test "${CMAKE_CURRENT_LIST_DIR}/test_${group}.c")
    target_include_directories(umicom-ibkr-${group}-test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../../src/ibkr_connection")
    target_link_libraries(umicom-ibkr-${group}-test PRIVATE Umicom::ibkr_connection)
    umicom_ibkr_target(umicom-ibkr-${group}-test)
    foreach(case IN LISTS ${group}_cases)
        add_test(NAME framework.ibkr_${group}.${case} COMMAND umicom-ibkr-${group}-test ${case})
        set_tests_properties(framework.ibkr_${group}.${case} PROPERTIES TIMEOUT 20 LABELS "framework;broker;catalogue;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-ibkr-${group}-test)
    endif()
endforeach()
