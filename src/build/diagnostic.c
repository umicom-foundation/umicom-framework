/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/diagnostic.c
 *
 * PURPOSE:
 *   Implement bounded diagnostic retention and severity queries.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/diagnostic.h"

#include <string.h>

void umi_build_diagnostic_list_init(UmiBuildDiagnosticList *list)
{
    if (list != NULL) {
        (void)memset(list, 0, sizeof(*list));
    }
}

UmiStatus umi_build_diagnostic_list_add(UmiBuildDiagnosticList *list,
                                        const UmiBuildDiagnostic *diagnostic)
{
    if (list == NULL || diagnostic == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (list->count >= UMI_BUILD_MAX_DIAGNOSTICS) {
        list->dropped += 1U;
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    list->items[list->count++] = *diagnostic;
    return UMI_STATUS_OK;
}

const UmiBuildDiagnostic *umi_build_diagnostic_list_at(
    const UmiBuildDiagnosticList *list,
    size_t index)
{
    return list != NULL && index < list->count
        ? &list->items[index]
        : NULL;
}

size_t umi_build_diagnostic_list_count_severity(
    const UmiBuildDiagnosticList *list,
    UmiBuildDiagnosticSeverity minimum)
{
    size_t index;
    size_t count = 0U;
    if (list == NULL) {
        return 0U;
    }
    for (index = 0U; index < list->count; ++index) {
        if (list->items[index].severity >= minimum) {
            count += 1U;
        }
    }
    return count;
}
