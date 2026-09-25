# Umicom Framework — financial operations regression cases.
# Author: Sammy Hegab, Umicom Foundation. Licence: MIT.
set(UMICOM_FINANCE_OPERATION_CASES
    empty
    format
    funding
    duplicate
    stale
    codec
    bounds
    maker_checker
    unbalanced
    cross_currency
    currency_scale
    period_overlap
    buying_power
    oversell
    price_overflow
    order_limit
    halt
    cash_reservation
    cancel_owner
    self_trade
    price_time
    resting_buy_price
    not_crossing
    journal_overflow
    reversal
    asof_report
    reversal_reserved
    partial_fill
    clear_before_settle
    settlement_checker
    settlement_date
    close_unfinished
    settlement
    settlement_duplicate
    settlement_reversal_blocked
    replay
    corrupt_event
    orphan_event
    missing_event
    reconciliation_break
    reconciliation_stale
    review_reopen
    close_checker
    close
    closed_reopen
    closed_trading
    audit
    input_integer input_date input_lines account_capacity order_capacity
    event_capacity codec_truncation utf8 reload_blocks_writes settlement_inventory_overflow)
set(UMICOM_FINANCE_OPERATION_SQLITE_CASES
    restart stale_writer write_failure read_failure settlement_rollback)
