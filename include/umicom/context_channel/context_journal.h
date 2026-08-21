/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_journal.h
 *
 * PURPOSE:
 *   Retain ordered context publication evidence for diagnostics and replay.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_JOURNAL_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_JOURNAL_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
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
void umi_context_journal_init(UmiContextJournal *state);
UmiStatus umi_context_journal_set_field(UmiContextJournal *state,size_t field_index,const char *value);
const char *umi_context_journal_field(const UmiContextJournal *state,size_t field_index);
UmiStatus umi_context_journal_record_success(UmiContextJournal *state,uint64_t sequence);
UmiStatus umi_context_journal_record_failure(UmiContextJournal *state,UmiStatus status,uint64_t sequence);
UmiStatus umi_context_journal_validate(const UmiContextJournal *state);
bool umi_context_journal_covers_sequence(const UmiContextJournal *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
