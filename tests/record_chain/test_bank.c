/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/record_chain/test_bank.c
 * PURPOSE: Qualify bank history commitments against real canonical service events.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/bank_operations/checkpoint.h"
#include "../../src/bank_operations/internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
static const unsigned char CHAIN[32] = {1U, 2U, 3U};
static void CreateCustomer(UmiBankOperations *operations, const char *name)
{
    UmiBankActor actor = {0}; UmiBankCommand command; UmiBankReceipt receipt;
    OK(umi_financial_id_assign(&actor.id, "maker")); actor.capabilities = UMI_BANK_CAP_ALL;
    UmiBankCommandInit(&command, UMI_BANK_CUSTOMER_CREATE);
    OK(umi_financial_id_assign(&command.requestId, "create.customer"));
    OK(umi_financial_id_assign(&command.id, "customer"));
    (void)snprintf(command.name, sizeof command.name, "%s", name);
    command.businessDate = (UmiFinancialDate){2026, 1U, 2U}; command.timestampMillis = 1;
    OK(UmiBankOperationsExecute(operations, &actor, &command, &receipt));
    CHECK(!receipt.idempotent);
    OK(UmiBankOperationsExecute(operations, &actor, &command, &receipt));
    CHECK(receipt.idempotent && receipt.revision == 1U);
}
int main(int argc, char **argv)
{
    UmiBankOperations *bank = NULL, *other = NULL;
    UmiRecordChainCheckpoint before, after, saved;
    UmiBankCounts counts;
    CHECK(argc == 2); OK(UmiBankOperationsOpenMemory(&bank));
    OK(UmiBankOperationsCheckpoint(bank, CHAIN, 0U, &before)); CHECK(before.sequence == 0U);
    CreateCustomer(bank, "Synthetic customer");
    OK(UmiBankOperationsCheckpoint(bank, CHAIN, 1U, &after)); CHECK(after.sequence == 1U);
    CHECK(!umi_record_chain_checkpoint_equal(&before, &after)); saved = after;
    if (!strcmp(argv[1], "history")) {
        OK(UmiBankOperationsCheckpoint(bank, CHAIN, 1U, &before));
        CHECK(umi_record_chain_checkpoint_equal(&before, &after));
        OK(UmiBankOperationsCounts(bank, &counts)); CHECK(counts.events == 1U && counts.revision == 1U);
    } else if (!strcmp(argv[1], "reload")) {
        OK(UmiBankOperationsReload(bank));
        OK(UmiBankOperationsCheckpoint(bank, CHAIN, 1U, &after));
        CHECK(umi_record_chain_checkpoint_equal(&saved, &after));
    } else if (!strcmp(argv[1], "different-payload")) {
        OK(UmiBankOperationsOpenMemory(&other)); CreateCustomer(other, "Different customer");
        OK(UmiBankOperationsCheckpoint(other, CHAIN, 1U, &before));
        CHECK(!umi_record_chain_checkpoint_equal(&before, &after));
    } else if (!strcmp(argv[1], "stale")) {
        CHECK(UmiBankOperationsCheckpoint(bank, CHAIN, 0U, &after) == UMI_STATUS_BUSY);
        CHECK(umi_record_chain_checkpoint_equal(&saved, &after));
    } else if (!strcmp(argv[1], "invalid-view")) {
        bank->poisoned = true;
        CHECK(UmiBankOperationsCheckpoint(bank, CHAIN, 1U, &after) == UMI_STATUS_INVALID_STATE);
        bank->poisoned = false;
        bank->state->events[0].revision = 2U;
        CHECK(UmiBankOperationsCheckpoint(bank, CHAIN, 1U, &after) == UMI_STATUS_INVALID_STATE);
        CHECK(umi_record_chain_checkpoint_equal(&saved, &after));
        OK(UmiBankOperationsReload(bank));
    } else if (!strcmp(argv[1], "invalid-input")) {
        unsigned char zero[32] = {0};
        CHECK(UmiBankOperationsCheckpoint(bank, zero, 1U, &after) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBankOperationsCheckpoint(NULL, CHAIN, 1U, &after) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_record_chain_checkpoint_equal(&saved, &after));
    } else return 2;
    UmiBankOperationsDestroy(other); UmiBankOperationsDestroy(bank); return 0;
}
