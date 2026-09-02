/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/tests/test_diagnostic_store.c
 *
 * PURPOSE:
 *   Verify chronological reads, bounded overwrite behaviour, severity counts,
 *   owned text copies, summary counters, and clear-and-reuse operation for the
 *   toolkit-neutral Framework diagnostic store.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/store.h"

#include <assert.h>
#include <string.h>

/*
 * Exercise emit and return a clear result when the behaviour no longer matches its
 * contract.
 */
static void emit(UmiDiagnosticStore *store,
                 UmiDiagnosticSeverity severity,
                 const char *source,
                 const char *message,
                 uint64_t correlation_id)
{
    UmiDiagnostic diagnostic;
    diagnostic.severity = severity;
    diagnostic.source = source;
    diagnostic.message = message;
    diagnostic.correlation_id = correlation_id;
    umi_diagnostic_store_sink(&diagnostic, store);
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDiagnosticStoreConfig config = umi_diagnostic_store_config_default();
    UmiDiagnosticStore *store = NULL;
    UmiDiagnosticStoreSummary summary;
    UmiDiagnosticRecord record;
    char mutable_message[] = "second";

    config.capacity = 3U;
    assert(umi_diagnostic_store_create(&config, &store) == UMI_STATUS_OK);
    assert(store != NULL);
    assert(umi_diagnostic_store_capacity(store) == 3U);

    emit(store, UMI_DIAGNOSTIC_INFO, "test", "first", 1U);
    emit(store, UMI_DIAGNOSTIC_WARNING, "test", mutable_message, 2U);
    mutable_message[0] = 'X';
    emit(store, UMI_DIAGNOSTIC_ERROR, "test", "third", 3U);
    emit(store, UMI_DIAGNOSTIC_FATAL, "test", "fourth", 4U);

    assert(umi_diagnostic_store_count(store) == 3U);
    assert(umi_diagnostic_store_summary(store, &summary) == UMI_STATUS_OK);
    assert(summary.retained_count == 3U);
    assert(summary.capacity == 3U);
    assert(summary.total_received == 4U);
    assert(summary.overwritten_count == 1U);
    assert(summary.retained_by_severity[0] == 0U);
    assert(summary.retained_by_severity[1] == 0U);
    assert(summary.retained_by_severity[2] == 1U);
    assert(summary.retained_by_severity[3] == 1U);
    assert(summary.retained_by_severity[4] == 1U);

    assert(umi_diagnostic_store_record_at(store, 0U, &record) == UMI_STATUS_OK);
    assert(record.sequence == 2U);
    assert(record.severity == UMI_DIAGNOSTIC_WARNING);
    assert(record.correlation_id == 2U);
    assert(strcmp(record.source, "test") == 0);
    assert(strcmp(record.message, "second") == 0);

    assert(umi_diagnostic_store_record_at(store, 2U, &record) == UMI_STATUS_OK);
    assert(record.sequence == 4U);
    assert(record.severity == UMI_DIAGNOSTIC_FATAL);
    assert(strcmp(record.message, "fourth") == 0);
    assert(umi_diagnostic_store_record_at(store, 3U, &record) == UMI_STATUS_NOT_FOUND);

    umi_diagnostic_store_clear(store);
    assert(umi_diagnostic_store_summary(store, &summary) == UMI_STATUS_OK);
    assert(summary.retained_count == 0U);
    assert(summary.total_received == 0U);
    assert(summary.overwritten_count == 0U);

    umi_diagnostic_store_destroy(store);
    return 0;
}
