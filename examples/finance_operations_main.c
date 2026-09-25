/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/finance_operations_main.c
 *
 * PURPOSE:
 *   Walk through a complete in-memory order-to-ledger exercise using only public Framework operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance_operations/input.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#ifndef UMICOM_FINANCE_EXAMPLE_TITLE
#define UMICOM_FINANCE_EXAMPLE_TITLE "Umicom financial operations"
#endif
static UmiStatus SetId(UmiFinancialId *id, const char *text)
{
    size_t length = strlen(text);
    if (length == 0U || length >= sizeof(id->value)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(id, 0, sizeof(*id)); memcpy(id->value, text, length); return UMI_STATUS_OK;
}
static UmiStatus Submit(UmiFinanceOperations *operations, UmiFinanceOperationCommand *command,
    UmiFinanceOperationKind kind, const char *id, const char *actor)
{
    UmiFinanceOperationCounts counts;
    UmiFinanceOperationReceipt receipt;
    UmiStatus status = UmiFinanceOperationsCounts(operations, &counts);
    char request[48];
    if (status != UMI_STATUS_OK) return status;
    command->kind = kind; command->expectedRevision = counts.revision;
    (void)snprintf(request, sizeof(request), "practice.%" PRIu64, counts.revision + 1U);
    status = SetId(&command->requestId, request);
    if (status == UMI_STATUS_OK) status = SetId(&command->id, id);
    if (status == UMI_STATUS_OK) status = SetId(&command->actorId, actor);
    if (status == UMI_STATUS_OK) status = UmiFinanceOperationsApply(operations, command, &receipt);
    if (status == UMI_STATUS_OK) printf("%02" PRIu64 "  %-34s %s\n", receipt.revision, UmiFinanceOperationKindText(kind), id);
    else fprintf(stderr, "%s: %s (status %d)\n", UmiFinanceOperationKindText(kind), UmiFinanceOperationsExplainStatus(status), (int)status);
    UmiFinanceOperationCommandInit(command);
    return status;
}
#define STEP(kind, id, actor) do { status = Submit(operations, &command, kind, id, actor); if (status != UMI_STATUS_OK) goto done; } while (0)
#define ID(field, text) do { status = SetId(&(field), text); if (status != UMI_STATUS_OK) goto done; } while (0)
int main(void)
{
    UmiDataServer *server = NULL;
    UmiFinanceOperations *operations = NULL;
    UmiFinanceOperationCommand command;
    UmiFinanceTrialBalance trial;
    UmiFinanceOperationAvailability cash;
    UmiStatus status;
    const UmiCurrency currency = {{'G','B','P','\0'}};
    const UmiFinancialDate day = {2026,9U,25U};
    const char *accounts[] = {"control", "buyer.cash", "seller.cash"};
    int64_t balance;
    char amount[64];
    puts(UMICOM_FINANCE_EXAMPLE_TITLE " — local practice, memory only");
    puts("No real funds, broker, payment network or personal records are used.\n");
    status = umi_data_server_create_memory(&server);
    if (status == UMI_STATUS_OK) status = UmiFinanceOperationsCreate(server, &operations);
    if (status != UMI_STATUS_OK) goto done;
    UmiFinanceOperationCommandInit(&command);
    for (size_t index = 0U; index < 3U; ++index) {
        (void)snprintf(command.name, sizeof(command.name), "%s practice account", accounts[index]);
        command.currency = currency; command.scale = 2U;
        command.accountClass = index == 0U ? UMI_ACCOUNTING_ASSET : UMI_ACCOUNTING_LIABILITY;
        STEP(UMI_FINANCE_CREATE_ACCOUNT, accounts[index], "operator");
    }
    command.date = (UmiFinancialDate){2026,9U,1U}; command.endDate = (UmiFinancialDate){2026,9U,30U};
    STEP(UMI_FINANCE_OPEN_PERIOD, "september", "operator");
    command.date = day; ID(command.referenceId, "september");
    status = UmiFinanceOperationsParseLines("control,50000,0\nbuyer.cash,0,50000", command.lines,
        UMI_FINANCE_OPERATIONS_JOURNAL_LINES, &command.lineCount);
    if (status != UMI_STATUS_OK) goto done;
    STEP(UMI_FINANCE_ENTER_JOURNAL, "funding", "maker");
    STEP(UMI_FINANCE_APPROVE_JOURNAL, "funding", "checker");
    STEP(UMI_FINANCE_POST_JOURNAL, "funding", "checker");
    strcpy(command.name, "Practice buyer"); ID(command.accountId, "buyer.cash");
    STEP(UMI_FINANCE_REGISTER_PARTICIPANT, "buyer", "operator");
    strcpy(command.name, "Practice seller"); ID(command.accountId, "seller.cash");
    STEP(UMI_FINANCE_REGISTER_PARTICIPANT, "seller", "operator");
    strcpy(command.name, "Practice instrument"); command.currency = currency; command.scale = 2U;
    command.minorPerTick = 1; command.unitsPerLot = 1; command.maxOrderLots = 100;
    STEP(UMI_FINANCE_LIST_INSTRUMENT, "practice", "operator");
    command.enabled = true; STEP(UMI_FINANCE_SET_MARKET_STATE, "practice", "operator");
    ID(command.instrumentId, "practice"); command.lots = 100;
    STEP(UMI_FINANCE_DEPOSIT_LOTS, "seller", "custodian");
    ID(command.participantId, "seller"); ID(command.instrumentId, "practice");
    command.side = UMI_SIDE_SELL; command.priceTicks = 1000; command.lots = 10; command.date = day;
    STEP(UMI_FINANCE_PLACE_ORDER, "sell.1", "trader");
    ID(command.participantId, "buyer"); ID(command.instrumentId, "practice");
    command.side = UMI_SIDE_BUY; command.priceTicks = 1100; command.lots = 6; command.date = day;
    STEP(UMI_FINANCE_PLACE_ORDER, "buy.1", "trader");
    status = UmiFinanceOperationsCash(operations, "buyer", &cash);
    if (status != UMI_STATUS_OK || cash.reserved != 6000 || cash.available != 44000) { status = UMI_STATUS_INTERNAL_ERROR; goto done; }
    puts("    The fill reserves 6000 GBP minor units; no cash has been posted yet.");
    STEP(UMI_FINANCE_CLEAR_FILL, "system.fill.000001", "clearing");
    ID(command.periodId, "september"); command.date = day;
    STEP(UMI_FINANCE_SETTLE_FILL, "system.fill.000001", "settlement");
    ID(command.participantId, "seller"); STEP(UMI_FINANCE_CANCEL_ORDER, "sell.1", "trader");
    for (size_t index = 0U; index < 3U; ++index) {
        char evidence[48];
        status = UmiFinanceOperationsAccountBalance(operations, accounts[index], &balance);
        if (status != UMI_STATUS_OK) goto done;
        ID(command.accountId, accounts[index]); command.amountMinor = balance;
        (void)snprintf(evidence, sizeof(evidence), "reconciliation.%zu", index + 1U);
        STEP(UMI_FINANCE_RECONCILE_ACCOUNT, evidence, "reconciler");
    }
    STEP(UMI_FINANCE_PREPARE_CLOSE, "september", "closer");
    STEP(UMI_FINANCE_CLOSE_PERIOD, "september", "reviewer");
    status = UmiFinanceOperationsTrialBalance(operations, "september", currency, 2U, &trial);
    if (status != UMI_STATUS_OK || !trial.balanced) { status = UMI_STATUS_INTERNAL_ERROR; goto done; }
    for (size_t index = 0U; index < 3U; ++index) {
        status = UmiFinanceOperationsAccountBalance(operations, accounts[index], &balance);
        if (status == UMI_STATUS_OK) status = UmiFinanceOperationsFormatAmount(balance, 2U, amount, sizeof(amount));
        if (status != UMI_STATUS_OK) goto done;
        printf("%-15s GBP %s\n", accounts[index], amount);
    }
    puts("Trial balance agrees. The period is closed. Practice complete.");
done:
    UmiFinanceOperationsDestroy(operations); umi_data_server_destroy(server);
    if (status != UMI_STATUS_OK) fprintf(stderr, "Practice stopped with status %d.\n", (int)status);
    return status == UMI_STATUS_OK ? 0 : 1;
}
