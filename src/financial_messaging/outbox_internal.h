/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_messaging/outbox_internal.h
 *
 * PURPOSE:
 *   Share bounded storage and transition rules inside the outgoing journal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UFMP_OUTBOX_INTERNAL_H
#define UMICOM_UFMP_OUTBOX_INTERNAL_H
#include "inbox_internal.h"
#include "umicom/financial_messaging/outbox.h"
#define UMI_UFMP_OUTBOX_STATE_CAPACITY 256U
int UmiUfmpRetryValid(const UmiUfmpRetryPolicy *policy);
int UmiUfmpOutboxRecordValid(const UmiUfmpOutboxRecord *record);
UmiStatus UmiUfmpOutboxStateEncode(const UmiUfmpOutboxRecord *record, char out[256]);
UmiStatus UmiUfmpOutboxStateDecode(char text[256], UmiUfmpOutboxRecord *record);
UmiStatus UmiUfmpOutboxApply(UmiUfmpOutboxRecord *record, uint64_t now,
  UmiUfmpOutboxEvent event);
#endif
