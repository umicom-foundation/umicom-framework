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

/*
 * Initialise diagnostic navigation from caller-provided values so later operations receive
 * a known state.
 */
void umi_diagnostic_navigation_init(UmiDiagnosticNavigation *navigation)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (navigation != NULL) {
        navigation->current_sequence = 0U;
        navigation->wrap = 1;
    }
}

/* Provide the navigate operation used by this module and its client applications. */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (navigation == NULL || model == NULL || filter == NULL || out_diagnostic == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_diagnostic_query_init(&query);
    query.filter = *filter;
    query.limit = UMI_DIAGNOSTIC_QUERY_RESULT_MAX;
    query.sort = UMI_DIAGNOSTIC_SORT_SEQUENCE_ASCENDING;
    status = umi_diagnostic_query_execute(model, &query, &result);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (status != UMI_STATUS_OK || result.count == 0U) return status == UMI_STATUS_OK ? UMI_STATUS_NOT_FOUND : status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (forward != 0) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < result.count; ++index) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (result.items[index].sequence > navigation->current_sequence) {
                selected = index;
                break;
            }
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (selected == SIZE_MAX && navigation->wrap != 0) selected = 0U;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = result.count; index > 0U; --index) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (navigation->current_sequence == 0U ||
                result.items[index - 1U].sequence < navigation->current_sequence) {
                selected = index - 1U;
                break;
            }
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (selected == SIZE_MAX && navigation->wrap != 0) selected = result.count - 1U;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (selected == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_diagnostic = result.items[selected];
    navigation->current_sequence = out_diagnostic->sequence;
    return UMI_STATUS_OK;
}

/*
 * Provide the diagnostic navigation next operation used by this module and its client
 * applications.
 */
UmiStatus umi_diagnostic_navigation_next(UmiDiagnosticNavigation *navigation,
                                         const UmiDiagnosticModel *model,
                                         const UmiDiagnosticFilter *filter,
                                         UmiDiagnosticSnapshot *out_diagnostic)
{
    return navigate(navigation, model, filter, 1, out_diagnostic);
}

/*
 * Provide the diagnostic navigation previous operation used by this module and its client
 * applications.
 */
UmiStatus umi_diagnostic_navigation_previous(UmiDiagnosticNavigation *navigation,
                                             const UmiDiagnosticModel *model,
                                             const UmiDiagnosticFilter *filter,
                                             UmiDiagnosticSnapshot *out_diagnostic)
{
    return navigate(navigation, model, filter, 0, out_diagnostic);
}
