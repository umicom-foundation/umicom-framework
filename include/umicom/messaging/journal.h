/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/journal.h
 *
 * PURPOSE:
 *   Publish the public journal contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MESSAGING_JOURNAL_H
#define UMICOM_MESSAGING_JOURNAL_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/messaging/event_bus.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the journal data shared with callers of this public contract.
 */
typedef struct UmiJournal UmiJournal;

/**
 * Provide the journal open operation used by this module and its client applications.
 */
UmiStatus umi_journal_open(const char *path, UmiJournal **out_journal);
/**
 * Provide the journal close operation used by this module and its client applications.
 */
void umi_journal_close(UmiJournal *journal);
/**
 * Add journal only after its inputs and available capacity have been checked.
 */
UmiStatus umi_journal_append(UmiJournal *journal, const UmiMessageEnvelope *event);
/**
 * Provide the journal replay operation used by this module and its client applications.
 */
UmiStatus umi_journal_replay(const char *path, UmiEventBus *event_bus,
                             size_t *out_event_count);
/**
 * Replay only records whose durable sequence is greater than the caller's
 * checkpoint, and return the newest sequence seen in the journal.
 */
UmiStatus umi_journal_replay_after(const char *path,
                                   UmiEventBus *event_bus,
                                   uint64_t after_sequence,
                                   size_t *out_event_count,
                                   uint64_t *out_last_sequence);
/**
 * Read the greatest recorded sequence without replaying events. This lets a
 * durable publisher resume numbering after a process restart.
 */
UmiStatus umi_journal_last_sequence(const char *path,
                                    uint64_t *out_sequence);

#ifdef __cplusplus
}
#endif

#endif
