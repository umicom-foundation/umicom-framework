/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/src/diagnostics/store.c
 *
 * PURPOSE:
 *   Implement a bounded, thread-safe diagnostic ring buffer.  The oldest
 *   retained record is overwritten when capacity is reached, while lifetime
 *   received and overwrite counters remain available for observability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/store.h"

#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>

struct UmiDiagnosticStore {
    UmiDiagnosticRecord *records;
    size_t capacity;
    size_t start_index;
    size_t retained_count;
    uint64_t total_received;
    uint64_t overwritten_count;
    uint64_t retained_by_severity[UMI_DIAGNOSTIC_SEVERITY_COUNT];
    atomic_flag lock;
};

/* Provide the store lock operation used by this module and its client applications. */
static void store_lock(UmiDiagnosticStore *store)
{
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (atomic_flag_test_and_set_explicit(&store->lock,
                                              memory_order_acquire)) {
        /* A diagnostic write is intentionally short; wait for the owner. */
    }
}

/* Provide the store unlock operation used by this module and its client applications. */
static void store_unlock(UmiDiagnosticStore *store)
{
    atomic_flag_clear_explicit(&store->lock, memory_order_release);
}

/* Provide the copy text operation used by this module and its client applications. */
static void copy_text(char *destination,
                      size_t destination_capacity,
                      const char *source)
{
    size_t source_length;
    size_t copy_length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || destination_capacity == 0U) {
        return;
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL) {
        source = "";
    }

    source_length = strlen(source);
    copy_length = source_length;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (copy_length >= destination_capacity) {
        copy_length = destination_capacity - 1U;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (copy_length > 0U) {
        (void)memcpy(destination, source, copy_length);
    }
    destination[copy_length] = '\0';
}

/* Provide the severity index operation used by this module and its client applications. */
static int severity_index(UmiDiagnosticSeverity severity, size_t *out_index)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_index == NULL) {
        return 0;
    }

    /* Select the behaviour associated with the requested command or state value. */
    switch (severity) {
        case UMI_DIAGNOSTIC_TRACE: index = 0U; break;
        case UMI_DIAGNOSTIC_INFO: index = 1U; break;
        case UMI_DIAGNOSTIC_WARNING: index = 2U; break;
        case UMI_DIAGNOSTIC_ERROR: index = 3U; break;
        case UMI_DIAGNOSTIC_FATAL: index = 4U; break;
        default: return 0;
    }

    *out_index = index;
    return 1;
}

/*
 * Provide the diagnostic store config default operation used by this module and its client
 * applications.
 */
UmiDiagnosticStoreConfig umi_diagnostic_store_config_default(void)
{
    UmiDiagnosticStoreConfig config;
    config.capacity = UMI_DIAGNOSTIC_STORE_DEFAULT_CAPACITY;
    return config;
}

/*
 * Initialise diagnostic store from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_diagnostic_store_create(
    const UmiDiagnosticStoreConfig *config,
    UmiDiagnosticStore **out_store)
{
    UmiDiagnosticStoreConfig effective_config;
    UmiDiagnosticStore *store;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_store == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_store = NULL;

    effective_config = config != NULL
        ? *config
        : umi_diagnostic_store_config_default();

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (effective_config.capacity == 0U ||
        effective_config.capacity > UMI_DIAGNOSTIC_STORE_MAX_CAPACITY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (effective_config.capacity >
        (SIZE_MAX / sizeof(UmiDiagnosticRecord))) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    store = (UmiDiagnosticStore *)calloc(1U, sizeof(*store));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    store->records = (UmiDiagnosticRecord *)calloc(
        effective_config.capacity,
        sizeof(*store->records)
    );
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store->records == NULL) {
        free(store);
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    store->capacity = effective_config.capacity;
    atomic_flag_clear_explicit(&store->lock, memory_order_release);
    *out_store = store;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by diagnostic store so the same storage can be reused
 * safely.
 */
void umi_diagnostic_store_destroy(UmiDiagnosticStore *store)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL) {
        return;
    }

    free(store->records);
    store->records = NULL;
    free(store);
}

/*
 * Release or reset state held by diagnostic store so the same storage can be reused
 * safely.
 */
