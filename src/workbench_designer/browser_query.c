/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/browser_query.c
 *
 * PURPOSE:
 *   Implement small, inspectable query syntax for Layout Browser search.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/browser_query.h"
#include "internal.h"


void umi_workbench_designer_browser_query_init(
    UmiWorkbenchDesignerBrowserQuery *query)
{
    if (query == NULL) return;
    (void)memset(query, 0, sizeof(*query));
}

static UmiStatus browser_query_append_text(
    UmiWorkbenchDesignerBrowserQuery *query,
    const char *token)
{
    size_t current = strlen(query->text);
    size_t token_length = strlen(token);
    size_t required = current + token_length + (current > 0U ? 2U : 1U);
    if (required > sizeof(query->text)) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (current > 0U) query->text[current++] = ' ';
    (void)memcpy(query->text + current, token, token_length + 1U);
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_browser_query_parse(
    const char *text,
    UmiWorkbenchDesignerBrowserQuery *out_query)
{
    char buffer[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    char *cursor;
    UmiStatus status;
    if (text == NULL || out_query == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_workbench_designer_browser_query_init(out_query);
    status = umi_workbench_designer_copy_text(buffer, sizeof(buffer), text);
    if (status != UMI_STATUS_OK) return status;
    cursor = strtok(buffer, " \t\r\n");
    while (cursor != NULL) {
        if (strncmp(cursor, "category:", 9U) == 0) {
            status = umi_workbench_designer_copy_text(out_query->category, sizeof(out_query->category), cursor + 9U);
        } else if (strncmp(cursor, "owner:", 6U) == 0) {
            status = umi_workbench_designer_copy_text(out_query->owner, sizeof(out_query->owner), cursor + 6U);
        } else if (strncmp(cursor, "workspace:", 10U) == 0) {
            status = umi_workbench_designer_copy_text(out_query->workspace, sizeof(out_query->workspace), cursor + 10U);
        } else if (strncmp(cursor, "tag:", 4U) == 0) {
            status = umi_workbench_designer_copy_text(out_query->tag, sizeof(out_query->tag), cursor + 4U);
        } else if (strcmp(cursor, "is:built-in") == 0) {
            out_query->built_in_only = true;
            status = UMI_STATUS_OK;
        } else if (strcmp(cursor, "is:user") == 0) {
            out_query->user_only = true;
            status = UMI_STATUS_OK;
        } else if (strcmp(cursor, "is:locked") == 0) {
            out_query->locked_only = true;
            status = UMI_STATUS_OK;
        } else if (strcmp(cursor, "is:shared") == 0) {
            out_query->shared_only = true;
            status = UMI_STATUS_OK;
        } else {
            status = browser_query_append_text(out_query, cursor);
        }
        if (status != UMI_STATUS_OK) return status;
        cursor = strtok(NULL, " \t\r\n");
    }
    if (out_query->built_in_only && out_query->user_only) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
