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

/*
 * This implementation works on the semantic .umiapp model.  It keeps parsing,
 * validation and generation independent of any particular graphical toolkit.
 */

#include "umicom/declarative/diagnostic.h"

#include <string.h>

/*
 * Release or reset state held by decl diagnostics so the same storage can be reused
 * safely.
 */
void umi_decl_diagnostics_clear(UmiDeclDiagnosticList *list)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (list != NULL) (void)memset(list, 0, sizeof(*list));
}

/* Add decl diagnostics only after its inputs and available capacity have been checked. */
UmiStatus umi_decl_diagnostics_add(UmiDeclDiagnosticList *list, UmiDeclDiagnosticSeverity severity, size_t line, size_t column, const char *code, const char *message)
{
    UmiDeclDiagnostic *item;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (list == NULL || code == NULL || message == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/*
 * Return the number of records represented by decl diagnostics error without changing
 * their state.
 */
size_t umi_decl_diagnostics_error_count(const UmiDeclDiagnosticList *list)
{
    size_t count = 0U;
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (list == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < list->count; ++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (list->items[i].severity == UMI_DECL_DIAGNOSTIC_ERROR) count += 1U;
    return count;
}
