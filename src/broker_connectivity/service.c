/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/service.c
 *
 * PURPOSE:
 *   Provide one discoverable broker-connectivity policy catalogue and a bounded
 *   platform snapshot over provider/account/order/execution/position/audit state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/broker_connectivity/service.h"

#include <string.h>

#include "umicom/broker_connectivity/account/buying_power.h"
#include "umicom/broker_connectivity/account/cash.h"
#include "umicom/broker_connectivity/account/currency_mapping.h"
#include "umicom/broker_connectivity/account/daily_pnl.h"
#include "umicom/broker_connectivity/account/margin_utilisation.h"
#include "umicom/broker_connectivity/account/net_liquidation.h"
#include "umicom/broker_connectivity/account/realised_pnl.h"
#include "umicom/broker_connectivity/account/reconciliation.h"
#include "umicom/broker_connectivity/account/sync.h"
#include "umicom/broker_connectivity/account/unrealised_pnl.h"
#include "umicom/broker_connectivity/audit/causation.h"
#include "umicom/broker_connectivity/audit/correction.h"
#include "umicom/broker_connectivity/audit/gap.h"
#include "umicom/broker_connectivity/audit/integrity.h"
#include "umicom/broker_connectivity/audit/persistence.h"
#include "umicom/broker_connectivity/audit/readiness.h"
#include "umicom/broker_connectivity/audit/rejection.h"
#include "umicom/broker_connectivity/audit/sequence.h"
#include "umicom/broker_connectivity/audit/source.h"
#include "umicom/broker_connectivity/audit/timestamp.h"
#include "umicom/broker_connectivity/execution/audit.h"
#include "umicom/broker_connectivity/execution/average_price.h"
#include "umicom/broker_connectivity/execution/commission.h"
#include "umicom/broker_connectivity/execution/correction.h"
#include "umicom/broker_connectivity/execution/duplicate_detection.h"
#include "umicom/broker_connectivity/execution/fill_progress.h"
#include "umicom/broker_connectivity/execution/late_detection.h"
#include "umicom/broker_connectivity/execution/quality.h"
#include "umicom/broker_connectivity/execution/reconciliation.h"
#include "umicom/broker_connectivity/execution/slippage.h"
#include "umicom/broker_connectivity/ibkr/account_mapping.h"
#include "umicom/broker_connectivity/ibkr/client_id.h"
#include "umicom/broker_connectivity/ibkr/configuration.h"
#include "umicom/broker_connectivity/ibkr/contract_mapping.h"
#include "umicom/broker_connectivity/ibkr/error_mapping.h"
#include "umicom/broker_connectivity/ibkr/execution_mapping.h"
#include "umicom/broker_connectivity/ibkr/heartbeat.h"
#include "umicom/broker_connectivity/ibkr/order_mapping.h"
#include "umicom/broker_connectivity/ibkr/status_mapping.h"
#include "umicom/broker_connectivity/ibkr/transport.h"
#include "umicom/broker_connectivity/order/ack_latency.h"
#include "umicom/broker_connectivity/order/cancel_gate.h"
#include "umicom/broker_connectivity/order/idempotency.h"
#include "umicom/broker_connectivity/order/mapping.h"
#include "umicom/broker_connectivity/order/reject_attention.h"
#include "umicom/broker_connectivity/order/replace_gate.h"
#include "umicom/broker_connectivity/order/route_readiness.h"
#include "umicom/broker_connectivity/order/state_sync.h"
#include "umicom/broker_connectivity/order/submit_gate.h"
#include "umicom/broker_connectivity/order/working_notional.h"
#include "umicom/broker_connectivity/position/average_cost.h"
#include "umicom/broker_connectivity/position/concentration.h"
#include "umicom/broker_connectivity/position/freshness.h"
#include "umicom/broker_connectivity/position/gross_exposure.h"
#include "umicom/broker_connectivity/position/market_value.h"
#include "umicom/broker_connectivity/position/net_exposure.h"
#include "umicom/broker_connectivity/position/quantity.h"
#include "umicom/broker_connectivity/position/realised_pnl.h"
#include "umicom/broker_connectivity/position/reconciliation.h"
#include "umicom/broker_connectivity/position/unrealised_pnl.h"
#include "umicom/broker_connectivity/provider/authentication.h"
#include "umicom/broker_connectivity/provider/capabilities.h"
#include "umicom/broker_connectivity/provider/connection.h"
#include "umicom/broker_connectivity/provider/contract.h"
#include "umicom/broker_connectivity/provider/health.h"
#include "umicom/broker_connectivity/provider/heartbeat.h"
#include "umicom/broker_connectivity/provider/latency.h"
#include "umicom/broker_connectivity/provider/live_mode.h"
#include "umicom/broker_connectivity/provider/paper_mode.h"
#include "umicom/broker_connectivity/provider/reconnect.h"
#include "umicom/broker_connectivity/risk/buying_power.h"
#include "umicom/broker_connectivity/risk/concentration.h"
#include "umicom/broker_connectivity/risk/daily_loss.h"
#include "umicom/broker_connectivity/risk/kill_switch.h"
#include "umicom/broker_connectivity/risk/leverage.h"
#include "umicom/broker_connectivity/risk/order_notional.h"
#include "umicom/broker_connectivity/risk/order_quantity.h"
#include "umicom/broker_connectivity/risk/position_quantity.h"
#include "umicom/broker_connectivity/risk/pretrade_readiness.h"
#include "umicom/broker_connectivity/risk/price_band.h"

