/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/service.c
 *
 * PURPOSE:
 *   Provide the Open TMS capability catalogue and treasury operations snapshot.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/open_tms/service.h"
#include <string.h>
#include "umicom/open_tms/accounting/accrual.h"
#include "umicom/open_tms/accounting/double_entry.h"
#include "umicom/open_tms/accounting/event.h"
#include "umicom/open_tms/accounting/pnl.h"
#include "umicom/open_tms/accounting/posting.h"
#include "umicom/open_tms/accounting/reconciliation.h"
#include "umicom/open_tms/cash/account_coverage.h"
#include "umicom/open_tms/cash/currency_coverage.h"
#include "umicom/open_tms/cash/forecast_net.h"
#include "umicom/open_tms/cash/freshness.h"
#include "umicom/open_tms/cash/minimum_balance.h"
#include "umicom/open_tms/cash/projected_balance.h"
#include "umicom/open_tms/cash/settled_balance.h"
#include "umicom/open_tms/cash/shortfall.h"
#include "umicom/open_tms/cash/surplus.h"
#include "umicom/open_tms/cash/sweep_readiness.h"
#include "umicom/open_tms/dashboard/cash_health.h"
#include "umicom/open_tms/dashboard/liquidity_health.h"
#include "umicom/open_tms/dashboard/operations_health.h"
#include "umicom/open_tms/dashboard/risk_health.h"
#include "umicom/open_tms/dashboard/settlement_health.h"
#include "umicom/open_tms/fx_forward/audit.h"
#include "umicom/open_tms/fx_forward/confirmation.h"
#include "umicom/open_tms/fx_forward/forward_points.h"
#include "umicom/open_tms/fx_forward/forward_rate.h"
#include "umicom/open_tms/fx_forward/lifecycle.h"
#include "umicom/open_tms/fx_forward/maturity.h"
#include "umicom/open_tms/fx_forward/mtm.h"
#include "umicom/open_tms/fx_forward/notional.h"
#include "umicom/open_tms/fx_forward/settlement.h"
#include "umicom/open_tms/fx_forward/trade_readiness.h"
#include "umicom/open_tms/liquidity/bucket_coverage.h"
#include "umicom/open_tms/liquidity/buffer.h"
#include "umicom/open_tms/liquidity/concentration.h"
#include "umicom/open_tms/liquidity/coverage.h"
#include "umicom/open_tms/liquidity/funding_capacity.h"
#include "umicom/open_tms/liquidity/funding_requirement.h"
#include "umicom/open_tms/liquidity/gap.h"
#include "umicom/open_tms/liquidity/intraday.h"
#include "umicom/open_tms/liquidity/readiness.h"
#include "umicom/open_tms/liquidity/stress.h"
#include "umicom/open_tms/position/aggregation.h"
#include "umicom/open_tms/position/concentration.h"
#include "umicom/open_tms/position/currency_exposure.h"
#include "umicom/open_tms/position/gross_exposure.h"
#include "umicom/open_tms/position/mtm.h"
#include "umicom/open_tms/position/net_exposure.h"
#include "umicom/open_tms/position/readiness.h"
#include "umicom/open_tms/position/trade_coverage.h"
#include "umicom/open_tms/pricing/accrual.h"
#include "umicom/open_tms/pricing/curve_coverage.h"
#include "umicom/open_tms/pricing/discount_factor.h"
#include "umicom/open_tms/pricing/fixing_coverage.h"
#include "umicom/open_tms/pricing/forward_rate.h"
#include "umicom/open_tms/pricing/market_data.h"
#include "umicom/open_tms/pricing/present_value.h"
#include "umicom/open_tms/pricing/readiness.h"
#include "umicom/open_tms/pricing/spot.h"
#include "umicom/open_tms/pricing/valuation_quality.h"
#include "umicom/open_tms/reconciliation/breaks.h"
#include "umicom/open_tms/reconciliation/cash.h"
#include "umicom/open_tms/reconciliation/completion.h"
#include "umicom/open_tms/reconciliation/settlement.h"
#include "umicom/open_tms/reconciliation/trade.h"
#include "umicom/open_tms/risk/breach.h"
#include "umicom/open_tms/risk/counterparty.h"
#include "umicom/open_tms/risk/credit.h"
#include "umicom/open_tms/risk/limit_utilisation.h"
#include "umicom/open_tms/risk/liquidity.h"
#include "umicom/open_tms/risk/market.h"
#include "umicom/open_tms/risk/readiness.h"
#include "umicom/open_tms/risk/stress.h"
#include "umicom/open_tms/settlement/confirmation.h"
#include "umicom/open_tms/settlement/failure.h"
#include "umicom/open_tms/settlement/netting.h"
#include "umicom/open_tms/settlement/obligation.h"
#include "umicom/open_tms/settlement/queue.h"
#include "umicom/open_tms/settlement/readiness.h"
#include "umicom/open_tms/settlement/repair.h"
#include "umicom/open_tms/settlement/window.h"

