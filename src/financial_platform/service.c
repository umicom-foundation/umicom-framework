/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/service.c
 *
 * PURPOSE:
 *   Provide one discoverable financial-control policy catalogue and a bounded
 *   cross-domain service snapshot over canonical Framework financial engines.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/financial_platform/service.h"

#include <string.h>

#include "umicom/financial_platform/account/available_funds.h"
#include "umicom/financial_platform/account/balance_consistency.h"
#include "umicom/financial_platform/account/currency.h"
#include "umicom/financial_platform/account/identity.h"
#include "umicom/financial_platform/account/ownership.h"
#include "umicom/financial_platform/account/reconciliation.h"
#include "umicom/financial_platform/account/settlement.h"
#include "umicom/financial_platform/account/status.h"
#include "umicom/financial_platform/audit/actor.h"
#include "umicom/financial_platform/audit/causation.h"
#include "umicom/financial_platform/audit/immutability.h"
#include "umicom/financial_platform/audit/lineage.h"
#include "umicom/financial_platform/audit/persistence.h"
#include "umicom/financial_platform/audit/readiness.h"
#include "umicom/financial_platform/audit/sequence.h"
#include "umicom/financial_platform/audit/timestamp.h"
#include "umicom/financial_platform/currency/catalogue_coverage.h"
#include "umicom/financial_platform/currency/code_validation.h"
#include "umicom/financial_platform/currency/conversion_consistency.h"
#include "umicom/financial_platform/currency/pair_readiness.h"
#include "umicom/financial_platform/currency/precision_validation.h"
#include "umicom/financial_platform/currency/rate_freshness.h"
#include "umicom/financial_platform/currency/rate_tolerance.h"
#include "umicom/financial_platform/currency/settlement_currency.h"
#include "umicom/financial_platform/instrument/currency.h"
#include "umicom/financial_platform/instrument/identity.h"
#include "umicom/financial_platform/instrument/lifecycle.h"
#include "umicom/financial_platform/instrument/market_data.h"
#include "umicom/financial_platform/instrument/maturity.h"
#include "umicom/financial_platform/instrument/price_precision.h"
#include "umicom/financial_platform/instrument/product_mapping.h"
#include "umicom/financial_platform/instrument/settlement.h"
#include "umicom/financial_platform/ledger/account_mapping.h"
#include "umicom/financial_platform/ledger/double_entry.h"
#include "umicom/financial_platform/ledger/journal_validation.h"
#include "umicom/financial_platform/ledger/period.h"
#include "umicom/financial_platform/ledger/posting.h"
#include "umicom/financial_platform/ledger/subledger.h"
#include "umicom/financial_platform/ledger/suspense.h"
#include "umicom/financial_platform/ledger/trial_balance.h"
#include "umicom/financial_platform/party/counterparty.h"
#include "umicom/financial_platform/party/identity.h"
#include "umicom/financial_platform/party/kyc_readiness.h"
#include "umicom/financial_platform/party/legal_entity.h"
#include "umicom/financial_platform/party/lifecycle.h"
#include "umicom/financial_platform/party/ownership_consistency.h"
#include "umicom/financial_platform/party/relationship_coverage.h"
#include "umicom/financial_platform/party/role_coverage.h"
#include "umicom/financial_platform/payment/amount.h"
#include "umicom/financial_platform/payment/approval.h"
#include "umicom/financial_platform/payment/beneficiary.h"
#include "umicom/financial_platform/payment/currency.h"
#include "umicom/financial_platform/payment/cutoff.h"
#include "umicom/financial_platform/payment/instruction.h"
#include "umicom/financial_platform/payment/route.h"
#include "umicom/financial_platform/payment/settlement.h"
#include "umicom/financial_platform/position/concentration.h"
#include "umicom/financial_platform/position/currency.h"
#include "umicom/financial_platform/position/gross_exposure.h"
#include "umicom/financial_platform/position/market_value.h"
#include "umicom/financial_platform/position/net_exposure.h"
#include "umicom/financial_platform/position/pnl.h"
#include "umicom/financial_platform/position/quantity.h"
#include "umicom/financial_platform/position/reconciliation.h"
#include "umicom/financial_platform/reconciliation/ageing.h"
#include "umicom/financial_platform/reconciliation/automation.h"
#include "umicom/financial_platform/reconciliation/break_count.h"
#include "umicom/financial_platform/reconciliation/completion.h"
#include "umicom/financial_platform/reconciliation/coverage.h"
#include "umicom/financial_platform/reconciliation/exception.h"
#include "umicom/financial_platform/reconciliation/matching.h"
#include "umicom/financial_platform/reconciliation/tolerance.h"
#include "umicom/financial_platform/settlement/cash_leg.h"
#include "umicom/financial_platform/settlement/due_date.h"
#include "umicom/financial_platform/settlement/fail_age.h"
#include "umicom/financial_platform/settlement/instruction.h"
#include "umicom/financial_platform/settlement/netting.h"
#include "umicom/financial_platform/settlement/reconciliation.h"
#include "umicom/financial_platform/settlement/securities_leg.h"
#include "umicom/financial_platform/settlement/status.h"
#include "umicom/financial_platform/trade/audit_coverage.h"
#include "umicom/financial_platform/trade/counterparty.h"
#include "umicom/financial_platform/trade/economics.h"
#include "umicom/financial_platform/trade/identity.h"
#include "umicom/financial_platform/trade/lifecycle.h"
#include "umicom/financial_platform/trade/notional.h"
#include "umicom/financial_platform/trade/position_impact.h"
#include "umicom/financial_platform/trade/settlement.h"
#include "umicom/financial_platform/workflow/approval.h"
#include "umicom/financial_platform/workflow/completion.h"
#include "umicom/financial_platform/workflow/escalation.h"
#include "umicom/financial_platform/workflow/role_coverage.h"
#include "umicom/financial_platform/workflow/rollback.h"
#include "umicom/financial_platform/workflow/state_transition.h"
#include "umicom/financial_platform/workflow/task_coverage.h"
#include "umicom/financial_platform/workflow/timeout.h"

