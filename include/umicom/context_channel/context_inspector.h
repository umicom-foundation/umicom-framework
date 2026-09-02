/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_inspector.h
 *
 * PURPOSE:
 *   Expose a read-only property projection for one typed context payload.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_INSPECTOR_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_INSPECTOR_H
#include "umicom/context_channel/context_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context inspector data shared with callers of this public contract.
 */
typedef struct UmiContextInspector {
    uint32_t structure_size;
    char context_id[UMI_CONTEXT_VALUE_CAPACITY];
    char schema_id[UMI_CONTEXT_VALUE_CAPACITY];
    char source_application[UMI_CONTEXT_VALUE_CAPACITY];
    char source_panel[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextInspector;
/**
 * Initialise context inspector from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_inspector_init(UmiContextInspector *state);
/**
 * Provide the context inspector set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_inspector_set_field(UmiContextInspector *state,size_t field_index,const char *value);
/**
 * Provide the context inspector field operation used by this module and its client
 * applications.
 */
const char *umi_context_inspector_field(const UmiContextInspector *state,size_t field_index);
/**
 * Provide the context inspector record success operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_inspector_record_success(UmiContextInspector *state,uint64_t sequence);
/**
 * Provide the context inspector record failure operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_inspector_record_failure(UmiContextInspector *state,UmiStatus status,uint64_t sequence);
/**
 * Check that context inspector satisfies its contract before another service relies on it.
 */
UmiStatus umi_context_inspector_validate(const UmiContextInspector *state);
/**
 * Provide the context inspector covers sequence operation used by this module and its
 * client applications.
 */
bool umi_context_inspector_covers_sequence(const UmiContextInspector *state,uint64_t sequence);
#ifdef __cplusplus
}
#endif
#endif
