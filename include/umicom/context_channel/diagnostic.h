/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/diagnostic.h
 *
 * PURPOSE:
 *   Expose actionable context-channel diagnostics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_DIAGNOSTIC_H
#define UMICOM_CONTEXT_CHANNEL_DIAGNOSTIC_H
#include "umicom/context_channel/payload.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_DIAGNOSTIC_MAX_ITEMS 128U
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
typedef struct UmiContextDiagnosticStore {
    UmiContextDiagnostic items[UMI_CONTEXT_DIAGNOSTIC_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextDiagnosticStore;
void umi_context_diagnostic_init(UmiContextDiagnostic *record);
UmiStatus umi_context_diagnostic_validate(const UmiContextDiagnostic *record);
void umi_context_diagnostic_store_init(UmiContextDiagnosticStore *store);
UmiStatus umi_context_diagnostic_store_put(UmiContextDiagnosticStore *store,const UmiContextDiagnostic *record);
UmiStatus umi_context_diagnostic_store_remove(UmiContextDiagnosticStore *store,const char *identity);
UmiContextDiagnostic *umi_context_diagnostic_store_find(UmiContextDiagnosticStore *store,const char *identity);
const UmiContextDiagnostic *umi_context_diagnostic_store_find_const(const UmiContextDiagnosticStore *store,const char *identity);
size_t umi_context_diagnostic_store_count(const UmiContextDiagnosticStore *store);
UmiStatus umi_context_diagnostic_store_snapshot(const UmiContextDiagnosticStore *store,UmiContextDiagnostic *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
