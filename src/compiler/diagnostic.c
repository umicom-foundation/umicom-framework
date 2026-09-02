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
