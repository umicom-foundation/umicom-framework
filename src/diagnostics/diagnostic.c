#include "umicom/diagnostics/diagnostic.h"

#include <stdio.h>
#include <string.h>

const char *umi_diagnostic_severity_text(UmiDiagnosticSeverity severity)
{
    switch (severity) {
        case UMI_DIAGNOSTIC_TRACE: return "TRACE";
        case UMI_DIAGNOSTIC_INFO: return "INFO";
        case UMI_DIAGNOSTIC_WARNING: return "WARNING";
        case UMI_DIAGNOSTIC_ERROR: return "ERROR";
        case UMI_DIAGNOSTIC_FATAL: return "FATAL";
        default: return "UNKNOWN";
    }
}

const char *umi_diagnostic_kind_text(UmiDiagnosticKind kind)
{
    switch (kind) {
        case UMI_DIAGNOSTIC_KIND_GENERAL: return "GENERAL";
        case UMI_DIAGNOSTIC_KIND_COMPILER: return "COMPILER";
        case UMI_DIAGNOSTIC_KIND_LINKER: return "LINKER";
        case UMI_DIAGNOSTIC_KIND_TEST: return "TEST";
        case UMI_DIAGNOSTIC_KIND_RUNTIME: return "RUNTIME";
        case UMI_DIAGNOSTIC_KIND_LANGUAGE: return "LANGUAGE";
        case UMI_DIAGNOSTIC_KIND_VALIDATION: return "VALIDATION";
        case UMI_DIAGNOSTIC_KIND_SECURITY: return "SECURITY";
        default: return "UNKNOWN";
    }
}

static int copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U) {
        return 0;
    }
    source = source != NULL ? source : "";
    length = strlen(source);
    if (length >= capacity) {
        destination[0] = '\0';
        return 0;
    }
    (void)memcpy(destination, source, length + 1U);
    return 1;
}

UmiStatus umi_diagnostic_snapshot_init(UmiDiagnosticSnapshot *snapshot,
                                       const char *id,
                                       UmiDiagnosticSeverity severity,
                                       UmiDiagnosticKind kind,
                                       const char *source,
                                       const char *message)
{
    if (snapshot == NULL || id == NULL || id[0] == '\0' ||
        message == NULL || message[0] == '\0' ||
        severity < UMI_DIAGNOSTIC_TRACE || severity > UMI_DIAGNOSTIC_FATAL ||
        kind < UMI_DIAGNOSTIC_KIND_GENERAL || kind > UMI_DIAGNOSTIC_KIND_SECURITY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(snapshot, 0, sizeof(*snapshot));
    snapshot->struct_size = (uint32_t)sizeof(*snapshot);
    snapshot->api_version = UMI_DIAGNOSTIC_API_VERSION;
    snapshot->severity = severity;
    snapshot->kind = kind;
    if (!copy_text(snapshot->id, sizeof(snapshot->id), id) ||
        !copy_text(snapshot->source, sizeof(snapshot->source), source) ||
        !copy_text(snapshot->message, sizeof(snapshot->message), message)) {
        (void)memset(snapshot, 0, sizeof(*snapshot));
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_diagnostic_snapshot_validate(const UmiDiagnosticSnapshot *snapshot,
                                           char *out_message,
                                           size_t capacity)
{
    const char *message = "valid";
    UmiStatus status = UMI_STATUS_OK;
    if (snapshot == NULL) {
        message = "diagnostic snapshot is null";
        status = UMI_STATUS_INVALID_ARGUMENT;
    } else if (snapshot->struct_size != (uint32_t)sizeof(*snapshot) ||
               snapshot->api_version != UMI_DIAGNOSTIC_API_VERSION) {
        message = "diagnostic snapshot ABI version is unsupported";
        status = UMI_STATUS_INVALID_STATE;
    } else if (snapshot->id[0] == '\0' || snapshot->message[0] == '\0') {
        message = "diagnostic id and message are required";
        status = UMI_STATUS_INVALID_ARGUMENT;
    } else if (snapshot->severity < UMI_DIAGNOSTIC_TRACE ||
               snapshot->severity > UMI_DIAGNOSTIC_FATAL ||
               snapshot->kind < UMI_DIAGNOSTIC_KIND_GENERAL ||
               snapshot->kind > UMI_DIAGNOSTIC_KIND_SECURITY) {
        message = "diagnostic severity or kind is outside the supported range";
        status = UMI_STATUS_INVALID_ARGUMENT;
    } else if ((snapshot->line == 0U && snapshot->column != 0U) ||
               (snapshot->end_line != 0U && snapshot->line == 0U)) {
        message = "diagnostic source range is inconsistent";
        status = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "%s", message);
    }
    return status;
}

void umi_diagnostic_emit(UmiDiagnosticSink sink, void *user_data,
                         UmiDiagnosticSeverity severity, const char *source,
                         const char *message, uint64_t correlation_id)
{
    UmiDiagnostic diagnostic;
    if (sink == 0) {
        return;
    }
    diagnostic.severity = severity;
    diagnostic.source = source != 0 ? source : "unknown";
    diagnostic.message = message != 0 ? message : "";
    diagnostic.correlation_id = correlation_id;
    sink(&diagnostic, user_data);
}
