/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostic_ui/navigation.c
 *
 * PURPOSE:
 *   Implement the navigation behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostic_ui/navigation.h"

void umi_diagnostic_navigation_init(UmiDiagnosticNavigation *navigation)
{
    if (navigation != NULL) {
        navigation->current_sequence = 0U;
        navigation->wrap = 1;
    }
}

static UmiStatus navigate(UmiDiagnosticNavigation *navigation,
                          const UmiDiagnosticModel *model,
                          const UmiDiagnosticFilter *filter,
                          int forward,
                          UmiDiagnosticSnapshot *out_diagnostic)
{
    UmiDiagnosticQuery query;
    UmiDiagnosticQueryResult result;
    size_t index;
    size_t selected = SIZE_MAX;
    UmiStatus status;
    if (navigation == NULL || model == NULL || filter == NULL || out_diagnostic == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_diagnostic_query_init(&query);
    query.filter = *filter;
    query.limit = UMI_DIAGNOSTIC_QUERY_RESULT_MAX;
    query.sort = UMI_DIAGNOSTIC_SORT_SEQUENCE_ASCENDING;
    status = umi_diagnostic_query_execute(model, &query, &result);
    if (status != UMI_STATUS_OK || result.count == 0U) return status == UMI_STATUS_OK ? UMI_STATUS_NOT_FOUND : status;
    if (forward != 0) {
        for (index = 0U; index < result.count; ++index) {
            if (result.items[index].sequence > navigation->current_sequence) {
                selected = index;
                break;
            }
        }
        if (selected == SIZE_MAX && navigation->wrap != 0) selected = 0U;
    } else {
        for (index = result.count; index > 0U; --index) {
            if (navigation->current_sequence == 0U ||
                result.items[index - 1U].sequence < navigation->current_sequence) {
                selected = index - 1U;
                break;
            }
        }
        if (selected == SIZE_MAX && navigation->wrap != 0) selected = result.count - 1U;
    }
    if (selected == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_diagnostic = result.items[selected];
    navigation->current_sequence = out_diagnostic->sequence;
    return UMI_STATUS_OK;
}

UmiStatus umi_diagnostic_navigation_next(UmiDiagnosticNavigation *navigation,
                                         const UmiDiagnosticModel *model,
                                         const UmiDiagnosticFilter *filter,
                                         UmiDiagnosticSnapshot *out_diagnostic)
{
    return navigate(navigation, model, filter, 1, out_diagnostic);
}

UmiStatus umi_diagnostic_navigation_previous(UmiDiagnosticNavigation *navigation,
                                             const UmiDiagnosticModel *model,
                                             const UmiDiagnosticFilter *filter,
                                             UmiDiagnosticSnapshot *out_diagnostic)
{
    return navigate(navigation, model, filter, 0, out_diagnostic);
}