typedef struct BrokerConnectivityEntry {
    const char *id;
    UmiBrokerConnectivityEvaluator evaluator;
} BrokerConnectivityEntry;

static const BrokerConnectivityEntry CATALOGUE[] = {
    {"account.buying-power", umi_broker_connectivity_evaluate_account_buying_power},
    {"account.cash", umi_broker_connectivity_evaluate_account_cash},
    {"account.currency-mapping", umi_broker_connectivity_evaluate_account_currency_mapping},
    {"account.daily-pnl", umi_broker_connectivity_evaluate_account_daily_pnl},
    {"account.margin-utilisation", umi_broker_connectivity_evaluate_account_margin_utilisation},
    {"account.net-liquidation", umi_broker_connectivity_evaluate_account_net_liquidation},
    {"account.realised-pnl", umi_broker_connectivity_evaluate_account_realised_pnl},
    {"account.reconciliation", umi_broker_connectivity_evaluate_account_reconciliation},
    {"account.sync", umi_broker_connectivity_evaluate_account_sync},
    {"account.unrealised-pnl", umi_broker_connectivity_evaluate_account_unrealised_pnl},
    {"audit.causation", umi_broker_connectivity_evaluate_audit_causation},
    {"audit.correction", umi_broker_connectivity_evaluate_audit_correction},
    {"audit.gap", umi_broker_connectivity_evaluate_audit_gap},
    {"audit.integrity", umi_broker_connectivity_evaluate_audit_integrity},
    {"audit.persistence", umi_broker_connectivity_evaluate_audit_persistence},
    {"audit.readiness", umi_broker_connectivity_evaluate_audit_readiness},
    {"audit.rejection", umi_broker_connectivity_evaluate_audit_rejection},
    {"audit.sequence", umi_broker_connectivity_evaluate_audit_sequence},
    {"audit.source", umi_broker_connectivity_evaluate_audit_source},
    {"audit.timestamp", umi_broker_connectivity_evaluate_audit_timestamp},
    {"execution.audit", umi_broker_connectivity_evaluate_execution_audit},
    {"execution.average-price", umi_broker_connectivity_evaluate_execution_average_price},
    {"execution.commission", umi_broker_connectivity_evaluate_execution_commission},
    {"execution.correction", umi_broker_connectivity_evaluate_execution_correction},
    {"execution.duplicate-detection", umi_broker_connectivity_evaluate_execution_duplicate_detection},
    {"execution.fill-progress", umi_broker_connectivity_evaluate_execution_fill_progress},
    {"execution.late-detection", umi_broker_connectivity_evaluate_execution_late_detection},
    {"execution.quality", umi_broker_connectivity_evaluate_execution_quality},
    {"execution.reconciliation", umi_broker_connectivity_evaluate_execution_reconciliation},
    {"execution.slippage", umi_broker_connectivity_evaluate_execution_slippage},
    {"ibkr.account-mapping", umi_broker_connectivity_evaluate_ibkr_account_mapping},
    {"ibkr.client-id", umi_broker_connectivity_evaluate_ibkr_client_id},
    {"ibkr.configuration", umi_broker_connectivity_evaluate_ibkr_configuration},
    {"ibkr.contract-mapping", umi_broker_connectivity_evaluate_ibkr_contract_mapping},
    {"ibkr.error-mapping", umi_broker_connectivity_evaluate_ibkr_error_mapping},
    {"ibkr.execution-mapping", umi_broker_connectivity_evaluate_ibkr_execution_mapping},
    {"ibkr.heartbeat", umi_broker_connectivity_evaluate_ibkr_heartbeat},
    {"ibkr.order-mapping", umi_broker_connectivity_evaluate_ibkr_order_mapping},
    {"ibkr.status-mapping", umi_broker_connectivity_evaluate_ibkr_status_mapping},
    {"ibkr.transport", umi_broker_connectivity_evaluate_ibkr_transport},
    {"order.ack-latency", umi_broker_connectivity_evaluate_order_ack_latency},
    {"order.cancel-gate", umi_broker_connectivity_evaluate_order_cancel_gate},
    {"order.idempotency", umi_broker_connectivity_evaluate_order_idempotency},
    {"order.mapping", umi_broker_connectivity_evaluate_order_mapping},
    {"order.reject-attention", umi_broker_connectivity_evaluate_order_reject_attention},
    {"order.replace-gate", umi_broker_connectivity_evaluate_order_replace_gate},
    {"order.route-readiness", umi_broker_connectivity_evaluate_order_route_readiness},
    {"order.state-sync", umi_broker_connectivity_evaluate_order_state_sync},
    {"order.submit-gate", umi_broker_connectivity_evaluate_order_submit_gate},
    {"order.working-notional", umi_broker_connectivity_evaluate_order_working_notional},
    {"position.average-cost", umi_broker_connectivity_evaluate_position_average_cost},
    {"position.concentration", umi_broker_connectivity_evaluate_position_concentration},
    {"position.freshness", umi_broker_connectivity_evaluate_position_freshness},
    {"position.gross-exposure", umi_broker_connectivity_evaluate_position_gross_exposure},
    {"position.market-value", umi_broker_connectivity_evaluate_position_market_value},
    {"position.net-exposure", umi_broker_connectivity_evaluate_position_net_exposure},
    {"position.quantity", umi_broker_connectivity_evaluate_position_quantity},
    {"position.realised-pnl", umi_broker_connectivity_evaluate_position_realised_pnl},
    {"position.reconciliation", umi_broker_connectivity_evaluate_position_reconciliation},
    {"position.unrealised-pnl", umi_broker_connectivity_evaluate_position_unrealised_pnl},
    {"provider.authentication", umi_broker_connectivity_evaluate_provider_authentication},
    {"provider.capabilities", umi_broker_connectivity_evaluate_provider_capabilities},
    {"provider.connection", umi_broker_connectivity_evaluate_provider_connection},
    {"provider.contract", umi_broker_connectivity_evaluate_provider_contract},
    {"provider.health", umi_broker_connectivity_evaluate_provider_health},
    {"provider.heartbeat", umi_broker_connectivity_evaluate_provider_heartbeat},
    {"provider.latency", umi_broker_connectivity_evaluate_provider_latency},
    {"provider.live-mode", umi_broker_connectivity_evaluate_provider_live_mode},
    {"provider.paper-mode", umi_broker_connectivity_evaluate_provider_paper_mode},
    {"provider.reconnect", umi_broker_connectivity_evaluate_provider_reconnect},
    {"risk.buying-power", umi_broker_connectivity_evaluate_risk_buying_power},
    {"risk.concentration", umi_broker_connectivity_evaluate_risk_concentration},
    {"risk.daily-loss", umi_broker_connectivity_evaluate_risk_daily_loss},
    {"risk.kill-switch", umi_broker_connectivity_evaluate_risk_kill_switch},
    {"risk.leverage", umi_broker_connectivity_evaluate_risk_leverage},
    {"risk.order-notional", umi_broker_connectivity_evaluate_risk_order_notional},
    {"risk.order-quantity", umi_broker_connectivity_evaluate_risk_order_quantity},
    {"risk.position-quantity", umi_broker_connectivity_evaluate_risk_position_quantity},
    {"risk.pretrade-readiness", umi_broker_connectivity_evaluate_risk_pretrade_readiness},
    {"risk.price-band", umi_broker_connectivity_evaluate_risk_price_band},
};

