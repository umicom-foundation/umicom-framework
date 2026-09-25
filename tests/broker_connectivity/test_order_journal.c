/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/broker_connectivity/test_order_journal.c
 *
 * PURPOSE:
 *   Verify idempotent broker order retention and monotonic updates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/broker_connectivity/order_journal.h"

int main(void)
{
    UmiBrokerOrderJournal journal;
    UmiBrokerOrderRecord record = {0};

    umi_broker_order_journal_init(&journal);
    (void)strcpy(record.request.client_order_id.value, "order-1");
    record.status = UMI_ORDER_NEW;
    record.providerSequence = 1U;
    assert(umi_broker_order_journal_add(&journal, &record) == UMI_STATUS_OK);
    assert(umi_broker_order_journal_add(&journal, &record) ==
           UMI_STATUS_ALREADY_EXISTS);
    assert(umi_broker_order_journal_update(
               &journal, "order-1", UMI_ORDER_ACCEPTED, 2U, 1000U) ==
           UMI_STATUS_OK);
    assert(umi_broker_order_journal_find(&journal, "order-1")->status ==
           UMI_ORDER_ACCEPTED);
    return 0;
}
