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

static int runtime_contains(const char *text, const char *pattern)
{
    size_t length = strlen(pattern);
    const char *cursor;
    for (cursor = text; *cursor != '\0'; ++cursor) {
        size_t index;
        for (index = 0U; index < length; ++index) {
            if (cursor[index] == '\0' ||
                tolower((unsigned char)cursor[index]) != tolower((unsigned char)pattern[index])) break;
        }
        if (index == length) return 1;
    }
    return 0;
}

static uint64_t runtime_hash(const char *text)
{
    uint64_t hash = UINT64_C(1469598103934665603);
    while (*text != '\0') {
        hash ^= (uint64_t)(unsigned char)*text++;
        hash *= UINT64_C(1099511628211);
    }
    return hash;
}

UmiStatus umi_runtime_diagnostic_parse(const UmiOutputRecord *output,
                                       UmiDiagnosticSnapshot *out_diagnostic,
                                       int *out_matched,
                                       void *user_data)
{
    UmiDiagnosticSeverity severity = UMI_DIAGNOSTIC_ERROR;
    const char *code = NULL;
    char identifier[UMI_DIAGNOSTIC_ID_CAPACITY];
    (void)user_data;
    if (output == NULL || out_diagnostic == NULL || out_matched == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_matched = 0;
    if (runtime_contains(output->text, "addresssanitizer")) code = "address-sanitizer";
    else if (runtime_contains(output->text, "undefinedbehaviorsanitizer") ||
             runtime_contains(output->text, "runtime error:")) code = "undefined-behavior";
    else if (runtime_contains(output->text, "segmentation fault") ||
             runtime_contains(output->text, "access violation")) code = "memory-fault";
    else if (runtime_contains(output->text, "assertion failed")) code = "assertion";
    else if (runtime_contains(output->text, "unhandled exception") ||
             runtime_contains(output->text, "terminate called")) code = "unhandled-exception";
    else if (output->stream == UMI_OUTPUT_STREAM_DEBUG &&
             runtime_contains(output->text, "error:")) code = "runtime-error";
    if (code == NULL) return UMI_STATUS_OK;
    if (strcmp(code, "runtime-error") != 0 && strcmp(code, "undefined-behavior") != 0) {
        severity = UMI_DIAGNOSTIC_FATAL;
    }
    (void)snprintf(identifier, sizeof(identifier), "runtime-%016" PRIx64,
                   runtime_hash(output->text) ^ output->correlation_id);
    if (umi_diagnostic_snapshot_init(out_diagnostic, identifier, severity,
                                     UMI_DIAGNOSTIC_KIND_RUNTIME,
                                     output->source[0] != '\0' ? output->source : "runtime",
                                     output->text) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)snprintf(out_diagnostic->code, sizeof(out_diagnostic->code), "%s", code);
    out_diagnostic->correlation_id = output->correlation_id;
    out_diagnostic->timestamp_ns = output->timestamp_ns;
    *out_matched = 1;
    return UMI_STATUS_OK;
}

UmiDiagnosticParser umi_runtime_diagnostic_parser(void)
{
    UmiDiagnosticParser parser = { "umicom.runtime", 50, umi_runtime_diagnostic_parse, NULL };
    return parser;
}
