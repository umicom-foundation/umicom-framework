#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomOpenTmsPlatform.cmake
#
# PURPOSE:
#   Complete the Framework-owned Open TMS platform for FX forwards, cash and
#   liquidity, pricing, positions, risk, settlement, accounting,
#   reconciliation and treasury operational dashboards.
#
# ARCHITECTURE:
#   Existing Umicom::finance treasury, valuation, settlement, accounting and
#   shared financial-core services remain authoritative. This layer composes
#   them for TMS and does not move reusable financial logic into the product.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

if(NOT TARGET umicom_finance)
    message(FATAL_ERROR "UmicomOpenTmsPlatform requires canonical umicom_finance")
endif()

target_sources(umicom_finance PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/accounting/accrual.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/accounting/double_entry.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/accounting/event.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/accounting/pnl.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/accounting/posting.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/accounting/reconciliation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/accounting_book.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/accounting_review.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/accounting_archive.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/accounting_report.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/cash/account_coverage.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/cash/currency_coverage.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/cash/forecast_net.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/cash/freshness.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/cash/minimum_balance.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/cash/projected_balance.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/cash/settled_balance.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/cash/shortfall.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/cash/surplus.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/cash/sweep_readiness.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/cash_book.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/dashboard/cash_health.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/dashboard/liquidity_health.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/dashboard/operations_health.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/dashboard/risk_health.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/dashboard/settlement_health.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/dashboard_service.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/fx_forward/audit.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/fx_forward/confirmation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/fx_forward/forward_points.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/fx_forward/forward_rate.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/fx_forward/lifecycle.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/fx_forward/maturity.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/fx_forward/mtm.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/fx_forward/notional.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/fx_forward/settlement.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/fx_forward/trade_readiness.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/fx_forward_book.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/fx_forward_review.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/fx_forward_archive.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/liquidity/bucket_coverage.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/liquidity/buffer.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/liquidity/concentration.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/liquidity/coverage.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/liquidity/funding_capacity.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/liquidity/funding_requirement.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/liquidity/gap.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/liquidity/intraday.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/liquidity/readiness.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/liquidity/stress.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/liquidity_ladder.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/open_tms_internal.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/position/aggregation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/position/concentration.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/position/currency_exposure.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/position/gross_exposure.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/position/mtm.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/position/net_exposure.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/position/readiness.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/position/trade_coverage.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/position_book.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/pricing/accrual.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/pricing/curve_coverage.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/pricing/discount_factor.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/pricing/fixing_coverage.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/pricing/forward_rate.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/pricing/market_data.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/pricing/present_value.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/pricing/readiness.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/pricing/spot.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/pricing/valuation_quality.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/pricing_engine.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/reconciliation/breaks.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/reconciliation/cash.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/reconciliation/completion.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/reconciliation/settlement.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/reconciliation/trade.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/reconciliation_book.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/risk/breach.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/risk/counterparty.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/risk/credit.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/risk/limit_utilisation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/risk/liquidity.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/risk/market.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/risk/readiness.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/risk/stress.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/risk_book.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/service.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/settlement/confirmation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/settlement/failure.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/settlement/netting.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/settlement/obligation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/settlement/queue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/settlement/readiness.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/settlement/repair.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/settlement/window.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/settlement_book.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/open_tms/types.c"
)

