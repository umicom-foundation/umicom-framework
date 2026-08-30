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
UmiStatus umi_compiler_diagnostic_parse_line(const char *text,UmiCompilerDiagnostic *out_diagnostic)
{
    char severity[32U]; int matched;
    if (text == NULL || out_diagnostic == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_diagnostic,0,sizeof(*out_diagnostic));
    matched = sscanf(text,"%2047[^:]:%u:%u: %31[^:]: %511[^\n]",out_diagnostic->file,&out_diagnostic->line,&out_diagnostic->column,severity,out_diagnostic->message);
    if (matched != 5) return UMI_STATUS_PARSE_ERROR;
    if (strstr(severity,"fatal") != NULL) out_diagnostic->severity = UMI_COMPILER_DIAGNOSTIC_FATAL;
    else if (strstr(severity,"error") != NULL) out_diagnostic->severity = UMI_COMPILER_DIAGNOSTIC_ERROR;
    else if (strstr(severity,"warning") != NULL) out_diagnostic->severity = UMI_COMPILER_DIAGNOSTIC_WARNING;
    else out_diagnostic->severity = UMI_COMPILER_DIAGNOSTIC_NOTE;
    return UMI_STATUS_OK;
}
UmiStatus umi_compiler_diagnostic_set_add(UmiCompilerDiagnosticSet *set,const UmiCompilerDiagnostic *diagnostic) { if (set == NULL || diagnostic == NULL || diagnostic->message[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT; if (set->count >= UMI_COMPILER_MAX_DIAGNOSTICS) return UMI_STATUS_CAPACITY_EXCEEDED; set->items[set->count++] = *diagnostic; if (diagnostic->severity >= UMI_COMPILER_DIAGNOSTIC_ERROR) set->errors += 1U; else if (diagnostic->severity == UMI_COMPILER_DIAGNOSTIC_WARNING) set->warnings += 1U; set->revision += 1U; return UMI_STATUS_OK; }
const UmiCompilerDiagnostic *umi_compiler_diagnostic_set_at(const UmiCompilerDiagnosticSet *set,size_t index) { return set == NULL || index >= set->count ? NULL : &set->items[index]; }
