/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/include/umicom/messaging/journal.h
 *
 * PURPOSE:
 *   Declare the journal contract shared by Framework services and thin
 *   applications.
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

#ifdef __cplusplus
}
#endif

#endif
