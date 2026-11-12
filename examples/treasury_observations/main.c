/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/treasury_observations/main.c
 *
 * PURPOSE:
 *   Build a thin Treasury observer from Framework owners and handle refused snapshots.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/open_tms/service.h"
#include "umicom/financial_platform/financial_control_service.h"
#include "umicom/financial_platform/currency_rate_book.h"
#include "umicom/financial_platform/financial_audit_journal.h"
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Keep policy and arithmetic in Framework. The application owns storage,
 * checks each status, and chooses whether to replace its displayed observation. */
/* Rates and audit flags remain Framework observations. This scenario performs
 * no filesystem write, bank operation or external data request. */
static int ObserveControls(void)
{
    UmiFinancialRateBook rates;
    UmiFinancialRateRecord rate = {"GBP", "USD", 1.25, 1U};
    UmiFinancialAuditJournal *journal = malloc(sizeof(*journal));
    if (journal == NULL) return 1;
    umi_financial_rate_book_init(&rates);
    umi_financial_audit_journal_init(journal);
    double converted = 0.0;
    UmiStatus status = umi_financial_rate_book_upsert(&rates, &rate);
    if (status == UMI_STATUS_OK) status = umi_financial_rate_book_convert(&rates, "GBP", "USD", -100.0, &converted);
    if (status != UMI_STATUS_OK) goto done;
    puts("Umicom financial observations - local practice");
    printf("Signed preview conversion: GBP -100.00 -> USD %.2f\n", converted);
    if (converted != -125.0) { status = UMI_STATUS_INTERNAL_ERROR; goto done; }
    rate.rate = DBL_MAX;
    status = umi_financial_rate_book_upsert(&rates, &rate);
    if (status != UMI_STATUS_OK) goto done;
    status = umi_financial_rate_book_convert(&rates, "GBP", "USD", 2.0, &converted);
    if (status != UMI_STATUS_CAPACITY_EXCEEDED || converted != -125.0) {
        status = UMI_STATUS_INTERNAL_ERROR; goto done;
    }
    puts("Overflow refused; previous conversion preserved.");
    UmiFinancialAuditRecord event = {0};
    event.sequence = 41;
    strcpy(event.eventId, "practice.event"); strcpy(event.lineageId, "practice.flow");
    strcpy(event.actorId, "learner"); strcpy(event.domain, "currency");
    strcpy(event.entityId, "GBPUSD"); strcpy(event.action, "observe");
    event.status = UMI_STATUS_CAPACITY_EXCEEDED;
    status = umi_financial_audit_journal_append(journal, &event);
    event.sequence = 43;
    if (status == UMI_STATUS_OK) status = umi_financial_audit_journal_append(journal, &event);
    size_t persisted = 0;
    if (status == UMI_STATUS_OK) status = umi_financial_audit_journal_persisted_count_checked(journal, &persisted);
    if (status != UMI_STATUS_OK) goto done;
    if (journal->gapCount != 1 || persisted != 0) { status = UMI_STATUS_INTERNAL_ERROR; goto done; }
    puts("Observed sequences 41 and 43: one gap inside the observed interval.");
    status = umi_financial_audit_journal_mark_persisted(journal, 41);
    if (status == UMI_STATUS_OK) status = umi_financial_audit_journal_persisted_count_checked(journal, &persisted);
    if (status == UMI_STATUS_OK && persisted != 1) status = UMI_STATUS_INTERNAL_ERROR;
    if (status == UMI_STATUS_OK) puts("One local persisted flag is set; no durable storage was written or verified.");
done:
    free(journal);
    if (status != UMI_STATUS_OK) fprintf(stderr, "Control observation stopped with status %d.\n", (int)status);
    return status == UMI_STATUS_OK ? 0 : 1;
}

/* The shared financial control plane keeps references and local lifecycle
 * observations. This learner-owned fixture neither resolves canonical records
 * nor submits a transaction; application-specific I/O belongs outside it. */
