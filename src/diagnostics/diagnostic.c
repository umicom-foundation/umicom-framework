/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/diagnostic.c
 *
 * PURPOSE:
 *   Implement the diagnostic behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/diagnostic.h"
#include "../base/value_construction_internal.h"

#include <stdio.h>
#include <string.h>

/*
 * Provide the diagnostic severity text operation used by this module and its client
 * applications.
 */
const char *umi_diagnostic_severity_text(UmiDiagnosticSeverity severity)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (severity) {
        case UMI_DIAGNOSTIC_TRACE: return "TRACE";
        case UMI_DIAGNOSTIC_INFO: return "INFO";
        case UMI_DIAGNOSTIC_WARNING: return "WARNING";
        case UMI_DIAGNOSTIC_ERROR: return "ERROR";
        case UMI_DIAGNOSTIC_FATAL: return "FATAL";
        default: return "UNKNOWN";
    }
}

/*
 * Provide the diagnostic kind text operation used by this module and its client
 * applications.
 */
const char *umi_diagnostic_kind_text(UmiDiagnosticKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
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

/* Provide the copy text operation used by this module and its client applications. */
static int copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) {
        return 0;
    }
    source = source != NULL ? source : "";
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) {
        destination[0] = '\0';
        return 0;
    }
    (void)memcpy(destination, source, length + 1U);
    return 1;
}

