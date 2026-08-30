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
#include "umicom/base/status.h"
#include "umicom/messaging/event_bus.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiJournal UmiJournal;

UmiStatus umi_journal_open(const char *path, UmiJournal **out_journal);
void umi_journal_close(UmiJournal *journal);
UmiStatus umi_journal_append(UmiJournal *journal, const UmiMessageEnvelope *event);
UmiStatus umi_journal_replay(const char *path, UmiEventBus *event_bus,
                             size_t *out_event_count);

#ifdef __cplusplus
}
#endif

#endif
