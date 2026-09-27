/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/diagnostic.c
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
/* Umicom Framework | Compiler diagnostics | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/diagnostic.h"
#include <stdio.h>
#include <string.h>
#include "umicom/diagnostics/compiler_parser.h"

/* Superseded: the original scanf grammar stops at a Windows drive colon,
 * silently crops messages and cannot validate overflowing numeric positions.
 * The public functions below now adapt the canonical Framework grammar and
 * check fixed-array boundaries. Retain this original code for comparison. */
#if 0
/*
 * Provide the compiler diagnostic parse line operation used by this module and its client
 * applications.
 */
UmiStatus umi_compiler_diagnostic_parse_line(const char *text,UmiCompilerDiagnostic *out_diagnostic)
{
    char severity[32U]; int matched;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || out_diagnostic == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_diagnostic,0,sizeof(*out_diagnostic));
    matched = sscanf(text,"%2047[^:]:%u:%u: %31[^:]: %511[^\n]",out_diagnostic->file,&out_diagnostic->line,&out_diagnostic->column,severity,out_diagnostic->message);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (matched != 5) return UMI_STATUS_PARSE_ERROR;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (strstr(severity,"fatal") != NULL) out_diagnostic->severity = UMI_COMPILER_DIAGNOSTIC_FATAL;
    else /* Protect caller-owned memory by checking that required state is available before it is used. */ if (strstr(severity,"error") != NULL) out_diagnostic->severity = UMI_COMPILER_DIAGNOSTIC_ERROR;
    else /* Protect caller-owned memory by checking that required state is available before it is used. */ if (strstr(severity,"warning") != NULL) out_diagnostic->severity = UMI_COMPILER_DIAGNOSTIC_WARNING;
    /* Use this fallback path when the earlier condition does not apply. */
    else out_diagnostic->severity = UMI_COMPILER_DIAGNOSTIC_NOTE;
    return UMI_STATUS_OK;
}
/*
 * Add compiler diagnostic set only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_compiler_diagnostic_set_add(UmiCompilerDiagnosticSet *set,const UmiCompilerDiagnostic *diagnostic) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (set == NULL || diagnostic == NULL || diagnostic->message[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (set->count >= UMI_COMPILER_MAX_DIAGNOSTICS) return UMI_STATUS_CAPACITY_EXCEEDED; set->items[set->count++] = *diagnostic; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (diagnostic->severity >= UMI_COMPILER_DIAGNOSTIC_ERROR) set->errors += 1U; else /* Protect caller-owned memory by checking that required state is available before it is used. */ if (diagnostic->severity == UMI_COMPILER_DIAGNOSTIC_WARNING) set->warnings += 1U; set->revision += 1U; return UMI_STATUS_OK; }
/*
 * Find compiler diagnostic set while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiCompilerDiagnostic *umi_compiler_diagnostic_set_at(const UmiCompilerDiagnosticSet *set,size_t index) { return set == NULL || index >= set->count ? NULL : &set->items[index]; }
#endif

/* Framework build and compiler consumers now share one location grammar.
 * Retain the older ABI: ordinary output is PARSE_ERROR, output is cleared
 * after valid pointers, and oversized fields fail instead of being cropped. */
UmiStatus umi_compiler_diagnostic_parse_line(const char *text,
    UmiCompilerDiagnostic *out_diagnostic)
{
    UmiCompilerDiagnosticFields fields;
    UmiStatus status;
    if (text == NULL || out_diagnostic == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_diagnostic, 0, sizeof *out_diagnostic);
    status = UmiCompilerDiagnosticParseText(text, &fields);
    if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_PARSE_ERROR;
    if (status != UMI_STATUS_OK) return status;
    if (strlen(fields.path) >= sizeof out_diagnostic->file ||
        strlen(fields.code) >= sizeof out_diagnostic->code ||
        strlen(fields.message) >= sizeof out_diagnostic->message ||
        fields.line > UINT32_MAX || fields.column > UINT32_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    strcpy(out_diagnostic->file, fields.path);
    strcpy(out_diagnostic->code, fields.code);
    strcpy(out_diagnostic->message, fields.message);
    out_diagnostic->line = (uint32_t)fields.line;
    out_diagnostic->column = (uint32_t)fields.column;
    switch (fields.severity) {
        case UMI_DIAGNOSTIC_FATAL: out_diagnostic->severity = UMI_COMPILER_DIAGNOSTIC_FATAL; break;
        case UMI_DIAGNOSTIC_ERROR: out_diagnostic->severity = UMI_COMPILER_DIAGNOSTIC_ERROR; break;
        case UMI_DIAGNOSTIC_WARNING: out_diagnostic->severity = UMI_COMPILER_DIAGNOSTIC_WARNING; break;
        default: out_diagnostic->severity = UMI_COMPILER_DIAGNOSTIC_NOTE; break;
    }
    return UMI_STATUS_OK;
}

static int CompilerDiagnosticValid(const UmiCompilerDiagnostic *diagnostic)
{
    return diagnostic != NULL && diagnostic->message[0] != '\0' &&
        diagnostic->severity >= UMI_COMPILER_DIAGNOSTIC_NOTE &&
        diagnostic->severity <= UMI_COMPILER_DIAGNOSTIC_FATAL &&
        memchr(diagnostic->file, 0, sizeof diagnostic->file) != NULL &&
        memchr(diagnostic->code, 0, sizeof diagnostic->code) != NULL &&
        memchr(diagnostic->message, 0, sizeof diagnostic->message) != NULL &&
        (diagnostic->line != 0U || diagnostic->column == 0U);
}

UmiStatus umi_compiler_diagnostic_set_add(UmiCompilerDiagnosticSet *set,
    const UmiCompilerDiagnostic *diagnostic)
{
    if (set == NULL || !CompilerDiagnosticValid(diagnostic)) return UMI_STATUS_INVALID_ARGUMENT;
    if (set->count > UMI_COMPILER_MAX_DIAGNOSTICS || set->errors > set->count ||
        set->warnings > set->count || set->errors > set->count - set->warnings)
        return UMI_STATUS_INVALID_STATE;
    if (set->count == UMI_COMPILER_MAX_DIAGNOSTICS || set->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    set->items[set->count++] = *diagnostic;
    if (diagnostic->severity >= UMI_COMPILER_DIAGNOSTIC_ERROR) ++set->errors;
    else if (diagnostic->severity == UMI_COMPILER_DIAGNOSTIC_WARNING) ++set->warnings;
    ++set->revision;
    return UMI_STATUS_OK;
}

const UmiCompilerDiagnostic *umi_compiler_diagnostic_set_at(
    const UmiCompilerDiagnosticSet *set, size_t index)
{
    return set == NULL || set->count > UMI_COMPILER_MAX_DIAGNOSTICS || index >= set->count
        ? NULL : &set->items[index];
}
