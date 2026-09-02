/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/resequencer.h
 *
 * PURPOSE:
 *   Declare a bounded resequencer that buffers out-of-order messages and releases the next expected sequence deterministically.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MESSAGING_RESEQUENCER_H
#define UMICOM_MESSAGING_RESEQUENCER_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/messaging/message.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the resequencer data shared with callers of this public contract.
 */
typedef struct UmiResequencer UmiResequencer;

/**
 * Initialise resequencer from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_resequencer_create(uint64_t first_sequence,
                                 size_t capacity,
                                 UmiResequencer **out_resequencer);
/**
 * Release or reset state held by resequencer so the same storage can be reused safely.
 */
void umi_resequencer_destroy(UmiResequencer *resequencer);
/**
 * Add resequencer only after its inputs and available capacity have been checked.
 */
UmiStatus umi_resequencer_add(UmiResequencer *resequencer,
                              const UmiMessageEnvelope *message);
/**
 * Provide the resequencer next operation used by this module and its client applications.
 */
UmiStatus umi_resequencer_next(UmiResequencer *resequencer,
                               UmiOwnedMessage *out_message);
/**
 * Provide the resequencer expected operation used by this module and its client
 * applications.
 */
uint64_t umi_resequencer_expected(const UmiResequencer *resequencer);
/**
 * Provide the resequencer buffered operation used by this module and its client
 * applications.
 */
size_t umi_resequencer_buffered(const UmiResequencer *resequencer);

#ifdef __cplusplus
}
#endif

#endif
