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
#include "umicom/document/uri.h"
#include "umicom/platform/path.h"
#include <string.h>

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

/* Diagnostic selection is separate from opening a file. Hosts may restore the
 * navigation cursor after a failed open without changing any document. */
static int SourceAvailable(const UmiDiagnosticSnapshot *diagnostic)
{
    return diagnostic->resolved == 0 && diagnostic->line != 0U &&
        diagnostic->uri[0] != '\0' && diagnostic->uri[0] != '<' &&
        (strstr(diagnostic->uri, "://") == NULL ||
         strncmp(diagnostic->uri, "file:///", 8U) == 0);
}

UmiStatus UmiDiagnosticNavigationSource(UmiDiagnosticNavigation *navigation,
    const UmiDiagnosticModel *model, const UmiDiagnosticFilter *filter,
    int backwards, UmiDiagnosticSnapshot *outDiagnostic)
{
    UmiDiagnosticSnapshot chosen, edge;
    int found = 0, haveEdge = 0;
    if (navigation == NULL || model == NULL || filter == NULL || outDiagnostic == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t count = umi_diagnostic_model_count(model);
    for (size_t i = 0U; i < count; ++i) {
        UmiDiagnosticSnapshot item;
        UmiStatus status = umi_diagnostic_model_at(model, i, &item);
        if (status != UMI_STATUS_OK) return status;
        if (!SourceAvailable(&item) || !umi_diagnostic_filter_matches(filter, &item)) continue;
        if (!haveEdge || (backwards ? item.sequence > edge.sequence : item.sequence < edge.sequence)) {
            edge = item; haveEdge = 1;
        }
        int eligible = backwards
            ? (navigation->current_sequence == 0U || item.sequence < navigation->current_sequence)
            : item.sequence > navigation->current_sequence;
        if (eligible && (!found || (backwards ? item.sequence > chosen.sequence : item.sequence < chosen.sequence))) {
            chosen = item; found = 1;
        }
    }
    if (!found && navigation->wrap && haveEdge) { chosen = edge; found = 1; }
    if (!found) return UMI_STATUS_NOT_FOUND;
    *outDiagnostic = chosen; navigation->current_sequence = chosen.sequence;
    return UMI_STATUS_OK;
}

UmiStatus UmiDiagnosticOpenSource(UmiDocumentCoordinator *documents,
    const UmiDiagnosticSnapshot *diagnostic, const char *baseDirectory,
    size_t *outOffset)
{
    char decoded[UMI_PATH_CAPACITY], absolute[UMI_PATH_CAPACITY], viewId[UMI_UI_ID_CAPACITY];
    const char *path;
    UmiStatus status;
    if (documents == NULL || diagnostic == NULL ||
        umi_diagnostic_snapshot_validate(diagnostic, NULL, 0U) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!SourceAvailable(diagnostic)) return UMI_STATUS_NOT_FOUND;
    path = diagnostic->uri;
    if (strncmp(path, "file:///", 8U) == 0) {
        /* Encoded zero bytes must never crop the selected destination. */
        if (strstr(path, "%00") != NULL) return UMI_STATUS_INVALID_ARGUMENT;
        status = umi_document_uri_to_path(path, decoded, sizeof decoded);
        if (status != UMI_STATUS_OK) return status;
        path = decoded;
    }
    if (umi_path_is_absolute(path)) {
        status = umi_path_normalise(path, absolute, sizeof absolute);
    } else {
        if (baseDirectory == NULL || !umi_path_is_absolute(baseDirectory)) return UMI_STATUS_INVALID_ARGUMENT;
        status = umi_path_absolute(path, baseDirectory, absolute, sizeof absolute);
    }
    if (status != UMI_STATUS_OK) return status;
    status = umi_document_coordinator_open(documents, absolute, viewId, sizeof viewId);
    if (status != UMI_STATUS_OK) return status;
    return UmiDocumentCoordinatorGoToPosition(documents, diagnostic->line,
        diagnostic->column, outOffset);
}
