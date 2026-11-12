#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomTradingExecutionQualification.cmake
# PURPOSE: Link execution regressions to the existing trading libraries.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
if(NOT BUILD_TESTING)
    return()
endif()
function(umicom_add_trading_execution_checks group source)
    set(target "umicom-trading-execution-${group}-test")
    add_executable("${target}" "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/trading_execution/${source}")
    target_link_libraries("${target}" PRIVATE Umicom::trading_ui)
    umicom_apply_warnings("${target}")
    umicom_apply_sanitizers("${target}")
    foreach(test_case IN LISTS ARGN)
        set(name "framework.trading_execution.${group}.${test_case}")
        add_test(NAME "${name}" COMMAND "${target}" "${test_case}")
        set_tests_properties("${name}" PROPERTIES TIMEOUT 30 LABELS "framework;trading;execution;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target("${target}")
    endif()
endfunction()
umicom_add_trading_execution_checks(regression test_regressions.c
    unpriced-market execution-duplicate realised-multiplier invalid-fill cancelled-fill order-duplicate quote-rollback)
umicom_add_trading_execution_checks(price test_price_evidence.c
    buy-sell freshness identity prices limits-stops policy-overflow economic-order
    workspace-refresh workspace-expiry workspace-gates workspace-policy workspace-metadata)
umicom_add_trading_execution_checks(fill test_fill_atomicity.c
    store-bounds position-arithmetic order-arithmetic workspace-replay workspace-invalid workspace-overflow)
umicom_add_trading_execution_checks(view test_evidence_views.c approved denial empty)
umicom_add_trading_execution_checks(clock test_simulation_clock.c delay backwards overflow)

umicom_add_trading_execution_checks(review test_order_review.c
    query selection stale-fill hidden identity output)

umicom_add_trading_execution_checks(chart test_chart_workspace.c
    navigation anchors coordinates drawings ticket isolation scene)

# Use the same order/position owners for direct checks and workspace publication.
umicom_add_trading_execution_checks(quantity test_fill_quantities.c
    tiny-partial tiny-complete tiny-overfill near-complete near-overfill remaining-overfill lost-increment large-complete subnormal-complete already-full beyond-full nan-stored infinite-report zero-report negative-report version-exhausted cancelled two-step workspace-partial workspace-overfill workspace-replay workspace-cancel position-long-increase position-long-reduce position-short-increase position-short-reduce)

# Include the real workspace owner in this fixture to observe revision faults
# without adding a mutable product API. Remaining owners come from the archive.
umicom_add_trading_execution_checks(cancel test_cancel_ownership.c
    accepted partial cancelled replay-at-max last-revision exhausted new validated filled rejected unknown null broker-simulation broker-paper workspace-accepted workspace-partial workspace-replay order-exhausted workspace-exhausted both-final cancelled-max filled-stability filter-retire missing)

# Exercise the final owner revision and bounded canonical lookup before publication.
umicom_add_trading_execution_checks(admission test_execution_admission.c
    new-exhausted existing-exhausted last-new last-existing replay-exhausted conflict-exhausted replay-cancelled replay-filled order-count order-text execution-count execution-text execution-full position-count position-full order-revision missing-order invalid-report)

# Reading a projection must not invent or wrap its owner identity.
umicom_add_trading_execution_checks(snapshot test_snapshot_revision.c
    empty zero maximum near-maximum partial cancelled flatten tape-only report-agreement repeat)

# Request enum validity does not grant a local matching capability.
umicom_add_trading_execution_checks(policy test_local_policy.c
    value-day value-gtc value-ioc value-fok value-negative value-unknown broker-simulation-day broker-simulation-gtc broker-simulation-ioc broker-simulation-fok broker-paper-day broker-paper-gtc broker-paper-ioc broker-paper-fok preview-day preview-gtc preview-ioc preview-fok timed-day timed-gtc timed-ioc timed-fok submit-day submit-gtc submit-ioc submit-fok preview-exhausted timed-exhausted submit-exhausted sequence-exhausted sequence-last unsupported-final)

# Exact UI value observations require no GUI launch.
umicom_add_trading_execution_checks(unsigned test_unsigned_revision.c
    value-zero value-one value-decimal value-u32 value-real-boundary value-real-loss value-signed-max value-unsigned-half value-near-max value-max null views-small views-boundary views-max)

# A refused ticket edit preserves the complete owner and any existing preview.
umicom_add_trading_execution_checks(draft test_draft_admission.c
    side-ordinary side-last side-exhausted side-repeated-exhausted side-invalid-exhausted type-ordinary type-last type-exhausted type-repeated-exhausted type-invalid-exhausted quantity-ordinary quantity-last quantity-exhausted quantity-repeated-exhausted quantity-invalid-exhausted prices-ordinary prices-last prices-exhausted prices-repeated-exhausted prices-invalid-exhausted policy-ordinary policy-last policy-exhausted policy-repeated-exhausted policy-invalid-exhausted reset-ordinary reset-last reset-exhausted reset-repeated-exhausted reset-invalid-exhausted)

# Emergency effects remain available; releasing protection needs owner capacity.
umicom_add_trading_execution_checks(protection test_protective_controls.c
    health-ordinary health-last health-exhausted kill-ordinary kill-last kill-exhausted disarm-ordinary disarm-last disarm-exhausted arm-ordinary arm-last arm-exhausted reset-ordinary reset-last reset-exhausted legacy-reset-ordinary legacy-reset-last legacy-reset-exhausted controller-reset-ordinary controller-reset-last controller-reset-exhausted health-recovery kill-repeat arm-invalid-health arm-invalid-environment null)

# Retained data and view state require capacity before mutation; readiness stays observable.
umicom_add_trading_execution_checks(tape test_tape_capacity.c
    append-ordinary append-last append-exhausted append-full append-stale append-invalid filter-ordinary filter-last filter-exhausted filter-noop filter-invalid pause-ordinary pause-last pause-exhausted pause-noop pause-resume remove-ordinary remove-last remove-exhausted remove-absent ready-ordinary ready-last ready-exhausted ready-noop workspace-filter-owner workspace-filter-child workspace-pause-owner workspace-pause-child workspace-ready-both)

# Observe both outer owner and nested data before and after capacity refusal.
umicom_add_trading_execution_checks(workspace test_workspace_admission.c
    add:ordinary add:last add:exhausted remove:ordinary remove:last remove:exhausted quote:ordinary quote:last quote:exhausted bar:ordinary bar:last bar:exhausted depth:ordinary depth:last depth:exhausted trade:ordinary trade:last trade:exhausted state:ordinary state:last state:exhausted filter:ordinary filter:last filter:exhausted study:ordinary study:last study:exhausted instrument:ordinary instrument:last instrument:exhausted order:ordinary order:last order:exhausted environment:ordinary environment:last environment:exhausted refresh:ordinary refresh:last refresh:exhausted alert-add:ordinary alert-add:last alert-add:exhausted alert-remove:ordinary alert-remove:last alert-remove:exhausted alert-disable:ordinary alert-disable:last alert-disable:exhausted alert-ack:ordinary alert-ack:last alert-ack:exhausted quote:market-last quote:market-exhausted bar:market-last bar:market-exhausted depth:market-last depth:market-exhausted trade:market-last trade:market-exhausted state:market-last state:market-exhausted)

# Validate bounded identity and active levels before lookup or publication.
umicom_add_trading_execution_checks(depth test_depth_validation.c
    valid-basic valid-empty valid-bid-only valid-ask-only valid-zero valid-negative valid-unordered valid-crossed valid-negative-time valid-inactive-tail valid-full bad-id-text bad-symbol-text bad-venue-text bad-currency-text bad-empty-id bad-empty-symbol bad-empty-venue bad-multiplier-zero bad-multiplier-nan bad-bid-count bad-ask-count bad-bid-price-nan bad-ask-price-nan bad-bid-price-inf bad-ask-price-inf bad-bid-size-nan bad-ask-size-nan bad-bid-size-inf bad-ask-size-inf bad-bid-size-negative bad-ask-size-negative meta-symbol meta-venue meta-currency meta-multiplier meta-expiry missing null)

# Prepare dependent alert changes before publishing workspace removals.
umicom_add_trading_execution_checks(alert test_alert_admission.c
    add:ordinary add:last add:exhausted add:corrupt remove:ordinary remove:last remove:exhausted remove:corrupt enable:ordinary enable:last enable:exhausted enable:corrupt ack:ordinary ack:last ack:exhausted ack:corrupt evaluate:ordinary evaluate:last evaluate:exhausted evaluate:corrupt batch:ordinary batch:last batch:exhausted batch:corrupt enable:noop batch:absent evaluate:absent evaluate:stale evaluate:occurrences add:count workspace:alert-capacity workspace:tape-capacity workspace:last workspace:allocation workspace:no-alerts workspace:no-tape workspace:quote-capacity workspace:quote-empty)

# Validate market contract ownership and every declared/unknown state pairing.
umicom_add_trading_execution_checks(market-event test_market_event_integrity.c
    bar:valid trade:valid bar:symbol trade:symbol bar:venue trade:venue bar:currency trade:currency bar:multiplier trade:multiplier bar:expiry trade:expiry bar:bad-id trade:bad-id bar:bad-symbol trade:bad-symbol bar:bad-venue trade:bad-venue bar:bad-currency trade:bad-currency bar:bad-multiplier trade:bad-multiplier bar:missing trade:missing bar:null trade:null bar:metadata-at-capacity trade:metadata-at-capacity tick:ordinary tick:zero-size tick:zero-time tick:maximum-finite tick:price-nan tick:price-infinity tick:price-zero tick:price-negative tick:size-nan tick:size-infinity tick:size-negative tick:negative-time tick:null state:negative:negative state:negative:closed state:negative:preopen state:negative:open state:negative:halted state:negative:above state:negative:maximum state:closed:negative state:closed:closed state:closed:preopen state:closed:open state:closed:halted state:closed:above state:closed:maximum state:preopen:negative state:preopen:closed state:preopen:preopen state:preopen:open state:preopen:halted state:preopen:above state:preopen:maximum state:open:negative state:open:closed state:open:preopen state:open:open state:open:halted state:open:above state:open:maximum state:halted:negative state:halted:closed state:halted:preopen state:halted:open state:halted:halted state:halted:above state:halted:maximum state:above:negative state:above:closed state:above:preopen state:above:open state:above:halted state:above:above state:above:maximum state:maximum:negative state:maximum:closed state:maximum:preopen state:maximum:open state:maximum:halted state:maximum:above state:maximum:maximum bar:replace bar:older bar:negative-price state:corrupt-owner)