typedef struct UmiFinancialPlatformCatalogueEntry {
    const char *id;
    UmiFinancialPlatformEvaluator evaluator;
} UmiFinancialPlatformCatalogueEntry;

static const UmiFinancialPlatformCatalogueEntry CATALOGUE[] = {
    {"account.available-funds", umi_financial_platform_evaluate_account_available_funds},
    {"account.balance-consistency", umi_financial_platform_evaluate_account_balance_consistency},
    {"account.currency", umi_financial_platform_evaluate_account_currency},
    {"account.identity", umi_financial_platform_evaluate_account_identity},
    {"account.ownership", umi_financial_platform_evaluate_account_ownership},
    {"account.reconciliation", umi_financial_platform_evaluate_account_reconciliation},
    {"account.settlement", umi_financial_platform_evaluate_account_settlement},
    {"account.status", umi_financial_platform_evaluate_account_status},
    {"audit.actor", umi_financial_platform_evaluate_audit_actor},
    {"audit.causation", umi_financial_platform_evaluate_audit_causation},
    {"audit.immutability", umi_financial_platform_evaluate_audit_immutability},
    {"audit.lineage", umi_financial_platform_evaluate_audit_lineage},
    {"audit.persistence", umi_financial_platform_evaluate_audit_persistence},
    {"audit.readiness", umi_financial_platform_evaluate_audit_readiness},
    {"audit.sequence", umi_financial_platform_evaluate_audit_sequence},
    {"audit.timestamp", umi_financial_platform_evaluate_audit_timestamp},
    {"currency.catalogue-coverage", umi_financial_platform_evaluate_currency_catalogue_coverage},
    {"currency.code-validation", umi_financial_platform_evaluate_currency_code_validation},
    {"currency.conversion-consistency", umi_financial_platform_evaluate_currency_conversion_consistency},
    {"currency.pair-readiness", umi_financial_platform_evaluate_currency_pair_readiness},
    {"currency.precision-validation", umi_financial_platform_evaluate_currency_precision_validation},
    {"currency.rate-freshness", umi_financial_platform_evaluate_currency_rate_freshness},
    {"currency.rate-tolerance", umi_financial_platform_evaluate_currency_rate_tolerance},
    {"currency.settlement-currency", umi_financial_platform_evaluate_currency_settlement_currency},
    {"instrument.currency", umi_financial_platform_evaluate_instrument_currency},
    {"instrument.identity", umi_financial_platform_evaluate_instrument_identity},
    {"instrument.lifecycle", umi_financial_platform_evaluate_instrument_lifecycle},
    {"instrument.market-data", umi_financial_platform_evaluate_instrument_market_data},
    {"instrument.maturity", umi_financial_platform_evaluate_instrument_maturity},
    {"instrument.price-precision", umi_financial_platform_evaluate_instrument_price_precision},
    {"instrument.product-mapping", umi_financial_platform_evaluate_instrument_product_mapping},
    {"instrument.settlement", umi_financial_platform_evaluate_instrument_settlement},
    {"ledger.account-mapping", umi_financial_platform_evaluate_ledger_account_mapping},
    {"ledger.double-entry", umi_financial_platform_evaluate_ledger_double_entry},
    {"ledger.journal-validation", umi_financial_platform_evaluate_ledger_journal_validation},
    {"ledger.period", umi_financial_platform_evaluate_ledger_period},
    {"ledger.posting", umi_financial_platform_evaluate_ledger_posting},
    {"ledger.subledger", umi_financial_platform_evaluate_ledger_subledger},
    {"ledger.suspense", umi_financial_platform_evaluate_ledger_suspense},
    {"ledger.trial-balance", umi_financial_platform_evaluate_ledger_trial_balance},
    {"party.counterparty", umi_financial_platform_evaluate_party_counterparty},
    {"party.identity", umi_financial_platform_evaluate_party_identity},
    {"party.kyc-readiness", umi_financial_platform_evaluate_party_kyc_readiness},
    {"party.legal-entity", umi_financial_platform_evaluate_party_legal_entity},
    {"party.lifecycle", umi_financial_platform_evaluate_party_lifecycle},
    {"party.ownership-consistency", umi_financial_platform_evaluate_party_ownership_consistency},
    {"party.relationship-coverage", umi_financial_platform_evaluate_party_relationship_coverage},
    {"party.role-coverage", umi_financial_platform_evaluate_party_role_coverage},
    {"payment.amount", umi_financial_platform_evaluate_payment_amount},
    {"payment.approval", umi_financial_platform_evaluate_payment_approval},
    {"payment.beneficiary", umi_financial_platform_evaluate_payment_beneficiary},
    {"payment.currency", umi_financial_platform_evaluate_payment_currency},
    {"payment.cutoff", umi_financial_platform_evaluate_payment_cutoff},
    {"payment.instruction", umi_financial_platform_evaluate_payment_instruction},
    {"payment.route", umi_financial_platform_evaluate_payment_route},
    {"payment.settlement", umi_financial_platform_evaluate_payment_settlement},
    {"position.concentration", umi_financial_platform_evaluate_position_concentration},
    {"position.currency", umi_financial_platform_evaluate_position_currency},
    {"position.gross-exposure", umi_financial_platform_evaluate_position_gross_exposure},
    {"position.market-value", umi_financial_platform_evaluate_position_market_value},
    {"position.net-exposure", umi_financial_platform_evaluate_position_net_exposure},
    {"position.pnl", umi_financial_platform_evaluate_position_pnl},
    {"position.quantity", umi_financial_platform_evaluate_position_quantity},
    {"position.reconciliation", umi_financial_platform_evaluate_position_reconciliation},
    {"reconciliation.ageing", umi_financial_platform_evaluate_reconciliation_ageing},
    {"reconciliation.automation", umi_financial_platform_evaluate_reconciliation_automation},
    {"reconciliation.break-count", umi_financial_platform_evaluate_reconciliation_break_count},
    {"reconciliation.completion", umi_financial_platform_evaluate_reconciliation_completion},
    {"reconciliation.coverage", umi_financial_platform_evaluate_reconciliation_coverage},
    {"reconciliation.exception", umi_financial_platform_evaluate_reconciliation_exception},
    {"reconciliation.matching", umi_financial_platform_evaluate_reconciliation_matching},
    {"reconciliation.tolerance", umi_financial_platform_evaluate_reconciliation_tolerance},
    {"settlement.cash-leg", umi_financial_platform_evaluate_settlement_cash_leg},
    {"settlement.due-date", umi_financial_platform_evaluate_settlement_due_date},
    {"settlement.fail-age", umi_financial_platform_evaluate_settlement_fail_age},
    {"settlement.instruction", umi_financial_platform_evaluate_settlement_instruction},
    {"settlement.netting", umi_financial_platform_evaluate_settlement_netting},
    {"settlement.reconciliation", umi_financial_platform_evaluate_settlement_reconciliation},
    {"settlement.securities-leg", umi_financial_platform_evaluate_settlement_securities_leg},
    {"settlement.status", umi_financial_platform_evaluate_settlement_status},
    {"trade.audit-coverage", umi_financial_platform_evaluate_trade_audit_coverage},
    {"trade.counterparty", umi_financial_platform_evaluate_trade_counterparty},
    {"trade.economics", umi_financial_platform_evaluate_trade_economics},
    {"trade.identity", umi_financial_platform_evaluate_trade_identity},
    {"trade.lifecycle", umi_financial_platform_evaluate_trade_lifecycle},
    {"trade.notional", umi_financial_platform_evaluate_trade_notional},
    {"trade.position-impact", umi_financial_platform_evaluate_trade_position_impact},
    {"trade.settlement", umi_financial_platform_evaluate_trade_settlement},
    {"workflow.approval", umi_financial_platform_evaluate_workflow_approval},
    {"workflow.completion", umi_financial_platform_evaluate_workflow_completion},
    {"workflow.escalation", umi_financial_platform_evaluate_workflow_escalation},
    {"workflow.role-coverage", umi_financial_platform_evaluate_workflow_role_coverage},
    {"workflow.rollback", umi_financial_platform_evaluate_workflow_rollback},
    {"workflow.state-transition", umi_financial_platform_evaluate_workflow_state_transition},
    {"workflow.task-coverage", umi_financial_platform_evaluate_workflow_task_coverage},
    {"workflow.timeout", umi_financial_platform_evaluate_workflow_timeout},
};