typedef struct Entry { const char *id; UmiOpenTmsEvaluator evaluator; } Entry;
static const Entry CATALOGUE[] = {
    {"accounting.accrual", umi_open_tms_evaluate_accounting_accrual},
    {"accounting.double-entry", umi_open_tms_evaluate_accounting_double_entry},
    {"accounting.event", umi_open_tms_evaluate_accounting_event},
    {"accounting.pnl", umi_open_tms_evaluate_accounting_pnl},
    {"accounting.posting", umi_open_tms_evaluate_accounting_posting},
    {"accounting.reconciliation", umi_open_tms_evaluate_accounting_reconciliation},
    {"cash.account-coverage", umi_open_tms_evaluate_cash_account_coverage},
    {"cash.currency-coverage", umi_open_tms_evaluate_cash_currency_coverage},
    {"cash.forecast-net", umi_open_tms_evaluate_cash_forecast_net},
    {"cash.freshness", umi_open_tms_evaluate_cash_freshness},
    {"cash.minimum-balance", umi_open_tms_evaluate_cash_minimum_balance},
    {"cash.projected-balance", umi_open_tms_evaluate_cash_projected_balance},
    {"cash.settled-balance", umi_open_tms_evaluate_cash_settled_balance},
    {"cash.shortfall", umi_open_tms_evaluate_cash_shortfall},
    {"cash.surplus", umi_open_tms_evaluate_cash_surplus},
    {"cash.sweep-readiness", umi_open_tms_evaluate_cash_sweep_readiness},
    {"dashboard.cash-health", umi_open_tms_evaluate_dashboard_cash_health},
    {"dashboard.liquidity-health", umi_open_tms_evaluate_dashboard_liquidity_health},
    {"dashboard.operations-health", umi_open_tms_evaluate_dashboard_operations_health},
    {"dashboard.risk-health", umi_open_tms_evaluate_dashboard_risk_health},
    {"dashboard.settlement-health", umi_open_tms_evaluate_dashboard_settlement_health},
    {"fx_forward.audit", umi_open_tms_evaluate_fx_forward_audit},
    {"fx_forward.confirmation", umi_open_tms_evaluate_fx_forward_confirmation},
    {"fx_forward.forward-points", umi_open_tms_evaluate_fx_forward_forward_points},
    {"fx_forward.forward-rate", umi_open_tms_evaluate_fx_forward_forward_rate},
    {"fx_forward.lifecycle", umi_open_tms_evaluate_fx_forward_lifecycle},
    {"fx_forward.maturity", umi_open_tms_evaluate_fx_forward_maturity},
    {"fx_forward.mtm", umi_open_tms_evaluate_fx_forward_mtm},
    {"fx_forward.notional", umi_open_tms_evaluate_fx_forward_notional},
    {"fx_forward.settlement", umi_open_tms_evaluate_fx_forward_settlement},
    {"fx_forward.trade-readiness", umi_open_tms_evaluate_fx_forward_trade_readiness},
    {"liquidity.bucket-coverage", umi_open_tms_evaluate_liquidity_bucket_coverage},
    {"liquidity.buffer", umi_open_tms_evaluate_liquidity_buffer},
    {"liquidity.concentration", umi_open_tms_evaluate_liquidity_concentration},
    {"liquidity.coverage", umi_open_tms_evaluate_liquidity_coverage},
    {"liquidity.funding-capacity", umi_open_tms_evaluate_liquidity_funding_capacity},
    {"liquidity.funding-requirement", umi_open_tms_evaluate_liquidity_funding_requirement},
    {"liquidity.gap", umi_open_tms_evaluate_liquidity_gap},
    {"liquidity.intraday", umi_open_tms_evaluate_liquidity_intraday},
    {"liquidity.readiness", umi_open_tms_evaluate_liquidity_readiness},
    {"liquidity.stress", umi_open_tms_evaluate_liquidity_stress},
    {"position.aggregation", umi_open_tms_evaluate_position_aggregation},
    {"position.concentration", umi_open_tms_evaluate_position_concentration},
    {"position.currency-exposure", umi_open_tms_evaluate_position_currency_exposure},
    {"position.gross-exposure", umi_open_tms_evaluate_position_gross_exposure},
    {"position.mtm", umi_open_tms_evaluate_position_mtm},
    {"position.net-exposure", umi_open_tms_evaluate_position_net_exposure},
    {"position.readiness", umi_open_tms_evaluate_position_readiness},
    {"position.trade-coverage", umi_open_tms_evaluate_position_trade_coverage},
    {"pricing.accrual", umi_open_tms_evaluate_pricing_accrual},
    {"pricing.curve-coverage", umi_open_tms_evaluate_pricing_curve_coverage},
    {"pricing.discount-factor", umi_open_tms_evaluate_pricing_discount_factor},
    {"pricing.fixing-coverage", umi_open_tms_evaluate_pricing_fixing_coverage},
    {"pricing.forward-rate", umi_open_tms_evaluate_pricing_forward_rate},
    {"pricing.market-data", umi_open_tms_evaluate_pricing_market_data},
    {"pricing.present-value", umi_open_tms_evaluate_pricing_present_value},
    {"pricing.readiness", umi_open_tms_evaluate_pricing_readiness},
    {"pricing.spot", umi_open_tms_evaluate_pricing_spot},
    {"pricing.valuation-quality", umi_open_tms_evaluate_pricing_valuation_quality},
    {"reconciliation.breaks", umi_open_tms_evaluate_reconciliation_breaks},
    {"reconciliation.cash", umi_open_tms_evaluate_reconciliation_cash},
    {"reconciliation.completion", umi_open_tms_evaluate_reconciliation_completion},
    {"reconciliation.settlement", umi_open_tms_evaluate_reconciliation_settlement},
    {"reconciliation.trade", umi_open_tms_evaluate_reconciliation_trade},
    {"risk.breach", umi_open_tms_evaluate_risk_breach},
    {"risk.counterparty", umi_open_tms_evaluate_risk_counterparty},
    {"risk.credit", umi_open_tms_evaluate_risk_credit},
    {"risk.limit-utilisation", umi_open_tms_evaluate_risk_limit_utilisation},
    {"risk.liquidity", umi_open_tms_evaluate_risk_liquidity},
    {"risk.market", umi_open_tms_evaluate_risk_market},
    {"risk.readiness", umi_open_tms_evaluate_risk_readiness},
    {"risk.stress", umi_open_tms_evaluate_risk_stress},
    {"settlement.confirmation", umi_open_tms_evaluate_settlement_confirmation},
    {"settlement.failure", umi_open_tms_evaluate_settlement_failure},
    {"settlement.netting", umi_open_tms_evaluate_settlement_netting},
    {"settlement.obligation", umi_open_tms_evaluate_settlement_obligation},
    {"settlement.queue", umi_open_tms_evaluate_settlement_queue},
    {"settlement.readiness", umi_open_tms_evaluate_settlement_readiness},
    {"settlement.repair", umi_open_tms_evaluate_settlement_repair},
    {"settlement.window", umi_open_tms_evaluate_settlement_window},
};


