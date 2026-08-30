/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/declarative/diagnostic.c
 *
 * PURPOSE:
 *   Implement bounded declarative diagnostics so parsing errors remain precise and testable.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This implementation works on the semantic .umiapp model.  It keeps parsing,
 * validation and generation independent of any particular graphical toolkit.
 */

#include "umicom/declarative/diagnostic.h"

#include <string.h>

void umi_decl_diagnostics_clear(UmiDeclDiagnosticList *list)
{
    if (list != NULL) (void)memset(list, 0, sizeof(*list));
}

UmiStatus umi_decl_diagnostics_add(UmiDeclDiagnosticList *list, UmiDeclDiagnosticSeverity severity, size_t line, size_t column, const char *code, const char *message)
{
    UmiDeclDiagnostic *item;
    if (list == NULL || code == NULL || message == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (list->count >= UMI_DECL_MAX_DIAGNOSTICS) return UMI_STATUS_CAPACITY_EXCEEDED;
    item = &list->items[list->count++];
    (void)memset(item, 0, sizeof(*item));
    item->severity = severity;
    item->line = line;
    item->column = column;
    (void)umi_decl_copy_text(item->code, sizeof(item->code), code);
    (void)umi_decl_copy_text(item->message, sizeof(item->message), message);
    return UMI_STATUS_OK;
}

size_t umi_decl_diagnostics_error_count(const UmiDeclDiagnosticList *list)
{
    size_t count = 0U;
    size_t i;
    if (list == NULL) return 0U;
    for (i = 0U; i < list->count; ++i) if (list->items[i].severity == UMI_DECL_DIAGNOSTIC_ERROR) count += 1U;
    return count;
}
