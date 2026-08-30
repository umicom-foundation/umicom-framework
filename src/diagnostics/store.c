/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/store.c
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

static void store_lock(UmiDiagnosticStore *store)
{
    while (atomic_flag_test_and_set_explicit(&store->lock,
                                              memory_order_acquire)) {
        /* A diagnostic write is intentionally short; wait for the owner. */
    }
}

static void store_unlock(UmiDiagnosticStore *store)
{
    atomic_flag_clear_explicit(&store->lock, memory_order_release);
}

static void copy_text(char *destination,
                      size_t destination_capacity,
                      const char *source)
{
    size_t source_length;
    size_t copy_length;

    if (destination == NULL || destination_capacity == 0U) {
        return;
    }

    if (source == NULL) {
        source = "";
    }

    source_length = strlen(source);
    copy_length = source_length;
    if (copy_length >= destination_capacity) {
        copy_length = destination_capacity - 1U;
    }

    if (copy_length > 0U) {
        (void)memcpy(destination, source, copy_length);
    }
    destination[copy_length] = '\0';
}

static int severity_index(UmiDiagnosticSeverity severity, size_t *out_index)
{
    size_t index;

    if (out_index == NULL) {
        return 0;
    }

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

UmiDiagnosticStoreConfig umi_diagnostic_store_config_default(void)
{
    UmiDiagnosticStoreConfig config;
    config.capacity = UMI_DIAGNOSTIC_STORE_DEFAULT_CAPACITY;
    return config;
}

UmiStatus umi_diagnostic_store_create(
    const UmiDiagnosticStoreConfig *config,
    UmiDiagnosticStore **out_store)
{
    UmiDiagnosticStoreConfig effective_config;
    UmiDiagnosticStore *store;

    if (out_store == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_store = NULL;

    effective_config = config != NULL
        ? *config
        : umi_diagnostic_store_config_default();

    if (effective_config.capacity == 0U ||
        effective_config.capacity > UMI_DIAGNOSTIC_STORE_MAX_CAPACITY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (effective_config.capacity >
        (SIZE_MAX / sizeof(UmiDiagnosticRecord))) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    store = (UmiDiagnosticStore *)calloc(1U, sizeof(*store));
    if (store == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    store->records = (UmiDiagnosticRecord *)calloc(
        effective_config.capacity,
        sizeof(*store->records)
    );
    if (store->records == NULL) {
        free(store);
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    store->capacity = effective_config.capacity;
    atomic_flag_clear_explicit(&store->lock, memory_order_release);
    *out_store = store;
    return UMI_STATUS_OK;
}

void umi_diagnostic_store_destroy(UmiDiagnosticStore *store)
{
    if (store == NULL) {
        return;
    }

    free(store->records);
    store->records = NULL;
    free(store);
}

void umi_diagnostic_store_clear(UmiDiagnosticStore *store)
{
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

void umi_diagnostic_store_sink(const UmiDiagnostic *diagnostic, void *user_data)
{
    UmiDiagnosticStore *store = (UmiDiagnosticStore *)user_data;
    UmiDiagnosticRecord *record;
    size_t write_index;
    size_t severity_slot;

    if (store == NULL || diagnostic == NULL) {
        return;
    }

    store_lock(store);

    if (store->retained_count == store->capacity) {
        UmiDiagnosticRecord *overwritten =
            &store->records[store->start_index];
        size_t overwritten_severity;

        if (severity_index(overwritten->severity,
                           &overwritten_severity) &&
            store->retained_by_severity[overwritten_severity] > 0U) {
            --store->retained_by_severity[overwritten_severity];
        }

        write_index = store->start_index;
        store->start_index = (store->start_index + 1U) % store->capacity;
        ++store->overwritten_count;
    } else {
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

    if (severity_index(record->severity, &severity_slot)) {
        ++store->retained_by_severity[severity_slot];
    }

    store_unlock(store);
}

size_t umi_diagnostic_store_count(UmiDiagnosticStore *store)
{
    size_t count;

    if (store == NULL) {
        return 0U;
    }

    store_lock(store);
    count = store->retained_count;
    store_unlock(store);
    return count;
}

size_t umi_diagnostic_store_capacity(UmiDiagnosticStore *store)
{
    size_t capacity;

    if (store == NULL) {
        return 0U;
    }

    store_lock(store);
    capacity = store->capacity;
    store_unlock(store);
    return capacity;
}

UmiStatus umi_diagnostic_store_record_at(
    UmiDiagnosticStore *store,
    size_t chronological_index,
    UmiDiagnosticRecord *out_record)
{
    size_t physical_index;

    if (store == NULL || out_record == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    store_lock(store);
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

UmiStatus umi_diagnostic_store_summary(
    UmiDiagnosticStore *store,
    UmiDiagnosticStoreSummary *out_summary)
{
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