void umi_open_tms_service_init(UmiOpenTmsService *service)
{
    if (service == NULL) return;
    (void)memset(service, 0, sizeof(*service));
    umi_open_tms_fx_forward_book_init(&service->forwards);
    umi_open_tms_cash_book_init(&service->cash);
    umi_open_tms_liquidity_ladder_init(&service->liquidity);
    umi_open_tms_position_book_init(&service->positions);
    umi_open_tms_risk_book_init(&service->risk);
    umi_open_tms_settlement_book_init(&service->settlements);
    umi_open_tms_accounting_book_init(&service->accounting);
    umi_open_tms_reconciliation_book_init(&service->reconciliation);
    service->revision = 1U;
}

size_t umi_open_tms_service_count(void)
{
    return sizeof(CATALOGUE) / sizeof(CATALOGUE[0]);
}

UmiStatus umi_open_tms_service_id_at(size_t index, const char **outId)
{
    if (outId == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outId = NULL;
    if (index >= umi_open_tms_service_count()) return UMI_STATUS_NOT_FOUND;
    *outId = CATALOGUE[index].id;
    return UMI_STATUS_OK;
}

UmiStatus umi_open_tms_service_evaluate(
    const char *id,
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    size_t index;

    if (id == NULL || id[0] == '\0' || input == NULL || outSnapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < umi_open_tms_service_count(); ++index) {
        if (strcmp(CATALOGUE[index].id, id) == 0) {
            return CATALOGUE[index].evaluator(input, outSnapshot);
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_open_tms_service_snapshot(
    const UmiOpenTmsService *service,
    uint64_t nowMilliseconds,
    UmiOpenTmsServiceSnapshot *outSnapshot)
{
    UmiStatus status;

    if (service == NULL || outSnapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(outSnapshot, 0, sizeof(*outSnapshot));
    outSnapshot->policyCount = umi_open_tms_service_count();
    outSnapshot->forwardCount = service->forwards.count;
    outSnapshot->cashCount = service->cash.count;
    outSnapshot->liquidityBucketCount = service->liquidity.count;
    outSnapshot->positionCount = service->positions.count;
    outSnapshot->riskLimitCount = service->risk.count;
    outSnapshot->settlementCount = service->settlements.count;
    outSnapshot->reconciliationCount = service->reconciliation.count;

    status = umi_open_tms_dashboard_snapshot(
        &service->cash,
        &service->liquidity,
        &service->positions,
        &service->risk,
        &service->settlements,
        &service->accounting,
        &service->reconciliation,
        nowMilliseconds,
        &outSnapshot->dashboard);
    if (status != UMI_STATUS_OK) return status;

    outSnapshot->revision = outSnapshot->dashboard.revision > service->revision
        ? outSnapshot->dashboard.revision
        : service->revision;
    return UMI_STATUS_OK;
}