static int ObserveLifecycle(void)
{
    UmiFinancialControlService *service = malloc(sizeof(*service));
    if (service == NULL) return 1;
    umi_financial_control_service_init(service);
    UmiFinancialRelationship relationship = {"practice.link", "practice.party", "", "practice.account", "owner", 1};
    UmiFinancialInstrumentLink instrument = {"practice.bond", "practice.product", "GBP", "", 1U, 1};
    UmiFinancialTradeControl trade = {"practice.trade", "practice.account", "practice.party", "practice.bond", "GBP", 2, 100, 200, UMI_FINANCIAL_TRADE_DRAFT, 1U};
    UmiFinancialPositionControl position = {"practice.account", "practice.bond", "GBP", 2, 90, 100, 0, 1U};
    UmiFinancialPaymentControl payment = {"practice.payment", "practice.account", "practice.party", "GBP", 200, UMI_FINANCIAL_PAYMENT_DRAFT, 1U, 0};
    UmiFinancialSettlementControl settlement = {"practice.settlement", "practice.payment", "practice.account", "GBP", -200, 2, 1000U, UMI_FINANCIAL_SETTLEMENT_PENDING, 1U};
    UmiStatus status = umi_financial_relationship_store_add(&service->relationships, &relationship);
    if (status == UMI_STATUS_OK) status = umi_financial_instrument_link_store_upsert(&service->instruments, &instrument);
    if (status == UMI_STATUS_OK) status = umi_financial_trade_control_book_add(&service->trades, &trade);
    if (status == UMI_STATUS_OK) status = umi_financial_position_control_book_upsert(&service->positions, &position);
    if (status == UMI_STATUS_OK) status = umi_financial_payment_control_book_add(&service->payments, &payment);
    if (status == UMI_STATUS_OK) status = umi_financial_settlement_book_add(&service->settlements, &settlement);
    UmiFinancialControlSnapshot display = {0};
    if (status == UMI_STATUS_OK) status = umi_financial_control_service_snapshot(service, &display);
    if (status != UMI_STATUS_OK) goto done;
    size_t links = 0, overdue = 0;
    double marketValue = 0;
    status = umi_financial_relationship_store_count_for_account_checked(&service->relationships, "practice.account", &links);
    if (status == UMI_STATUS_OK) status = umi_financial_position_control_book_market_value_checked(&service->positions, "practice.account", &marketValue);
    if (status == UMI_STATUS_OK) status = umi_financial_settlement_book_overdue_count_checked(&service->settlements, 1001U, &overdue);
    if (status != UMI_STATUS_OK) goto done;
    if (links != 1 || marketValue != 200 || overdue != 1 || display.tradeCount != 1) {
        status = UMI_STATUS_INTERNAL_ERROR; goto done;
    }
    puts("Umicom financial lifecycle - local practice");
    printf("Account links: %zu; GBP market value: %.2f; overdue obligations: %zu\n", links, marketValue, overdue);
    status = umi_financial_payment_control_book_transition(&service->payments, "practice.payment", UMI_FINANCIAL_PAYMENT_SENT, 2U);
    if (status != UMI_STATUS_PERMISSION_DENIED || service->payments.payments[0].version != 1U) {
        status = UMI_STATUS_INTERNAL_ERROR; goto done;
    }
    puts("Unapproved payment transition refused; local version retained.");
    status = umi_financial_trade_control_book_transition(&service->trades, "practice.trade", UMI_FINANCIAL_TRADE_CANCELLED, 2U);
    if (status != UMI_STATUS_OK) goto done;
    status = umi_financial_trade_control_book_transition(&service->trades, "practice.trade", UMI_FINANCIAL_TRADE_DRAFT, 3U);
    if (status != UMI_STATUS_INVALID_STATE) { status = UMI_STATUS_INTERNAL_ERROR; goto done; }
    puts("Cancelled trade observation remains terminal.");
    UmiFinancialControlSnapshot before;
    memcpy(&before, &display, sizeof display);
    /* Deliberately corrupt a restored flag to show all-or-nothing capture.
     * Keep the existing display as stale evidence rather than a new success. */
    service->instruments.links[0].active = 2;
    status = umi_financial_control_service_snapshot(service, &display);
    if (status == UMI_STATUS_OK || memcmp(&before, &display, sizeof display) != 0) {
        status = UMI_STATUS_INTERNAL_ERROR; goto done;
    }
    puts("Malformed instrument observation refused; prior display retained as stale.");
    status = UMI_STATUS_OK;
done:
    free(service);
    if (status != UMI_STATUS_OK) fprintf(stderr, "Lifecycle observation stopped with status %d.\n", (int)status);
    return status == UMI_STATUS_OK ? 0 : 1;
}