size_t umi_broker_connectivity_service_count(void)
{
    return sizeof(CATALOGUE) / sizeof(CATALOGUE[0]);
}

UmiStatus umi_broker_connectivity_service_id_at(
    size_t index,
    const char **outId)
{
    if (outId == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outId = NULL;
    if (index >= umi_broker_connectivity_service_count()) {
        return UMI_STATUS_NOT_FOUND;
    }
    *outId = CATALOGUE[index].id;
    return UMI_STATUS_OK;
}

UmiStatus umi_broker_connectivity_service_evaluate(
    const char *id,
    const UmiBrokerConnectivityInput *input,
    UmiBrokerConnectivitySnapshot *outSnapshot)
{
    size_t index;
    if (id == NULL || id[0] == '\0' || input == NULL ||
        outSnapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < umi_broker_connectivity_service_count(); ++index) {
        if (strcmp(CATALOGUE[index].id, id) == 0) {
            return CATALOGUE[index].evaluator(input, outSnapshot);
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_broker_connectivity_platform_snapshot(
    const UmiBrokerProviderRegistry *providers,
    const UmiBrokerSessionSupervisor *session,
    const UmiBrokerAccountStore *accounts,
    const UmiBrokerOrderJournal *orders,
    const UmiBrokerExecutionReconciler *executions,
    const UmiBrokerPositionStore *positions,
    const UmiBrokerAuditJournal *audit,
    const UmiIbkrAdapterConfig *ibkr,
    int liveApproved,
    UmiBrokerConnectivityPlatformSnapshot *outSnapshot)
{
    uint64_t revision = 0U;

    if (providers == NULL || session == NULL || accounts == NULL ||
        orders == NULL || executions == NULL || positions == NULL ||
        audit == NULL || ibkr == NULL || outSnapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(outSnapshot, 0, sizeof(*outSnapshot));
    outSnapshot->providerCount = providers->count;
    outSnapshot->accountCount = accounts->count;
    outSnapshot->orderCount = orders->count;
    outSnapshot->executionCount = executions->count;
    outSnapshot->positionCount = positions->count;
    outSnapshot->auditCount = audit->count;
    outSnapshot->duplicateExecutions = executions->duplicates;
    outSnapshot->correctedExecutions = executions->corrections;
    outSnapshot->lateExecutions = executions->lateReports;
    outSnapshot->auditGapCount = audit->gapCount;
    outSnapshot->persistedAuditCount =
        umi_broker_audit_journal_persisted_count(audit);

    outSnapshot->connected =
        session->state == UMI_BROKER_SESSION_CONNECTED ||
        session->state == UMI_BROKER_SESSION_AUTHENTICATED;
    outSnapshot->authenticated =
        session->state == UMI_BROKER_SESSION_AUTHENTICATED;
    outSnapshot->paperReady =
        outSnapshot->authenticated && providers->count > 0U;
    outSnapshot->liveReady =
        outSnapshot->authenticated &&
        providers->count > 0U &&
        liveApproved;
    outSnapshot->ibkrConfigured =
        umi_ibkr_adapter_config_validate(ibkr) == UMI_STATUS_OK;
    outSnapshot->accountReady = accounts->count > 0U;
    outSnapshot->executionReady = executions->count > 0U;
    outSnapshot->positionReady = positions->count > 0U;
    outSnapshot->auditReady =
        audit->count > 0U &&
        audit->gapCount == 0U &&
        outSnapshot->persistedAuditCount == audit->count;

    if (providers->revision > revision) revision = providers->revision;
    if (session->revision > revision) revision = session->revision;
    if (accounts->revision > revision) revision = accounts->revision;
    if (orders->revision > revision) revision = orders->revision;
    if (executions->revision > revision) revision = executions->revision;
    if (positions->revision > revision) revision = positions->revision;
    if (audit->revision > revision) revision = audit->revision;
    outSnapshot->revision = revision;
    return UMI_STATUS_OK;
}