/*
 * Initialise diagnostic snapshot from caller-provided values so later operations receive a
 * known state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_diagnostic_snapshot_init(UmiDiagnosticSnapshot *snapshot,
                                       const char *id,
                                       UmiDiagnosticSeverity severity,
                                       UmiDiagnosticKind kind,
                                       const char *source,
                                       const char *message)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Apply this branch only when its contract condition is satisfied. */
    if (!copy_text(snapshot->id, sizeof(snapshot->id), id) ||
        !copy_text(snapshot->source, sizeof(snapshot->source), source) ||
        !copy_text(snapshot->message, sizeof(snapshot->message), message)) {
        (void)memset(snapshot, 0, sizeof(*snapshot));
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_diagnostic_snapshot_init_build_value(UmiDiagnosticSnapshot *snapshot,
                                       const char *id,
                                       UmiDiagnosticSeverity severity,
                                       UmiDiagnosticKind kind,
                                       const char *source,
                                       const char *message)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Apply this branch only when its contract condition is satisfied. */
    if (!copy_text(snapshot->id, sizeof(snapshot->id), id) ||
        !copy_text(snapshot->source, sizeof(snapshot->source), source) ||
        !copy_text(snapshot->message, sizeof(snapshot->message), message)) {
        (void)memset(snapshot, 0, sizeof(*snapshot));
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_diagnostic_snapshot_init, UmiDiagnosticSnapshot, snapshot,
    (UmiDiagnosticSnapshot *snapshot, const char *id, UmiDiagnosticSeverity severity, UmiDiagnosticKind kind, const char *source, const char *message),
    umi_diagnostic_snapshot_init_build_value(candidate, id, severity, kind, source, message))


/*
 * Check that diagnostic snapshot satisfies its contract before another service relies on
 * it.
 */
/* Bounded validation now rejects unterminated inline text before records enter model queries or native presentation Retained for engineering review. */
#if 0
UmiStatus umi_diagnostic_snapshot_validate(const UmiDiagnosticSnapshot *snapshot,
                                           char *out_message,
                                           size_t capacity)
{
    const char *message = "valid";
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (snapshot == NULL) {
        message = "diagnostic snapshot is null";
        status = UMI_STATUS_INVALID_ARGUMENT;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (snapshot->struct_size != (uint32_t)sizeof(*snapshot) ||
               snapshot->api_version != UMI_DIAGNOSTIC_API_VERSION) {
        message = "diagnostic snapshot ABI version is unsupported";
        status = UMI_STATUS_INVALID_STATE;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (snapshot->id[0] == '\0' || snapshot->message[0] == '\0') {
        message = "diagnostic id and message are required";
        status = UMI_STATUS_INVALID_ARGUMENT;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (snapshot->severity < UMI_DIAGNOSTIC_TRACE ||
               snapshot->severity > UMI_DIAGNOSTIC_FATAL ||
               snapshot->kind < UMI_DIAGNOSTIC_KIND_GENERAL ||
               snapshot->kind > UMI_DIAGNOSTIC_KIND_SECURITY) {
        message = "diagnostic severity or kind is outside the supported range";
        status = UMI_STATUS_INVALID_ARGUMENT;
    } else /* Apply this branch only when its contract condition is satisfied. */ if ((snapshot->line == 0U && snapshot->column != 0U) ||
               (snapshot->end_line != 0U && snapshot->line == 0U)) {
        message = "diagnostic source range is inconsistent";
        status = UMI_STATUS_INVALID_ARGUMENT;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "%s", message);
    }
    return status;
}

#endif
UmiStatus umi_diagnostic_snapshot_validate(const UmiDiagnosticSnapshot *snapshot,
                                           char *out_message,
                                           size_t capacity)
{
    const char *message = "valid";
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (snapshot == NULL) {
        message = "diagnostic snapshot is null";
        status = UMI_STATUS_INVALID_ARGUMENT;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (snapshot->struct_size != (uint32_t)sizeof(*snapshot) ||
               snapshot->api_version != UMI_DIAGNOSTIC_API_VERSION) {
        message = "diagnostic snapshot ABI version is unsupported";
        status = UMI_STATUS_INVALID_STATE;
    } else if (memchr(snapshot->id, 0, sizeof snapshot->id) == NULL ||
               memchr(snapshot->source, 0, sizeof snapshot->source) == NULL ||
               memchr(snapshot->code, 0, sizeof snapshot->code) == NULL ||
               memchr(snapshot->message, 0, sizeof snapshot->message) == NULL ||
               memchr(snapshot->detail, 0, sizeof snapshot->detail) == NULL ||
               memchr(snapshot->uri, 0, sizeof snapshot->uri) == NULL ||
               memchr(snapshot->owner, 0, sizeof snapshot->owner) == NULL ||
               memchr(snapshot->tags, 0, sizeof snapshot->tags) == NULL) {
        /* Validate inline text before model queries, sorting or presentation
         * treat it as a C string. Adjacent fields cannot terminate each other. */
        message = "diagnostic text is not terminated within its field capacity";
        status = UMI_STATUS_INVALID_ARGUMENT;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (snapshot->id[0] == '\0' || snapshot->message[0] == '\0') {
        message = "diagnostic id and message are required";
        status = UMI_STATUS_INVALID_ARGUMENT;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (snapshot->severity < UMI_DIAGNOSTIC_TRACE ||
               snapshot->severity > UMI_DIAGNOSTIC_FATAL ||
               snapshot->kind < UMI_DIAGNOSTIC_KIND_GENERAL ||
               snapshot->kind > UMI_DIAGNOSTIC_KIND_SECURITY) {
        message = "diagnostic severity or kind is outside the supported range";
        status = UMI_STATUS_INVALID_ARGUMENT;
    } else /* Apply this branch only when its contract condition is satisfied. */ if ((snapshot->line == 0U && snapshot->column != 0U) ||
               (snapshot->end_line != 0U && snapshot->line == 0U)) {
        message = "diagnostic source range is inconsistent";
        status = UMI_STATUS_INVALID_ARGUMENT;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "%s", message);
    }
    return status;
}

/* Provide the diagnostic emit operation used by this module and its client applications. */
void umi_diagnostic_emit(UmiDiagnosticSink sink, void *user_data,
                         UmiDiagnosticSeverity severity, const char *source,
                         const char *message, uint64_t correlation_id)
{
    UmiDiagnostic diagnostic;
    /* Apply this branch only when its contract condition is satisfied. */
    if (sink == 0) {
        return;
    }
    diagnostic.severity = severity;
    diagnostic.source = source != 0 ? source : "unknown";
    diagnostic.message = message != 0 ? message : "";
    diagnostic.correlation_id = correlation_id;
    sink(&diagnostic, user_data);
}
