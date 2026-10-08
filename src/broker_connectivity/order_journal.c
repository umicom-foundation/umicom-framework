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
#include "umicom/trading/order_state.h"

void umi_broker_order_journal_init(UmiBrokerOrderJournal *journal)
{
    if (journal == NULL) return;
    (void)memset(journal, 0, sizeof(*journal));
    journal->revision = 1U;
}

/* The original journal operations are retained for review. The replacement
 * below adds bounded identity checks, atomic revision limits and replay/terminal
 * state guards so a late provider message cannot silently reopen a finished order. */
#if 0
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

#endif

/* Broker observations can skip intermediate local states, so this journal
 * keeps its existing direct NEW-to-ACCEPTED behavior. It validates identity,
 * sequence and terminal-state ownership before changing any retained record. */
static int JournalText(const char *text,size_t capacity,int allowEmpty)
{
    if(text==NULL || capacity==0U) return 0;
    for(size_t i=0U;i<capacity;++i) if(text[i]=='\0') return allowEmpty || i!=0U;
    return 0;
}
static int JournalRecord(const UmiBrokerOrderRecord *record)
{
    return record!=NULL &&
        JournalText(record->request.client_order_id.value,sizeof record->request.client_order_id.value,0) &&
        JournalText(record->providerOrderId,sizeof record->providerOrderId,1) &&
        UmiOrderStatusValid(record->status) && record->updatedMilliseconds>=record->submittedMilliseconds;
}
static int JournalValid(const UmiBrokerOrderJournal *journal)
{
    if(journal==NULL || journal->count>UMI_BROKER_ORDER_JOURNAL_CAPACITY || journal->revision==0U) return 0;
    for(size_t i=0U;i<journal->count;++i) {
        if(!JournalRecord(&journal->records[i])) return 0;
        for(size_t j=0U;j<i;++j)
            if(strcmp(journal->records[i].request.client_order_id.value,
                journal->records[j].request.client_order_id.value)==0) return 0;
    }
    return 1;
}
const UmiBrokerOrderRecord *umi_broker_order_journal_find(
    const UmiBrokerOrderJournal *journal,const char *clientOrderId)
{
    if(!JournalText(clientOrderId,sizeof journal->records[0].request.client_order_id.value,0) ||
        !JournalValid(journal)) return NULL;
    for(size_t i=0U;i<journal->count;++i)
        if(strcmp(journal->records[i].request.client_order_id.value,clientOrderId)==0)
            return &journal->records[i];
    return NULL;
}
UmiStatus umi_broker_order_journal_add(UmiBrokerOrderJournal *journal,const UmiBrokerOrderRecord *record)
{
    if(journal==NULL || !JournalRecord(record)) return UMI_STATUS_INVALID_ARGUMENT;
    if(!JournalValid(journal)) return UMI_STATUS_INVALID_STATE;
    if(umi_broker_order_journal_find(journal,record->request.client_order_id.value)!=NULL)
        return UMI_STATUS_ALREADY_EXISTS;
    if(journal->count==UMI_BROKER_ORDER_JOURNAL_CAPACITY || journal->revision==UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    journal->records[journal->count++]=*record;++journal->revision;return UMI_STATUS_OK;
}
UmiStatus umi_broker_order_journal_update(UmiBrokerOrderJournal *journal,const char *clientOrderId,
    UmiOrderStatus status,uint64_t sequence,uint64_t updated)
{
    if(journal==NULL ||
        !JournalText(clientOrderId,sizeof journal->records[0].request.client_order_id.value,0) ||
        !UmiOrderStatusValid(status)) return UMI_STATUS_INVALID_ARGUMENT;
    if(!JournalValid(journal)) return UMI_STATUS_INVALID_STATE;
    for(size_t i=0U;i<journal->count;++i) {
        UmiBrokerOrderRecord *record=&journal->records[i];
        if(strcmp(record->request.client_order_id.value,clientOrderId)!=0) continue;
        /* Exact replay is a no-op even at revision capacity. A changed state or
         * timestamp on the same provider sequence cannot overwrite its meaning. */
        if(sequence==record->providerSequence)
            return status==record->status && updated==record->updatedMilliseconds?
                UMI_STATUS_OK:UMI_STATUS_INVALID_STATE;
        if(sequence<record->providerSequence || updated<record->updatedMilliseconds)
            return UMI_STATUS_INVALID_STATE;
        if((record->status==UMI_ORDER_FILLED || record->status==UMI_ORDER_CANCELLED ||
            record->status==UMI_ORDER_REJECTED) && status!=record->status)
            return UMI_STATUS_INVALID_STATE;
        if((record->status==UMI_ORDER_VALIDATED && status==UMI_ORDER_NEW) ||
            (record->status==UMI_ORDER_ACCEPTED && (status==UMI_ORDER_NEW || status==UMI_ORDER_VALIDATED)) ||
            (record->status==UMI_ORDER_PARTIALLY_FILLED && (status==UMI_ORDER_NEW ||
                status==UMI_ORDER_VALIDATED || status==UMI_ORDER_ACCEPTED)))
            return UMI_STATUS_INVALID_STATE;
        if(journal->revision==UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
        record->status=status;record->providerSequence=sequence;record->updatedMilliseconds=updated;
        ++journal->revision;return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}
