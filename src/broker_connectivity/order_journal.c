/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/broker_connectivity/order_journal.c
 *
 * PURPOSE:
 *   Implement idempotent broker order retention and monotonic state updates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/broker_connectivity/order_journal.h"

#include <string.h>

void umi_broker_order_journal_init(UmiBrokerOrderJournal *journal)
{
    if (journal == NULL) return;
    (void)memset(journal, 0, sizeof(*journal));
    journal->revision = 1U;
}

const UmiBrokerOrderRecord *umi_broker_order_journal_find(
    const UmiBrokerOrderJournal *journal,
    const char *clientOrderId)
{
    size_t index;
    if (journal == NULL || clientOrderId == NULL) return NULL;
    for (index = 0U; index < journal->count; ++index) {
        if (strcmp(journal->records[index].request.client_order_id.value,
                   clientOrderId) == 0) {
            return &journal->records[index];
        }
    }
    return NULL;
}

UmiStatus umi_broker_order_journal_add(
    UmiBrokerOrderJournal *journal,
    const UmiBrokerOrderRecord *record)
{
    if (journal == NULL || record == NULL ||
        record->request.client_order_id.value[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_broker_order_journal_find(
            journal, record->request.client_order_id.value) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (journal->count >= UMI_BROKER_ORDER_JOURNAL_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    journal->records[journal->count++] = *record;
    journal->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_broker_order_journal_update(
    UmiBrokerOrderJournal *journal,
    const char *clientOrderId,
    UmiOrderStatus status,
    uint64_t providerSequence,
    uint64_t updatedMilliseconds)
{
    size_t index;
    if (journal == NULL || clientOrderId == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < journal->count; ++index) {
        UmiBrokerOrderRecord *record = &journal->records[index];
        if (strcmp(record->request.client_order_id.value, clientOrderId) == 0) {
            if (providerSequence < record->providerSequence) {
                return UMI_STATUS_INVALID_STATE;
            }
            record->status = status;
            record->providerSequence = providerSequence;
            record->updatedMilliseconds = updatedMilliseconds;
            journal->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