size_t umi_financial_platform_service_count(void)
{
    return sizeof(CATALOGUE) / sizeof(CATALOGUE[0]);
}

UmiStatus umi_financial_platform_service_id_at(
    size_t index,
    const char **outId)
{
    if (outId == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outId = NULL;
    if (index >= umi_financial_platform_service_count()) {
        return UMI_STATUS_NOT_FOUND;
    }
    *outId = CATALOGUE[index].id;
    return UMI_STATUS_OK;
}

UmiStatus umi_financial_platform_service_evaluate(
    const char *id,
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    size_t index;

    if (id == NULL || id[0] == '\0' || input == NULL ||
        outSnapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < umi_financial_platform_service_count(); ++index) {
        if (strcmp(CATALOGUE[index].id, id) == 0) {
            return CATALOGUE[index].evaluator(input, outSnapshot);
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_financial_platform_service_snapshot(
    const UmiFinancialControlService *service,
    UmiFinancialPlatformServiceSnapshot *outSnapshot)
{
    UmiFinancialControlSnapshot control;
    UmiStatus status;

    if (service == NULL || outSnapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(outSnapshot, 0, sizeof(*outSnapshot));
    status = umi_financial_control_service_snapshot(service, &control);
    if (status != UMI_STATUS_OK) return status;

    outSnapshot->revision = control.revision;
    outSnapshot->policyCount = umi_financial_platform_service_count();
    outSnapshot->rateCount = control.rateCount;
    outSnapshot->relationshipCount = control.relationshipCount;
    outSnapshot->instrumentCount = control.instrumentCount;
    outSnapshot->tradeCount = control.tradeCount;
    outSnapshot->positionCount = control.positionCount;
    outSnapshot->paymentCount = control.paymentCount;
    outSnapshot->settlementCount = control.settlementCount;
    outSnapshot->auditCount = control.auditCount;
    outSnapshot->auditGapCount = control.auditGapCount;
    outSnapshot->persistedAuditCount = control.persistedAuditCount;
    outSnapshot->referenceDataReady =
        control.rateCount > 0U &&
        control.relationshipCount > 0U &&
        control.instrumentCount > 0U;
    outSnapshot->lifecycleReady =
        control.tradeCount > 0U ||
        control.paymentCount > 0U ||
        control.settlementCount > 0U;
    outSnapshot->auditReady = control.auditReady;
    return UMI_STATUS_OK;
}
