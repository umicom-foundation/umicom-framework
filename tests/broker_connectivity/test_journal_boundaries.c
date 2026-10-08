/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/broker_connectivity/test_journal_boundaries.c
 * PURPOSE: Verify broker journal replay, identity and terminal-state mutations are bounded and atomic.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/order_journal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%d: %s\n", __LINE__, #x);                                                       \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
static int Run(UmiBrokerOrderJournal *journal, UmiBrokerOrderJournal *before, const char *mode)
{
    umi_broker_order_journal_init(journal);
    UmiBrokerOrderRecord record = {0};
    strcpy(record.request.client_order_id.value, "order-1");
    record.providerSequence = 1U;
    record.submittedMilliseconds = 10U;
    record.updatedMilliseconds = 10U;
    CHECK(umi_broker_order_journal_add(journal, &record) == UMI_STATUS_OK);
    CHECK(umi_broker_order_journal_update(journal, "order-1", UMI_ORDER_ACCEPTED, 2U, 20U) == UMI_STATUS_OK);
    UmiStatus expected = UMI_STATUS_INVALID_STATE, status;
    UmiOrderStatus next = UMI_ORDER_PARTIALLY_FILLED;
    uint64_t sequence = 3U, time = 30U;
    if (!strcmp(mode, "replay"))
    {
        sequence = 2U;
        time = 20U;
        next = UMI_ORDER_ACCEPTED;
        expected = UMI_STATUS_OK;
    }
    else if (!strcmp(mode, "same-sequence"))
        sequence = 2U;
    else if (!strcmp(mode, "same-sequence-time"))
    {
        sequence = 2U;
        next = UMI_ORDER_ACCEPTED;
    }
    else if (!strcmp(mode, "old-sequence"))
        sequence = 1U;
    else if (!strcmp(mode, "old-time"))
        time = 19U;
    else if (!strcmp(mode, "regression"))
        next = UMI_ORDER_NEW;
    else if (!strcmp(mode, "terminal"))
    {
        CHECK(umi_broker_order_journal_update(journal, "order-1", UMI_ORDER_FILLED, 3U, 30U) ==
              UMI_STATUS_OK);
        sequence = 4U;
        time = 40U;
    }
    else if (!strcmp(mode, "unknown-status"))
    {
        next = (UmiOrderStatus)99;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (!strcmp(mode, "revision"))
    {
        journal->revision = UINT64_MAX;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    else if (!strcmp(mode, "replay-at-capacity"))
    {
        journal->revision = UINT64_MAX;
        sequence = 2U;
        time = 20U;
        next = UMI_ORDER_ACCEPTED;
        expected = UMI_STATUS_OK;
    }
    else if (!strcmp(mode, "count"))
        journal->count = UMI_BROKER_ORDER_JOURNAL_CAPACITY + 1U;
    else if (!strcmp(mode, "stored-id"))
        memset(journal->records[0].request.client_order_id.value, 'x',
               sizeof journal->records[0].request.client_order_id.value);
    else if (!strcmp(mode, "provider-id"))
        memset(journal->records[0].providerOrderId, 'x', sizeof journal->records[0].providerOrderId);
    else if (!strcmp(mode, "duplicate-stored"))
    {
        journal->records[1] = journal->records[0];
        journal->count = 2U;
    }
    else if (!strcmp(mode, "missing"))
        expected = UMI_STATUS_NOT_FOUND;
    else if (!strcmp(mode, "valid"))
        expected = UMI_STATUS_OK;
    else
        return 2;
    *before = *journal;
    status = umi_broker_order_journal_update(journal, !strcmp(mode, "missing") ? "other-order" : "order-1",
                                             next, sequence, time);
    CHECK(status == expected);
    if (!strcmp(mode, "valid"))
    {
        const UmiBrokerOrderRecord *found = umi_broker_order_journal_find(journal, "order-1");
        CHECK(found != NULL && found->status == UMI_ORDER_PARTIALLY_FILLED);
        CHECK(journal->revision == before->revision + 1U);
    }
    else
        CHECK(memcmp(journal, before, sizeof *journal) == 0);
    if (!strcmp(mode, "count") || !strcmp(mode, "stored-id") || !strcmp(mode, "provider-id") ||
        !strcmp(mode, "duplicate-stored"))
        CHECK(umi_broker_order_journal_find(journal, "order-1") == NULL);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    UmiBrokerOrderJournal *journal = calloc(1, sizeof *journal), *before = calloc(1, sizeof *before);
    if (journal == NULL || before == NULL)
    {
        free(journal);
        free(before);
        return 1;
    }
    int result = Run(journal, before, argv[1]);
    free(journal);
    free(before);
    return result;
}