if(BUILD_TESTING)
    function(umicom_add_open_tms_test target test_name source)
        if(TARGET "${target}")
            return()
        endif()
        add_executable(
            "${target}"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/open_tms/${source}"
        )
        target_link_libraries("${target}" PRIVATE Umicom::finance)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings("${target}")
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers("${target}")
        endif()
        add_test(NAME "${test_name}" COMMAND "${target}")
        set_tests_properties(
            "${test_name}"
            PROPERTIES LABELS
            "framework;open-tms;treasury;liquidity;risk;settlement;accounting"
        )
    endfunction()

    umicom_add_open_tms_test(
        umicom-open-tms-accounting-accrual-test
        framework.open_tms.accounting.accrual
        test_accounting_accrual.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-accounting-book-test
        framework.open_tms.accounting.book
        test_accounting_book.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-accounting-double-entry-test
        framework.open_tms.accounting.double.entry
        test_accounting_double_entry.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-accounting-event-test
        framework.open_tms.accounting.event
        test_accounting_event.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-accounting-pnl-test
        framework.open_tms.accounting.pnl
        test_accounting_pnl.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-accounting-posting-test
        framework.open_tms.accounting.posting
        test_accounting_posting.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-accounting-reconciliation-test
        framework.open_tms.accounting.reconciliation
        test_accounting_reconciliation.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-cash-account-coverage-test
        framework.open_tms.cash.account.coverage
        test_cash_account_coverage.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-cash-book-test
        framework.open_tms.cash.book
        test_cash_book.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-cash-currency-coverage-test
        framework.open_tms.cash.currency.coverage
        test_cash_currency_coverage.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-cash-forecast-net-test
        framework.open_tms.cash.forecast.net
        test_cash_forecast_net.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-cash-freshness-test
        framework.open_tms.cash.freshness
        test_cash_freshness.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-cash-minimum-balance-test
        framework.open_tms.cash.minimum.balance
        test_cash_minimum_balance.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-cash-projected-balance-test
        framework.open_tms.cash.projected.balance
        test_cash_projected_balance.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-cash-settled-balance-test
        framework.open_tms.cash.settled.balance
        test_cash_settled_balance.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-cash-shortfall-test
        framework.open_tms.cash.shortfall
        test_cash_shortfall.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-cash-surplus-test
        framework.open_tms.cash.surplus
        test_cash_surplus.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-cash-sweep-readiness-test
        framework.open_tms.cash.sweep.readiness
        test_cash_sweep_readiness.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-dashboard-cash-health-test
        framework.open_tms.dashboard.cash.health
        test_dashboard_cash_health.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-dashboard-liquidity-health-test
        framework.open_tms.dashboard.liquidity.health
        test_dashboard_liquidity_health.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-dashboard-operations-health-test
        framework.open_tms.dashboard.operations.health
        test_dashboard_operations_health.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-dashboard-risk-health-test
        framework.open_tms.dashboard.risk.health
        test_dashboard_risk_health.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-dashboard-service-test
        framework.open_tms.dashboard.service
        test_dashboard_service.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-dashboard-settlement-health-test
        framework.open_tms.dashboard.settlement.health
        test_dashboard_settlement_health.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-fx-forward-audit-test
        framework.open_tms.fx.forward.audit
        test_fx_forward_audit.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-fx-forward-book-test
        framework.open_tms.fx.forward.book
        test_fx_forward_book.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-fx-forward-confirmation-test
        framework.open_tms.fx.forward.confirmation
        test_fx_forward_confirmation.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-fx-forward-forward-points-test
        framework.open_tms.fx.forward.forward.points
        test_fx_forward_forward_points.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-fx-forward-forward-rate-test
        framework.open_tms.fx.forward.forward.rate
        test_fx_forward_forward_rate.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-fx-forward-lifecycle-test
        framework.open_tms.fx.forward.lifecycle
        test_fx_forward_lifecycle.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-fx-forward-maturity-test
        framework.open_tms.fx.forward.maturity
        test_fx_forward_maturity.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-fx-forward-mtm-test
        framework.open_tms.fx.forward.mtm
        test_fx_forward_mtm.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-fx-forward-notional-test
        framework.open_tms.fx.forward.notional
        test_fx_forward_notional.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-fx-forward-settlement-test
        framework.open_tms.fx.forward.settlement
        test_fx_forward_settlement.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-fx-forward-trade-readiness-test
        framework.open_tms.fx.forward.trade.readiness
        test_fx_forward_trade_readiness.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-liquidity-bucket-coverage-test
        framework.open_tms.liquidity.bucket.coverage
        test_liquidity_bucket_coverage.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-liquidity-buffer-test
        framework.open_tms.liquidity.buffer
        test_liquidity_buffer.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-liquidity-concentration-test
        framework.open_tms.liquidity.concentration
        test_liquidity_concentration.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-liquidity-coverage-test
        framework.open_tms.liquidity.coverage
        test_liquidity_coverage.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-liquidity-funding-capacity-test
        framework.open_tms.liquidity.funding.capacity
        test_liquidity_funding_capacity.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-liquidity-funding-requirement-test
        framework.open_tms.liquidity.funding.requirement
        test_liquidity_funding_requirement.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-liquidity-gap-test
        framework.open_tms.liquidity.gap
        test_liquidity_gap.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-liquidity-intraday-test
        framework.open_tms.liquidity.intraday
        test_liquidity_intraday.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-liquidity-ladder-test
        framework.open_tms.liquidity.ladder
        test_liquidity_ladder.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-liquidity-readiness-test
        framework.open_tms.liquidity.readiness
        test_liquidity_readiness.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-liquidity-stress-test
        framework.open_tms.liquidity.stress
        test_liquidity_stress.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-position-aggregation-test
        framework.open_tms.position.aggregation
        test_position_aggregation.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-position-book-test
        framework.open_tms.position.book
        test_position_book.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-position-concentration-test
        framework.open_tms.position.concentration
        test_position_concentration.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-position-currency-exposure-test
        framework.open_tms.position.currency.exposure
        test_position_currency_exposure.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-position-gross-exposure-test
        framework.open_tms.position.gross.exposure
        test_position_gross_exposure.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-position-mtm-test
        framework.open_tms.position.mtm
        test_position_mtm.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-position-net-exposure-test
        framework.open_tms.position.net.exposure
        test_position_net_exposure.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-position-readiness-test
        framework.open_tms.position.readiness
        test_position_readiness.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-position-trade-coverage-test
        framework.open_tms.position.trade.coverage
        test_position_trade_coverage.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-pricing-accrual-test
        framework.open_tms.pricing.accrual
        test_pricing_accrual.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-pricing-curve-coverage-test
        framework.open_tms.pricing.curve.coverage
        test_pricing_curve_coverage.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-pricing-discount-factor-test
        framework.open_tms.pricing.discount.factor
        test_pricing_discount_factor.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-pricing-engine-test
        framework.open_tms.pricing.engine
        test_pricing_engine.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-pricing-fixing-coverage-test
        framework.open_tms.pricing.fixing.coverage
        test_pricing_fixing_coverage.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-pricing-forward-rate-test
        framework.open_tms.pricing.forward.rate
        test_pricing_forward_rate.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-pricing-market-data-test
        framework.open_tms.pricing.market.data
        test_pricing_market_data.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-pricing-present-value-test
        framework.open_tms.pricing.present.value
        test_pricing_present_value.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-pricing-readiness-test
        framework.open_tms.pricing.readiness
        test_pricing_readiness.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-pricing-spot-test
        framework.open_tms.pricing.spot
        test_pricing_spot.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-pricing-valuation-quality-test
        framework.open_tms.pricing.valuation.quality
        test_pricing_valuation_quality.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-reconciliation-book-test
        framework.open_tms.reconciliation.book
        test_reconciliation_book.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-reconciliation-breaks-test
        framework.open_tms.reconciliation.breaks
        test_reconciliation_breaks.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-reconciliation-cash-test
        framework.open_tms.reconciliation.cash
        test_reconciliation_cash.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-reconciliation-completion-test
        framework.open_tms.reconciliation.completion
        test_reconciliation_completion.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-reconciliation-settlement-test
        framework.open_tms.reconciliation.settlement
        test_reconciliation_settlement.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-reconciliation-trade-test
        framework.open_tms.reconciliation.trade
        test_reconciliation_trade.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-risk-book-test
        framework.open_tms.risk.book
        test_risk_book.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-risk-breach-test
        framework.open_tms.risk.breach
        test_risk_breach.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-risk-counterparty-test
        framework.open_tms.risk.counterparty
        test_risk_counterparty.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-risk-credit-test
        framework.open_tms.risk.credit
        test_risk_credit.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-risk-limit-utilisation-test
        framework.open_tms.risk.limit.utilisation
        test_risk_limit_utilisation.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-risk-liquidity-test
        framework.open_tms.risk.liquidity
        test_risk_liquidity.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-risk-market-test
        framework.open_tms.risk.market
        test_risk_market.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-risk-readiness-test
        framework.open_tms.risk.readiness
        test_risk_readiness.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-risk-stress-test
        framework.open_tms.risk.stress
        test_risk_stress.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-service-catalogue-test
        framework.open_tms.service.catalogue
        test_service_catalogue.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-settlement-book-test
        framework.open_tms.settlement.book
        test_settlement_book.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-settlement-confirmation-test
        framework.open_tms.settlement.confirmation
        test_settlement_confirmation.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-settlement-failure-test
        framework.open_tms.settlement.failure
        test_settlement_failure.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-settlement-netting-test
        framework.open_tms.settlement.netting
        test_settlement_netting.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-settlement-obligation-test
        framework.open_tms.settlement.obligation
        test_settlement_obligation.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-settlement-queue-test
        framework.open_tms.settlement.queue
        test_settlement_queue.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-settlement-readiness-test
        framework.open_tms.settlement.readiness
        test_settlement_readiness.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-settlement-repair-test
        framework.open_tms.settlement.repair
        test_settlement_repair.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-settlement-window-test
        framework.open_tms.settlement.window
        test_settlement_window.c
    )
    umicom_add_open_tms_test(
        umicom-open-tms-types-test
        framework.open_tms.types
        test_types.c
    )