void umi_diagnostic_store_clear(UmiDiagnosticStore *store)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL) {
        return;
    }

    store_lock(store);
    (void)memset(store->records,
                 0,
                 store->capacity * sizeof(*store->records));
    store->start_index = 0U;
    store->retained_count = 0U;
    store->total_received = 0U;
    store->overwritten_count = 0U;
    (void)memset(store->retained_by_severity,
                 0,
                 sizeof(store->retained_by_severity));
    store_unlock(store);
}

/*
 * Provide the diagnostic store sink operation used by this module and its client
 * applications.
 */
void umi_diagnostic_store_sink(const UmiDiagnostic *diagnostic, void *user_data)
{
    UmiDiagnosticStore *store = (UmiDiagnosticStore *)user_data;
    UmiDiagnosticRecord *record;
    size_t write_index;
    size_t severity_slot;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || diagnostic == NULL) {
        return;
    }

    store_lock(store);

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (store->retained_count == store->capacity) {
        UmiDiagnosticRecord *overwritten =
            &store->records[store->start_index];
        size_t overwritten_severity;

        /* Apply this branch only when its contract condition is satisfied. */
        if (severity_index(overwritten->severity,
                           &overwritten_severity) &&
            store->retained_by_severity[overwritten_severity] > 0U) {
            --store->retained_by_severity[overwritten_severity];
        }

        write_index = store->start_index;
        store->start_index = (store->start_index + 1U) % store->capacity;
        ++store->overwritten_count;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        write_index = (store->start_index + store->retained_count) %
                      store->capacity;
        ++store->retained_count;
    }

    ++store->total_received;
    record = &store->records[write_index];
    (void)memset(record, 0, sizeof(*record));
    record->sequence = store->total_received;
    record->severity = diagnostic->severity;
    record->correlation_id = diagnostic->correlation_id;
    copy_text(record->source,
              sizeof(record->source),
              diagnostic->source != NULL ? diagnostic->source : "unknown");
    copy_text(record->message,
              sizeof(record->message),
              diagnostic->message != NULL ? diagnostic->message : "");

    /* Apply this branch only when its contract condition is satisfied. */
    if (severity_index(record->severity, &severity_slot)) {
        ++store->retained_by_severity[severity_slot];
    }

    store_unlock(store);
}

/*
 * Return the number of records represented by diagnostic store without changing their
 * state.
 */
size_t umi_diagnostic_store_count(UmiDiagnosticStore *store)
{
    size_t count;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL) {
        return 0U;
    }

    store_lock(store);
    count = store->retained_count;
    store_unlock(store);
    return count;
}

/*
 * Provide the diagnostic store capacity operation used by this module and its client
 * applications.
 */
size_t umi_diagnostic_store_capacity(UmiDiagnosticStore *store)
{
    size_t capacity;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL) {
        return 0U;
    }

    store_lock(store);
    capacity = store->capacity;
    store_unlock(store);
    return capacity;
}

/*
 * Find diagnostic store record while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_diagnostic_store_record_at(
    UmiDiagnosticStore *store,
    size_t chronological_index,
    UmiDiagnosticRecord *out_record)
{
    size_t physical_index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || out_record == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    store_lock(store);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (chronological_index >= store->retained_count) {
        store_unlock(store);
        return UMI_STATUS_NOT_FOUND;
    }

    physical_index = (store->start_index + chronological_index) %
                     store->capacity;
    *out_record = store->records[physical_index];
    store_unlock(store);
    return UMI_STATUS_OK;
}

/*
 * Provide the diagnostic store summary operation used by this module and its client
 * applications.
 */
UmiStatus umi_diagnostic_store_summary(
    UmiDiagnosticStore *store,
    UmiDiagnosticStoreSummary *out_summary)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || out_summary == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    store_lock(store);
    out_summary->retained_count = store->retained_count;
    out_summary->capacity = store->capacity;
    out_summary->total_received = store->total_received;
    out_summary->overwritten_count = store->overwritten_count;
    (void)memcpy(out_summary->retained_by_severity,
                 store->retained_by_severity,
                 sizeof(out_summary->retained_by_severity));
    store_unlock(store);
    return UMI_STATUS_OK;
}
