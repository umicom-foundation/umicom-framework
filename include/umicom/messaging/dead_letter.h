/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/dead_letter.h
 *
 * PURPOSE:
 *   Declare retained dead-letter records containing the failed message, status, reason, attempts and failure timestamp.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MESSAGING_DEAD_LETTER_H
#define UMICOM_MESSAGING_DEAD_LETTER_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/messaging/message.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeadLetterStore UmiDeadLetterStore;

typedef struct UmiDeadLetterView {
    uint64_t dead_letter_id;
    const UmiMessageEnvelope *message;
    UmiStatus status;
    const char *reason;
    uint32_t attempts;
    uint64_t failed_at_nanoseconds;
} UmiDeadLetterView;

UmiStatus umi_dead_letter_store_create(size_t capacity,
                                       UmiDeadLetterStore **out_store);
void umi_dead_letter_store_destroy(UmiDeadLetterStore *store);
UmiStatus umi_dead_letter_store_add(UmiDeadLetterStore *store,
                                    const UmiMessageEnvelope *message,
                                    UmiStatus status,
                                    const char *reason,
                                    uint32_t attempts,
                                    uint64_t failed_at_nanoseconds,
                                    uint64_t *out_dead_letter_id);
UmiStatus umi_dead_letter_store_get(const UmiDeadLetterStore *store,
                                    size_t index,
                                    UmiDeadLetterView *out_record);
UmiStatus umi_dead_letter_store_remove(UmiDeadLetterStore *store,
                                       uint64_t dead_letter_id);
size_t umi_dead_letter_store_count(const UmiDeadLetterStore *store);

#ifdef __cplusplus
}
#endif

#endif
