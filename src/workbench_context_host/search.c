/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/search.c
 *
 * PURPOSE:
 *   Perform case-insensitive endpoint substring search with deterministic scoring.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/search.h"

#include <ctype.h>
#include <string.h>

/* Provide the contains ci operation used by this module and its client applications. */
static int contains_ci(const char *text, const char *query)
{
    size_t query_length;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || query == NULL) return 0;
    query_length = strlen(query);
    /* Apply this branch only when its contract condition is satisfied. */
    if (query_length == 0U) return 1;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; text[index] != '\0'; ++index) {
        size_t query_index;
        /* Visit each bounded item once so every record receives the same rule. */
        for (query_index = 0U;
             query_index < query_length &&
             text[index + query_index] != '\0' &&
             tolower((unsigned char)text[index + query_index]) ==
                 tolower((unsigned char)query[query_index]);
             ++query_index) {
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (query_index == query_length) return 1;
    }
    return 0;
}

/*
 * Provide the workbench context host search operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_context_host_search(
    const UmiWorkbenchContextHostEndpointRegistry *registry,
    const char *query,
    UmiWorkbenchContextHostSearchResults *out_results)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || query == NULL || out_results == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    memset(out_results, 0, sizeof(*out_results));

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < registry->count &&
         out_results->count < UMI_WORKBENCH_CONTEXT_HOST_MAX_SEARCH_RESULTS;
         ++index) {
        const UmiWorkbenchContextHostEndpoint *endpoint =
            &registry->items[index];
        int score = 0;

        /* Apply this branch only when its contract condition is satisfied. */
        if (contains_ci(endpoint->display_name, query)) score += 50;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (contains_ci(endpoint->panel_id, query)) score += 40;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (contains_ci(endpoint->application_id, query)) score += 20;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (contains_ci(endpoint->group_id, query)) score += 10;

        /* Apply this branch only when its contract condition is satisfied. */
        if (score > 0) {
            UmiWorkbenchContextHostSearchResult *result =
                &out_results->items[out_results->count++];
            (void)umi_workbench_context_host_copy_text(
                result->endpoint_id,
                sizeof(result->endpoint_id),
                endpoint->endpoint_id);
            (void)umi_workbench_context_host_copy_text(
                result->panel_id,
                sizeof(result->panel_id),
                endpoint->panel_id);
            (void)umi_workbench_context_host_copy_text(
                result->display_name,
                sizeof(result->display_name),
                endpoint->display_name);
            (void)umi_workbench_context_host_copy_text(
                result->application_id,
                sizeof(result->application_id),
                endpoint->application_id);
            (void)umi_workbench_context_host_copy_text(
                result->group_id,
                sizeof(result->group_id),
                endpoint->group_id);
            result->role = endpoint->role;
            result->score = score;
        }
    }
    return UMI_STATUS_OK;
}
