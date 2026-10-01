/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_operations/work_queue_private.h
 * PURPOSE: Own a bounded queue projection and the canonical events that gave it meaning.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BANK_WORK_QUEUE_PRIVATE_H
#define UMICOM_BANK_WORK_QUEUE_PRIVATE_H
#include "umicom/bank_operations/work_queue.h"
struct UmiBankWorkQueue {
    UmiBankWorkQueueSummary summary;
    UmiBankWorkQueueRow rows[UMI_BANK_WORK_QUEUE_CAPACITY];
    size_t eventCount;
    UmiBankAuditEvent *history;
};
#endif
