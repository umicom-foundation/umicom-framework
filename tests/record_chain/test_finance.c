/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/record_chain/test_finance.c
 * PURPOSE: Qualify financial event commitments and fail-closed view ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance_operations/checkpoint.h"
#include "../../src/finance_operations/internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
static const unsigned char CHAIN[32] = {4U, 5U, 6U};
static void CreateAccount(UmiFinanceOperations *operations, const char *name)
{
    UmiFinanceOperationCommand command; UmiFinanceOperationReceipt receipt;
    UmiFinanceOperationCommandInit(&command); command.kind = UMI_FINANCE_CREATE_ACCOUNT;
    OK(umi_financial_id_assign(&command.actorId, "maker"));
    OK(umi_financial_id_assign(&command.requestId, "create.account"));
    OK(umi_financial_id_assign(&command.id, "account"));
    (void)snprintf(command.name, sizeof command.name, "%s", name);
    command.currency = (UmiCurrency){{'G','B','P',0}}; command.scale = 2U;
    command.accountClass = UMI_ACCOUNTING_ASSET;
    OK(UmiFinanceOperationsApply(operations, &command, &receipt)); CHECK(!receipt.duplicate);
    OK(UmiFinanceOperationsApply(operations, &command, &receipt)); CHECK(receipt.duplicate);
}
int main(int argc, char **argv)
{
    UmiDataServer *server = NULL, *other_server = NULL;
    UmiFinanceOperations *operations = NULL, *other = NULL;
    UmiRecordChainCheckpoint before, after, saved;
    UmiFinanceOperationCounts counts;
    CHECK(argc == 2); OK(umi_data_server_create_memory(&server));
    OK(UmiFinanceOperationsCreate(server, &operations));
    OK(UmiFinanceOperationsCheckpoint(operations, CHAIN, 0U, &before)); CHECK(before.sequence == 0U);
    CreateAccount(operations, "Synthetic account");
    OK(UmiFinanceOperationsCheckpoint(operations, CHAIN, 1U, &after)); CHECK(after.sequence == 1U);
    CHECK(!umi_record_chain_checkpoint_equal(&before, &after)); saved = after;
    if (!strcmp(argv[1], "history")) {
        OK(UmiFinanceOperationsCheckpoint(operations, CHAIN, 1U, &before));
        CHECK(umi_record_chain_checkpoint_equal(&before, &after));
        OK(UmiFinanceOperationsCounts(operations, &counts)); CHECK(counts.events == 1U && counts.revision == 1U);
    } else if (!strcmp(argv[1], "reload")) {
        UmiFinanceOperationsDestroy(operations); operations = NULL;
        OK(UmiFinanceOperationsCreate(server, &operations));
        OK(UmiFinanceOperationsCheckpoint(operations, CHAIN, 1U, &after));
        CHECK(umi_record_chain_checkpoint_equal(&saved, &after));
    } else if (!strcmp(argv[1], "different-payload")) {
        OK(umi_data_server_create_memory(&other_server)); OK(UmiFinanceOperationsCreate(other_server, &other));
        CreateAccount(other, "Different account");
        OK(UmiFinanceOperationsCheckpoint(other, CHAIN, 1U, &before));
        CHECK(!umi_record_chain_checkpoint_equal(&before, &after));
    } else if (!strcmp(argv[1], "stale")) {
        CHECK(UmiFinanceOperationsCheckpoint(operations, CHAIN, 0U, &after) == UMI_STATUS_BUSY);
        CHECK(umi_record_chain_checkpoint_equal(&saved, &after));
    } else if (!strcmp(argv[1], "invalid-view")) {
        operations->writesBlocked = true;
        CHECK(UmiFinanceOperationsCheckpoint(operations, CHAIN, 1U, &after) == UMI_STATUS_INVALID_STATE);
        operations->writesBlocked = false; operations->poisoned = true;
        CHECK(UmiFinanceOperationsCheckpoint(operations, CHAIN, 1U, &after) == UMI_STATUS_INVALID_STATE);
        operations->poisoned = false; operations->state->commands[0].expectedRevision = 3U;
        CHECK(UmiFinanceOperationsCheckpoint(operations, CHAIN, 1U, &after) == UMI_STATUS_INVALID_STATE);
        CHECK(umi_record_chain_checkpoint_equal(&saved, &after));
        OK(UmiFinanceOperationsReload(operations));
    } else if (!strcmp(argv[1], "invalid-input")) {
        unsigned char zero[32] = {0};
        CHECK(UmiFinanceOperationsCheckpoint(operations, zero, 1U, &after) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiFinanceOperationsCheckpoint(NULL, CHAIN, 1U, &after) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_record_chain_checkpoint_equal(&saved, &after));
    } else return 2;
    UmiFinanceOperationsDestroy(other); umi_data_server_destroy(other_server);
    UmiFinanceOperationsDestroy(operations); umi_data_server_destroy(server); return 0;
}
