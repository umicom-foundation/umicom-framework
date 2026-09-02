/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/store.h
 *
 * PURPOSE:
 *   Retain a bounded chronological history of diagnostic records emitted
 *   through Umicom Framework.  The store owns copies of source and message
 *   text, exposes summary counters, and can be registered directly as a
 *   toolkit-neutral diagnostic sink.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the diagnostic store data shared with callers of this public contract.
 */
typedef struct UmiDiagnosticStore UmiDiagnosticStore;

/**
 * Represent the diagnostic store config data shared with callers of this public contract.
 */
typedef struct UmiDiagnosticStoreConfig {
    size_t capacity;
} UmiDiagnosticStoreConfig;

/**
 * Represent the diagnostic record data shared with callers of this public contract.
 */
typedef struct UmiDiagnosticRecord {
    uint64_t sequence;
    UmiDiagnosticSeverity severity;
    uint64_t correlation_id;
    char source[UMI_DIAGNOSTIC_STORE_SOURCE_CAPACITY];
    char message[UMI_DIAGNOSTIC_STORE_MESSAGE_CAPACITY];
} UmiDiagnosticRecord;

/**
 * Represent the diagnostic store summary data shared with callers of this public contract.
 */
typedef struct UmiDiagnosticStoreSummary {
    size_t retained_count;
    size_t capacity;
    uint64_t total_received;
    uint64_t overwritten_count;
    uint64_t retained_by_severity[UMI_DIAGNOSTIC_SEVERITY_COUNT];
} UmiDiagnosticStoreSummary;

/**
 * Provide the diagnostic store config default operation used by this module and its client
 * applications.
 */
UmiDiagnosticStoreConfig umi_diagnostic_store_config_default(void);

/**
 * Initialise diagnostic store from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_diagnostic_store_create(
    const UmiDiagnosticStoreConfig *config,
    UmiDiagnosticStore **out_store
);

/**
 * Release or reset state held by diagnostic store so the same storage can be reused
 * safely.
 */
void umi_diagnostic_store_destroy(UmiDiagnosticStore *store);
/**
 * Release or reset state held by diagnostic store so the same storage can be reused
 * safely.
 */
void umi_diagnostic_store_clear(UmiDiagnosticStore *store);

/**
 * Provide the diagnostic store sink operation used by this module and its client
 * applications.
 */
void umi_diagnostic_store_sink(
    const UmiDiagnostic *diagnostic,
    void *user_data
);

/**
 * Return the number of records represented by diagnostic store without changing their
 * state.
 */
size_t umi_diagnostic_store_count(UmiDiagnosticStore *store);
/**
 * Provide the diagnostic store capacity operation used by this module and its client
 * applications.
 */
size_t umi_diagnostic_store_capacity(UmiDiagnosticStore *store);

/**
 * Find diagnostic store record while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_diagnostic_store_record_at(
    UmiDiagnosticStore *store,
    size_t chronological_index,
    UmiDiagnosticRecord *out_record
);

/**
 * Provide the diagnostic store summary operation used by this module and its client
 * applications.
 */
UmiStatus umi_diagnostic_store_summary(
    UmiDiagnosticStore *store,
    UmiDiagnosticStoreSummary *out_summary
);

#ifdef __cplusplus
}
#endif

#endif
