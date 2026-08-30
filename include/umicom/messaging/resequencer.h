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

typedef struct UmiResequencer UmiResequencer;

UmiStatus umi_resequencer_create(uint64_t first_sequence,
                                 size_t capacity,
                                 UmiResequencer **out_resequencer);
void umi_resequencer_destroy(UmiResequencer *resequencer);
UmiStatus umi_resequencer_add(UmiResequencer *resequencer,
                              const UmiMessageEnvelope *message);
UmiStatus umi_resequencer_next(UmiResequencer *resequencer,
                               UmiOwnedMessage *out_message);
uint64_t umi_resequencer_expected(const UmiResequencer *resequencer);
size_t umi_resequencer_buffered(const UmiResequencer *resequencer);

#ifdef __cplusplus
}
#endif

#endif
