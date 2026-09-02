/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_journal.h
 *
 * PURPOSE:
 *   Retain ordered context publication evidence for diagnostics and replay.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_JOURNAL_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_JOURNAL_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context journal data shared with callers of this public contract.
 */
typedef struct UmiContextJournal {
    uint32_t structure_size;
    char journal_id[UMI_CONTEXT_VALUE_CAPACITY];
    char channel_id[UMI_CONTEXT_VALUE_CAPACITY];
    char context_id[UMI_CONTEXT_VALUE_CAPACITY];
    char correlation_id[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextJournal;
/**
 * Initialise context journal from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_journal_init(UmiContextJournal *state);
/**
 * Provide the context journal set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_journal_set_field(UmiContextJournal *state,size_t field_index,const char *value);
/**
 * Provide the context journal field operation used by this module and its client
 * applications.
 */
const char *umi_context_journal_field(const UmiContextJournal *state,size_t field_index);
/**
 * Provide the context journal record success operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_journal_record_success(UmiContextJournal *state,uint64_t sequence);
/**
 * Provide the context journal record failure operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_journal_record_failure(UmiContextJournal *state,UmiStatus status,uint64_t sequence);
/**
 * Check that context journal satisfies its contract before another service relies on it.
 */
UmiStatus umi_context_journal_validate(const UmiContextJournal *state);
/**
 * Provide the context journal covers sequence operation used by this module and its client
 * applications.
 */
bool umi_context_journal_covers_sequence(const UmiContextJournal *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