int main(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "lifecycle") == 0) return ObserveLifecycle();
    if (argc == 2 && strcmp(argv[1], "controls") == 0) return ObserveControls();
    if (argc != 2 || (strcmp(argv[1], "preview") != 0 &&
        strcmp(argv[1], "recovery") != 0 && strcmp(argv[1], "reconciliation") != 0)) {
        fputs("Usage: treasury-observations preview|recovery|reconciliation|controls|lifecycle\n", stderr);
        return 2;
    }
    UmiOpenTmsService *service = malloc(sizeof(*service));
    if (service == NULL) return 1;
    umi_open_tms_service_init(service);
    UmiOpenTmsCashRecord cash = {"practice", "GBP", 1000, 1000, 200, 500, 850, 1U};
    UmiOpenTmsPosition position = {"practice", "training-bond", "GBP", 2, 90, 100, 0, 1U};
    UmiOpenTmsServiceSnapshot display = {0};
    UmiStatus status = umi_open_tms_cash_book_upsert(&service->cash, &cash);
    if (status == UMI_STATUS_OK) status = umi_open_tms_position_book_upsert(&service->positions, &position);
    if (status == UMI_STATUS_OK) status = umi_open_tms_service_snapshot(service, 0, &display);
    if (status != UMI_STATUS_OK) goto done;
    puts("Umicom Treasury observations - local practice");
    printf("GBP cash headroom: %.2f; GBP gross exposure: %.2f\n",
        display.dashboard.cashHeadroom, display.dashboard.grossExposure);
    if (display.dashboard.cashHeadroom != 450 || display.dashboard.grossExposure != 200) {
        status = UMI_STATUS_INTERNAL_ERROR; goto done;
    }
    if (strcmp(argv[1], "recovery") == 0) {
        UmiOpenTmsServiceSnapshot before;
        memcpy(&before, &display, sizeof display);
        /* Simulate malformed restored storage without reading outside its array.
         * The owner refuses the snapshot; the display remains the previous one. */
        service->settlements.count = SIZE_MAX;
        status = umi_open_tms_service_snapshot(service, 0, &display);
        if (status == UMI_STATUS_OK || memcmp(&before, &display, sizeof display) != 0) {
            status = UMI_STATUS_INTERNAL_ERROR; goto done;
        }
        puts("Rejected malformed settlement count; previous display retained.");
        umi_open_tms_settlement_book_init(&service->settlements);
        status = umi_open_tms_service_snapshot(service, 0, &display);
        if (status == UMI_STATUS_OK) puts("Corrected owner captured successfully.");
    } else if (strcmp(argv[1], "reconciliation") == 0) {
        UmiOpenTmsReconciliationItem item = {"statement.1", "book", "statement", 100, 102, 0.5, 0, 0};
        status = umi_open_tms_reconciliation_book_add(&service->reconciliation, &item);
        if (status == UMI_STATUS_OK) status = umi_open_tms_service_snapshot(service, 0, &display);
        if (status != UMI_STATUS_OK) goto done;
        printf("Unresolved breaks: %zu\n", display.dashboard.reconciliationBreaks);
        if (display.dashboard.reconciliationBreaks != 1) { status = UMI_STATUS_INTERNAL_ERROR; goto done; }
        /* Resolution records a local review decision. It sends no payment and
         * changes neither of the original observed amounts. */
        status = umi_open_tms_reconciliation_book_resolve(&service->reconciliation, "statement.1");
        if (status == UMI_STATUS_OK) status = umi_open_tms_service_snapshot(service, 0, &display);
        if (status == UMI_STATUS_OK && display.dashboard.reconciliationBreaks != 0) status = UMI_STATUS_INTERNAL_ERROR;
        if (status == UMI_STATUS_OK) puts("Break resolved locally; original observations retained.");
    }
    if (status == UMI_STATUS_OK) puts("Coverage flags are observations; they do not authorize moving money.");
done:
    free(service);
    if (status != UMI_STATUS_OK) fprintf(stderr, "Observation stopped with status %d.\n", (int)status);
    return status == UMI_STATUS_OK ? 0 : 1;
}
