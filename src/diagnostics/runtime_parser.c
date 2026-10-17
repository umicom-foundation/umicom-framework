/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/runtime_parser.c
 *
 * PURPOSE:
 *   Implement the runtime parser behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/runtime_parser.h"

#include <ctype.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

/* Provide the runtime contains operation used by this module and its client applications. */
static int runtime_contains(const char *text, const char *pattern)
{
    size_t length = strlen(pattern);
    const char *cursor;
    /* Visit each bounded item once so every record receives the same rule. */
    for (cursor = text; *cursor != '\0'; ++cursor) {
        size_t index;
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < length; ++index) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (cursor[index] == '\0' ||
                tolower((unsigned char)cursor[index]) != tolower((unsigned char)pattern[index])) break;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index == length) return 1;
    }
    return 0;
}


/* Native toolkit checks are actionable diagnostics even when the process
 * continues. Restrict matching to the toolkit's domain-and-level marker;
 * ordinary output mentioning a warning or an assertion is not enough. */
static const char *runtime_native_diagnostic(const char *text,
    UmiDiagnosticSeverity *severity)
{
    static const char *const domains[] = {
        "Gtk", "Gdk", "Gsk", "GLib", "GLib-GObject", "GLib-GIO"
    };
    for (size_t index = 0U; index < sizeof domains / sizeof domains[0]; ++index) {
        char marker[48];
        (void)snprintf(marker, sizeof marker, "%s-CRITICAL **:", domains[index]);
        if (runtime_contains(text, marker)) {
            *severity = UMI_DIAGNOSTIC_ERROR;
            return "native-toolkit-critical";
        }
        (void)snprintf(marker, sizeof marker, "%s-WARNING **:", domains[index]);
        if (runtime_contains(text, marker)) {
            *severity = UMI_DIAGNOSTIC_WARNING;
            return "native-toolkit-warning";
        }
    }
    return NULL;
}

/* Provide the runtime hash operation used by this module and its client applications. */
static uint64_t runtime_hash(const char *text)
{
    uint64_t hash = UINT64_C(1469598103934665603);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*text != '\0') {
        hash ^= (uint64_t)(unsigned char)*text++;
        hash *= UINT64_C(1099511628211);
    }
    return hash;
}

/*
 * Read runtime diagnostic into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_runtime_diagnostic_parse(const UmiOutputRecord *output,
                                       UmiDiagnosticSnapshot *out_diagnostic,
                                       int *out_matched,
                                       void *user_data)
{
    UmiDiagnosticSeverity severity = UMI_DIAGNOSTIC_ERROR;
    const char *code = NULL;
    char identifier[UMI_DIAGNOSTIC_ID_CAPACITY];
    (void)user_data;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (output == NULL || out_diagnostic == NULL || out_matched == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_matched = 0;
    /* A toolkit critical check may itself mention an assertion. Its native
     * level takes precedence over the generic crash-text fallback. */
    code = runtime_native_diagnostic(output->text, &severity);
    if (code == NULL) {
    /* Apply this branch only when its contract condition is satisfied. */
    if (runtime_contains(output->text, "addresssanitizer")) code = "address-sanitizer";
    else /* Apply this branch only when its contract condition is satisfied. */ if (runtime_contains(output->text, "undefinedbehaviorsanitizer") ||
             runtime_contains(output->text, "runtime error:")) code = "undefined-behavior";
    else /* Apply this branch only when its contract condition is satisfied. */ if (runtime_contains(output->text, "segmentation fault") ||
             runtime_contains(output->text, "access violation")) code = "memory-fault";
    else /* Apply this branch only when its contract condition is satisfied. */ if (runtime_contains(output->text, "assertion failed")) code = "assertion";
    else /* Apply this branch only when its contract condition is satisfied. */ if (runtime_contains(output->text, "unhandled exception") ||
             runtime_contains(output->text, "terminate called")) code = "unhandled-exception";
    else /* Apply this branch only when its contract condition is satisfied. */ if (output->stream == UMI_OUTPUT_STREAM_DEBUG &&
             runtime_contains(output->text, "error:")) code = "runtime-error";
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (code == NULL) return UMI_STATUS_OK;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    /* Preserve the previous fatal classification for review. Native warnings
     * and critical checks carry their actual level; neither proves termination. */
#if 0
    if (strcmp(code, "runtime-error") != 0 && strcmp(code, "undefined-behavior") != 0) {
        severity = UMI_DIAGNOSTIC_FATAL;
    }
#endif
    if (strcmp(code, "runtime-error") != 0 && strcmp(code, "undefined-behavior") != 0 &&
        strcmp(code, "native-toolkit-critical") != 0 && strcmp(code, "native-toolkit-warning") != 0) {
        severity = UMI_DIAGNOSTIC_FATAL;
    }
    (void)snprintf(identifier, sizeof(identifier), "runtime-%016" PRIx64,
                   runtime_hash(output->text) ^ output->correlation_id);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    /* The snapshot constructor distinguishes allocation and capacity errors.
     * Keep that evidence intact instead of reporting every failure as capacity.
     * The old conversion remains available for engineering comparison. */
#if 0
    if (umi_diagnostic_snapshot_init(out_diagnostic, identifier, severity,
                                     UMI_DIAGNOSTIC_KIND_RUNTIME,
                                     output->source[0] != '\0' ? output->source : "runtime",
                                     output->text) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
#endif
    UmiStatus status = umi_diagnostic_snapshot_init(out_diagnostic, identifier, severity,
        UMI_DIAGNOSTIC_KIND_RUNTIME, output->source[0] != '\0' ? output->source : "runtime",
        output->text);
    if (status != UMI_STATUS_OK) return status;
    (void)snprintf(out_diagnostic->code, sizeof(out_diagnostic->code), "%s", code);
    out_diagnostic->correlation_id = output->correlation_id;
    out_diagnostic->timestamp_ns = output->timestamp_ns;
    *out_matched = 1;
    return UMI_STATUS_OK;
}

/*
 * Provide the runtime diagnostic parser operation used by this module and its client
 * applications.
 */
UmiDiagnosticParser umi_runtime_diagnostic_parser(void)
{
    UmiDiagnosticParser parser = { "umicom.runtime", 50, umi_runtime_diagnostic_parse, NULL };
    return parser;
}
