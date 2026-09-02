/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/browser_query.c
 *
 * PURPOSE:
 *   Implement small, inspectable query syntax for Layout Browser search.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/browser_query.h"
#include "internal.h"


/*
 * Initialise workbench designer browser query from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_designer_browser_query_init(
    UmiWorkbenchDesignerBrowserQuery *query)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL) return;
    (void)memset(query, 0, sizeof(*query));
}

/*
 * Provide the browser query append text operation used by this module and its client
 * applications.
 */
static UmiStatus browser_query_append_text(
    UmiWorkbenchDesignerBrowserQuery *query,
    const char *token)
{
    size_t current = strlen(query->text);
    size_t token_length = strlen(token);
    size_t required = current + token_length + (current > 0U ? 2U : 1U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (required > sizeof(query->text)) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Apply this branch only when its contract condition is satisfied. */
    if (current > 0U) query->text[current++] = ' ';
    (void)memcpy(query->text + current, token, token_length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Read workbench designer browser query into validated module state and return a status
 * when input cannot be used.
 */
UmiStatus umi_workbench_designer_browser_query_parse(
    const char *text,
    UmiWorkbenchDesignerBrowserQuery *out_query)
{
    char buffer[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    char *cursor;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || out_query == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_workbench_designer_browser_query_init(out_query);
    status = umi_workbench_designer_copy_text(buffer, sizeof(buffer), text);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    cursor = strtok(buffer, " \t\r\n");
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (cursor != NULL) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strncmp(cursor, "category:", 9U) == 0) {
            status = umi_workbench_designer_copy_text(out_query->category, sizeof(out_query->category), cursor + 9U);
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strncmp(cursor, "owner:", 6U) == 0) {
            status = umi_workbench_designer_copy_text(out_query->owner, sizeof(out_query->owner), cursor + 6U);
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strncmp(cursor, "workspace:", 10U) == 0) {
            status = umi_workbench_designer_copy_text(out_query->workspace, sizeof(out_query->workspace), cursor + 10U);
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strncmp(cursor, "tag:", 4U) == 0) {
            status = umi_workbench_designer_copy_text(out_query->tag, sizeof(out_query->tag), cursor + 4U);
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(cursor, "is:built-in") == 0) {
            out_query->built_in_only = true;
            status = UMI_STATUS_OK;
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(cursor, "is:user") == 0) {
            out_query->user_only = true;
            status = UMI_STATUS_OK;
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(cursor, "is:locked") == 0) {
            out_query->locked_only = true;
            status = UMI_STATUS_OK;
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(cursor, "is:shared") == 0) {
            out_query->shared_only = true;
            status = UMI_STATUS_OK;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            status = browser_query_append_text(out_query, cursor);
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        cursor = strtok(NULL, " \t\r\n");
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (out_query->built_in_only && out_query->user_only) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
