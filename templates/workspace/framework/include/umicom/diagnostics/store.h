/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/include/umicom/diagnostics/store.h
 *
 * PURPOSE:
 *   Retain a bounded chronological history of diagnostic records emitted
 *   through Umicom Framework.  The store owns copies of source and message
 *   text, exposes summary counters, and can be registered directly as a
 *   toolkit-neutral diagnostic sink.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTICS_STORE_H
#define UMICOM_DIAGNOSTICS_STORE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/diagnostics/diagnostic.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DIAGNOSTIC_STORE_DEFAULT_CAPACITY 512U
#define UMI_DIAGNOSTIC_STORE_MAX_CAPACITY 65536U
#define UMI_DIAGNOSTIC_STORE_SOURCE_CAPACITY 128U
#define UMI_DIAGNOSTIC_STORE_MESSAGE_CAPACITY 1024U
#define UMI_DIAGNOSTIC_SEVERITY_COUNT 5U

typedef struct UmiDiagnosticStore UmiDiagnosticStore;

typedef struct UmiDiagnosticStoreConfig {
    size_t capacity;
} UmiDiagnosticStoreConfig;

typedef struct UmiDiagnosticRecord {
    uint64_t sequence;
    UmiDiagnosticSeverity severity;
    uint64_t correlation_id;
    char source[UMI_DIAGNOSTIC_STORE_SOURCE_CAPACITY];
    char message[UMI_DIAGNOSTIC_STORE_MESSAGE_CAPACITY];
} UmiDiagnosticRecord;

typedef struct UmiDiagnosticStoreSummary {
    size_t retained_count;
    size_t capacity;
    uint64_t total_received;
    uint64_t overwritten_count;
    uint64_t retained_by_severity[UMI_DIAGNOSTIC_SEVERITY_COUNT];
} UmiDiagnosticStoreSummary;

UmiDiagnosticStoreConfig umi_diagnostic_store_config_default(void);

UmiStatus umi_diagnostic_store_create(
    const UmiDiagnosticStoreConfig *config,
    UmiDiagnosticStore **out_store
);

void umi_diagnostic_store_destroy(UmiDiagnosticStore *store);
void umi_diagnostic_store_clear(UmiDiagnosticStore *store);

void umi_diagnostic_store_sink(
    const UmiDiagnostic *diagnostic,
    void *user_data
);

size_t umi_diagnostic_store_count(UmiDiagnosticStore *store);
size_t umi_diagnostic_store_capacity(UmiDiagnosticStore *store);

UmiStatus umi_diagnostic_store_record_at(
    UmiDiagnosticStore *store,
    size_t chronological_index,
    UmiDiagnosticRecord *out_record
);

UmiStatus umi_diagnostic_store_summary(
    UmiDiagnosticStore *store,
    UmiDiagnosticStoreSummary *out_summary
);

#ifdef __cplusplus
}
#endif

#endif
