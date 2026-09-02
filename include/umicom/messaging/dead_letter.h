/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/dead_letter.h
 *
 * PURPOSE:
 *   Declare retained dead-letter records containing the failed message, status, reason, attempts and failure timestamp.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the dead letter store data shared with callers of this public contract.
 */
typedef struct UmiDeadLetterStore UmiDeadLetterStore;

/**
 * Represent the dead letter view data shared with callers of this public contract.
 */
typedef struct UmiDeadLetterView {
    uint64_t dead_letter_id;
    const UmiMessageEnvelope *message;
    UmiStatus status;
    const char *reason;
    uint32_t attempts;
    uint64_t failed_at_nanoseconds;
} UmiDeadLetterView;

/**
 * Initialise dead letter store from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_dead_letter_store_create(size_t capacity,
                                       UmiDeadLetterStore **out_store);
/**
 * Release or reset state held by dead letter store so the same storage can be reused
 * safely.
 */
void umi_dead_letter_store_destroy(UmiDeadLetterStore *store);
/**
 * Add dead letter store only after its inputs and available capacity have been checked.
 */
UmiStatus umi_dead_letter_store_add(UmiDeadLetterStore *store,
                                    const UmiMessageEnvelope *message,
                                    UmiStatus status,
                                    const char *reason,
                                    uint32_t attempts,
                                    uint64_t failed_at_nanoseconds,
                                    uint64_t *out_dead_letter_id);
/**
 * Provide the dead letter store get operation used by this module and its client
 * applications.
 */
UmiStatus umi_dead_letter_store_get(const UmiDeadLetterStore *store,
                                    size_t index,
                                    UmiDeadLetterView *out_record);
/**
 * Remove dead letter store while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_dead_letter_store_remove(UmiDeadLetterStore *store,
                                       uint64_t dead_letter_id);
/**
 * Return the number of records represented by dead letter store without changing their
 * state.
 */
size_t umi_dead_letter_store_count(const UmiDeadLetterStore *store);

#ifdef __cplusplus
}
#endif

#endif
