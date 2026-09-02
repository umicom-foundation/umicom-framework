/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_import.h
 *
 * PURPOSE:
 *   Validate and stage imported context records before publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_IMPORT_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_IMPORT_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context import data shared with callers of this public contract.
 */
typedef struct UmiContextImport {
    uint32_t structure_size;
    char import_id[UMI_CONTEXT_VALUE_CAPACITY];
    char schema_id[UMI_CONTEXT_VALUE_CAPACITY];
    char source_name[UMI_CONTEXT_VALUE_CAPACITY];
    char target_channel[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextImport;
/**
 * Initialise context import from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_import_init(UmiContextImport *state);
/**
 * Provide the context import set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_import_set_field(UmiContextImport *state,size_t field_index,const char *value);
/**
 * Provide the context import field operation used by this module and its client
 * applications.
 */
const char *umi_context_import_field(const UmiContextImport *state,size_t field_index);
/**
 * Provide the context import record success operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_import_record_success(UmiContextImport *state,uint64_t sequence);
/**
 * Provide the context import record failure operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_import_record_failure(UmiContextImport *state,UmiStatus status,uint64_t sequence);
/**
 * Check that context import satisfies its contract before another service relies on it.
 */
UmiStatus umi_context_import_validate(const UmiContextImport *state);
/**
 * Provide the context import covers sequence operation used by this module and its client
 * applications.
 */
bool umi_context_import_covers_sequence(const UmiContextImport *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
