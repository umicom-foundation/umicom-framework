/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/diagnostic.h
 *
 * PURPOSE:
 *   Expose actionable context-channel diagnostics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_DIAGNOSTIC_H
#define UMICOM_CONTEXT_CHANNEL_DIAGNOSTIC_H
#include "umicom/context_channel/payload.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_DIAGNOSTIC_MAX_ITEMS 128U
/**
 * Represent the context diagnostic data shared with callers of this public contract.
 */
typedef struct UmiContextDiagnostic {
    uint32_t structure_size;
    char diagnostic_id[UMI_CONTEXT_TEXT_CAPACITY];
    char channel_id[UMI_CONTEXT_TEXT_CAPACITY];
    char context_id[UMI_CONTEXT_TEXT_CAPACITY];
    char message[UMI_CONTEXT_VALUE_CAPACITY];
    uint32_t severity;
    UmiStatus status;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiContextDiagnostic;
/**
 * Represent the context diagnostic store data shared with callers of this public contract.
 */
typedef struct UmiContextDiagnosticStore {
    UmiContextDiagnostic items[UMI_CONTEXT_DIAGNOSTIC_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextDiagnosticStore;
/**
 * Initialise context diagnostic from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_diagnostic_init(UmiContextDiagnostic *record);
/**
 * Check that context diagnostic satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_context_diagnostic_validate(const UmiContextDiagnostic *record);
/**
 * Initialise context diagnostic store from caller-provided values so later operations
 * receive a known state.
 */
void umi_context_diagnostic_store_init(UmiContextDiagnosticStore *store);
/**
 * Provide the context diagnostic store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_diagnostic_store_put(UmiContextDiagnosticStore *store,const UmiContextDiagnostic *record);
/**
 * Remove context diagnostic store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_context_diagnostic_store_remove(UmiContextDiagnosticStore *store,const char *identity);
/**
 * Find context diagnostic store while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiContextDiagnostic *umi_context_diagnostic_store_find(UmiContextDiagnosticStore *store,const char *identity);
/**
 * Provide the context diagnostic store find const operation used by this module and its
 * client applications.
 */
const UmiContextDiagnostic *umi_context_diagnostic_store_find_const(const UmiContextDiagnosticStore *store,const char *identity);
/**
 * Return the number of records represented by context diagnostic store without changing
 * their state.
 */
size_t umi_context_diagnostic_store_count(const UmiContextDiagnosticStore *store);
/**
 * Provide the context diagnostic store snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_diagnostic_store_snapshot(const UmiContextDiagnosticStore *store,UmiContextDiagnostic *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
