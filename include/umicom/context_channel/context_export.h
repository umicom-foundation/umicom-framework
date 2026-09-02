/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_export.h
 *
 * PURPOSE:
 *   Plan portable context exports without leaking application-private state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_EXPORT_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_EXPORT_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context export data shared with callers of this public contract.
 */
typedef struct UmiContextExport {
    uint32_t structure_size;
    char export_id[UMI_CONTEXT_VALUE_CAPACITY];
    char channel_id[UMI_CONTEXT_VALUE_CAPACITY];
    char schema_id[UMI_CONTEXT_VALUE_CAPACITY];
    char destination[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextExport;
/**
 * Initialise context export from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_export_init(UmiContextExport *state);
/**
 * Provide the context export set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_export_set_field(UmiContextExport *state,size_t field_index,const char *value);
/**
 * Provide the context export field operation used by this module and its client
 * applications.
 */
const char *umi_context_export_field(const UmiContextExport *state,size_t field_index);
/**
 * Provide the context export record success operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_export_record_success(UmiContextExport *state,uint64_t sequence);
/**
 * Provide the context export record failure operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_export_record_failure(UmiContextExport *state,UmiStatus status,uint64_t sequence);
/**
 * Check that context export satisfies its contract before another service relies on it.
 */
UmiStatus umi_context_export_validate(const UmiContextExport *state);
/**
 * Provide the context export covers sequence operation used by this module and its client
 * applications.
 */
bool umi_context_export_covers_sequence(const UmiContextExport *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