endif()

message(STATUS "Umicom Open TMS platform enabled")

# Boundary and lifecycle checks remain active in release test binaries too.
if(BUILD_TESTING)
    add_executable(umi-open-tms-forward-boundary-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/open_tms/test_fx_forward_boundaries.c")
    target_link_libraries(umi-open-tms-forward-boundary-test PRIVATE Umicom::finance)
    set_target_properties(umi-open-tms-forward-boundary-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umi-open-tms-forward-boundary-test)
    umicom_apply_sanitizers(umi-open-tms-forward-boundary-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umi-open-tms-forward-boundary-test)
    endif()
    foreach(case matrix lifecycle full stale-version unknown-state revision-limit not-found
        corrupt-count corrupt-row uninitialised value-zero value-negative value-unavailable
        value-overflow value-nan value-infinity value-positive invalid-api
        add-nan add-infinity add-market-nan add-market-negative add-notional-zero add-rate-zero
        add-unterminated-id add-unterminated-account add-unterminated-party add-empty add-control
        add-blank add-currency add-currency-width add-same-currency add-state add-zero-market add-duplicate)
        set(test framework.open_tms.forward.boundary.${case})
        add_test(NAME ${test} COMMAND umi-open-tms-forward-boundary-test ${case})
        set_tests_properties(${test} PROPERTIES TIMEOUT 30
            LABELS "framework;finance;treasury;lifecycle;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS umi-open-tms-forward-boundary-test)
        endif()
    endforeach()
    unset(test)
endif()

# One owned review crosses preview/application boundaries without borrowed rows.
if(BUILD_TESTING)
    add_executable(umi-open-tms-forward-review-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/open_tms/test_fx_forward_review.c")
    target_link_libraries(umi-open-tms-forward-review-test PRIVATE Umicom::finance)
    set_target_properties(umi-open-tms-forward-review-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umi-open-tms-forward-review-test)
    umicom_apply_sanitizers(umi-open-tms-forward-review-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umi-open-tms-forward-review-test)
    endif()
    foreach(case apply equivalent snapshot-isolation unused-bytes source-lifetime
        changed-id changed-account changed-party changed-base changed-quote changed-notional
        changed-rate changed-market changed-maturity changed-state changed-version changed-revision
        changed-unrelated changed-order changed-count changed-added changed-corrupt changed-nan changed-duplicate
        refuse-skip refuse-version refuse-missing refuse-limit refuse-count refuse-duplicate invalid-api)
        set(test framework.open_tms.forward.review.${case})
        add_test(NAME ${test} COMMAND umi-open-tms-forward-review-test ${case})
        set_tests_properties(${test} PROPERTIES TIMEOUT 30
            LABELS "framework;finance;treasury;review;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS umi-open-tms-forward-review-test)
        endif()
    endforeach()
    unset(test)
endif()

# Consumers use the same public interface in-tree and from an installed SDK.
if(BUILD_TESTING)
    add_executable(umi-treasury-forward-consumer
        "${CMAKE_CURRENT_LIST_DIR}/../examples/treasury_forward_review/main.c")
    target_link_libraries(umi-treasury-forward-consumer PRIVATE Umicom::finance)
    set_target_properties(umi-treasury-forward-consumer PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umi-treasury-forward-consumer)
    umicom_apply_sanitizers(umi-treasury-forward-consumer)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umi-treasury-forward-consumer)
    endif()
    # The earlier apply/stale cases remain; archive cases also exercise SDK closure.
    foreach(scenario apply stale archive-roundtrip archive-stale)
        set(test framework.open_tms.forward.consumer.${scenario})
        add_test(NAME ${test} COMMAND umi-treasury-forward-consumer ${scenario})
        set_tests_properties(${test} PROPERTIES TIMEOUT 30
            LABELS "framework;finance;treasury;review;sdk;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS umi-treasury-forward-consumer)
        endif()
    endforeach()
    unset(test)
endif()
install(FILES
    "${CMAKE_CURRENT_LIST_DIR}/../examples/treasury_forward_review/main.c"
    "${CMAKE_CURRENT_LIST_DIR}/../examples/treasury_forward_review/CMakeLists.txt"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/examples/treasury_forward_review COMPONENT Learning)

# File services are separately composed so portable finance stays base-only.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomTreasuryFiles.cmake")

# Native composition stays in Framework; headless finance retains no GTK requirement.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomTreasuryForwardUi.cmake")

# Persisted bytes are data, so decode exercises adversarial envelopes and rows.
if(BUILD_TESTING)
    add_executable(umi-forward-archive-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/open_tms/test_fx_forward_archive.c")
    target_link_libraries(umi-forward-archive-test PRIVATE Umicom::finance)
    set_target_properties(umi-forward-archive-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umi-forward-archive-test)
    umicom_apply_sanitizers(umi-forward-archive-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umi-forward-archive-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umi-forward-archive-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/base/value_archive_internal.h")
    endif()
    foreach(case roundtrip empty full workspace short tails truncation conditional stale
        conditional-invalid overlap invalid-api encode-count encode-text encode-nan
        checksum schema trailing count revision text-length text-nul duplicate state nan negative)
        set(test framework.open_tms.forward.archive.${case})
        add_test(NAME ${test} COMMAND umi-forward-archive-test ${case})
        set_tests_properties(${test} PROPERTIES TIMEOUT 30
            LABELS "framework;finance;treasury;persistence;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS umi-forward-archive-test)
        endif()
    endforeach()
    unset(test)
endif()

# Accounting approval belongs to the complete validated batch, separately by currency.
if(BUILD_TESTING)
    add_executable(umi-accounting-boundary-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/open_tms/test_accounting_boundaries.c")
    target_link_libraries(umi-accounting-boundary-test PRIVATE Umicom::finance)
    set_target_properties(umi-accounting-boundary-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umi-accounting-boundary-test)
    umicom_apply_sanitizers(umi-accounting-boundary-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umi-accounting-boundary-test)
    endif()
    foreach(case normal multi cross-currency edit-approved post-unbalanced post-nan post-before-approve
        post-twice approve-twice approve-posted overflow approve-overflow maximum tiny-one-side
        tolerance-in tolerance-out count revision-zero approved-flag posted-flag posted-without-approval
        empty-approved limit empty outputs currency-query revision-add revision-approve revision-post
        add-nan add-infinity add-negative add-both add-zero add-empty-account add-empty-reference
        add-blank add-control add-account-bound add-reference-bound add-currency add-currency-width
        add-currency-bound add-posted negative-zero)
        set(test framework.open_tms.accounting.boundary.${case})
        add_test(NAME ${test} COMMAND umi-accounting-boundary-test ${case})
        set_tests_properties(${test} PROPERTIES TIMEOUT 30
            LABELS "framework;finance;treasury;accounting;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS umi-accounting-boundary-test)
        endif()
    endforeach()
    unset(test)
endif()

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/open_tms/accounting_review.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/open_tms COMPONENT Framework)
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_finance include/umicom/open_tms/accounting_review.h)
endif()
if(BUILD_TESTING)
    add_executable(umi-accounting-review-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/open_tms/test_accounting_review.c")
    target_link_libraries(umi-accounting-review-test PRIVATE Umicom::finance)
    set_target_properties(umi-accounting-review-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umi-accounting-review-test)
    umicom_apply_sanitizers(umi-accounting-review-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umi-accounting-review-test)
    endif()
    foreach(case approve post equivalent source-lifetime snapshot line inactive-storage text-tails
        changed-account changed-reference changed-currency changed-debit changed-credit balanced-edit
        changed-count changed-order changed-revision changed-approved changed-posted changed-nan
        changed-text-bound changed-count-bound create-null create-action create-empty create-unbalanced
        create-unapproved-post create-repeat-approval create-revision-limit outputs)
        set(test framework.open_tms.accounting.review.${case})
        add_test(NAME ${test} COMMAND umi-accounting-review-test ${case})
        set_tests_properties(${test} PROPERTIES TIMEOUT 30 LABELS "framework;finance;treasury;accounting;review;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS umi-accounting-review-test)
        endif()
    endforeach()
    unset(test)
endif()

# The same sample exercises public ownership in-tree and against an installed SDK.
if(BUILD_TESTING)
    add_executable(umi-accounting-consumer
        "${CMAKE_CURRENT_LIST_DIR}/../examples/accounting_review/main.c")
    target_link_libraries(umi-accounting-consumer PRIVATE Umicom::finance)
    set_target_properties(umi-accounting-consumer PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umi-accounting-consumer)
    umicom_apply_sanitizers(umi-accounting-consumer)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umi-accounting-consumer)
    endif()
    foreach(scenario apply stale currencies archive-roundtrip archive-stale)
        set(test framework.open_tms.accounting.consumer.${scenario})
        add_test(NAME ${test} COMMAND umi-accounting-consumer ${scenario})
        set_tests_properties(${test} PROPERTIES TIMEOUT 30 LABELS "framework;finance;treasury;accounting;review;sdk;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS umi-accounting-consumer)
        endif()
    endforeach()
    unset(test)
endif()
install(FILES
    "${CMAKE_CURRENT_LIST_DIR}/../examples/accounting_review/main.c"
    "${CMAKE_CURRENT_LIST_DIR}/../examples/accounting_review/CMakeLists.txt"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/examples/accounting_review COMPONENT Learning)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/guides/accounting-review.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/guides COMPONENT Learning)

# Applications compose the optional native accounting surface from Framework.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomAccountingPracticeUi.cmake")

# Archives remain within portable finance; native filesystem ownership is separate.
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/open_tms/accounting_archive.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/open_tms COMPONENT Framework)
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_finance include/umicom/open_tms/accounting_archive.h)
endif()
if(BUILD_TESTING)
    add_executable(umi-accounting-archive-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/open_tms/test_accounting_archive.c")
    target_link_libraries(umi-accounting-archive-test PRIVATE Umicom::finance)
    set_target_properties(umi-accounting-archive-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umi-accounting-archive-test)
    umicom_apply_sanitizers(umi-accounting-archive-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umi-accounting-archive-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umi-accounting-archive-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/base/value_archive_internal.h"
            "${CMAKE_CURRENT_LIST_DIR}/../src/base/conditional_archive_internal.h")
    endif()
    foreach(case roundtrip empty full multi approved posted maximum-revision draft-unbalanced
        workspace short tails truncation conditional conditional-stale conditional-row conditional-tail
        conditional-invalid overlap invalid-api encode-count encode-text encode-nan encode-approved
        encode-posted checksum schema trailing count revision approved-flag posted-flag
        posted-without-approval approved-unbalanced approved-cross-currency text-length text-nul
        text-control nan negative both-sides)
        set(test framework.open_tms.accounting.archive.${case})
        add_test(NAME ${test} COMMAND umi-accounting-archive-test ${case})
        set_tests_properties(${test} PROPERTIES TIMEOUT 30 LABELS "framework;finance;treasury;accounting;persistence;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS umi-accounting-archive-test)
        endif()
    endforeach()
    unset(test)
endif()

# Reports reuse the shared CSV owner and remain usable without native widgets.
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/open_tms/accounting_report.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/open_tms COMPONENT Framework)
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_finance include/umicom/open_tms/accounting_report.h)
endif()
if(BUILD_TESTING)
    add_executable(umi-accounting-report-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/open_tms/test_accounting_report.c")
    target_link_libraries(umi-accounting-report-test PRIVATE Umicom::finance)
    set_target_properties(umi-accounting-report-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umi-accounting-report-test)
    umicom_apply_sanitizers(umi-accounting-report-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umi-accounting-report-test)
    endif()
    foreach(case empty balanced unbalanced cross-currency currency-order approved posted maximum-lines formula-account formula-reference quoted-text utf8 invalid-utf8 nonterminated bad-currency nan infinity negative both-sides zero-sides invalid-count invalid-revision invalid-flags unbalanced-approved overflow late-overflow null-book null-output snapshot maximum-revision short-copy)
        set(test framework.open_tms.accounting.report.${case})
        add_test(NAME ${test} COMMAND umi-accounting-report-test ${case})
        set_tests_properties(${test} PROPERTIES TIMEOUT 30
            LABELS "framework;finance;treasury;accounting;report;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS umi-accounting-report-test)
        endif()
    endforeach()
endif()
