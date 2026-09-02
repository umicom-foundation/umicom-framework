/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/context_links.c
 *
 * PURPOSE:
 *   Implement the context links behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework | Desktop context-link bridge
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/context_links.h"

#include <stdio.h>
#include <string.h>

/*
 * Initialise desktop context links from caller-provided values so later operations receive
 * a known state.
 */
void umi_desktop_context_links_init(
    UmiDesktopContextLinks *links,
    UmiApplicationContextHub *hub)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (links == NULL) return;
    (void)memset(links, 0, sizeof(*links));
    links->hub = hub;
}

/*
 * Provide the desktop context links define operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_context_links_define(
    UmiDesktopContextLinks *links,
    const char *group_id,
    const char *colour_token,
    UmiUiWindowContextKind context_kind)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (links == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_window_group_define(
        &links->groups, group_id, colour_token, context_kind);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) links->revision += 1U;
    return status;
}

/*
 * Provide the desktop context links join operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_context_links_join(
    UmiDesktopContextLinks *links,
    const char *group_id,
    const char *window_id,
    UmiUiWindowGroupRole role)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (links == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_window_group_join(
        &links->groups, group_id, window_id, role);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) links->revision += 1U;
    return status;
}

/*
 * Provide the desktop context links leave operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_context_links_leave(
    UmiDesktopContextLinks *links,
    const char *group_id,
    const char *window_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (links == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_window_group_leave(&links->groups, group_id, window_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) links->revision += 1U;
    return status;
}

/*
 * Provide the desktop context links route operation used by this module and its client
 * applications.
 */
size_t umi_desktop_context_links_route(
    const UmiDesktopContextLinks *links,
    const char *group_id,
    const char *source_window_id,
    const char **out_window_ids,
    size_t capacity)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (links == NULL) return 0U;
    return umi_ui_window_group_route(
        &links->groups, group_id, source_window_id, out_window_ids, capacity);
}

/*
 * Provide the desktop context links publish operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_context_links_publish(
    UmiDesktopContextLinks *links,
    const char *group_id,
    const char *source_application_id,
    const char *context_type,
    const char *subject_id,
    const char *correlation_id)
{
    UmiApplicationContext context;
    int first;
    int second;
    int third;
    int fourth;
    int fifth = 0;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (links == NULL || links->hub == NULL || group_id == NULL ||
        source_application_id == NULL || context_type == NULL ||
        subject_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_ui_window_group_find(&links->groups, group_id) == NULL)
        return UMI_STATUS_NOT_FOUND;
    (void)memset(&context, 0, sizeof(context));
    first = snprintf(context.group_id, sizeof(context.group_id), "%s", group_id);
    second = snprintf(context.source_application_id,
                      sizeof(context.source_application_id), "%s",
                      source_application_id);
    third = snprintf(context.context_type, sizeof(context.context_type), "%s",
                     context_type);
    fourth = snprintf(context.subject_id, sizeof(context.subject_id), "%s",
                      subject_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (correlation_id != NULL && correlation_id[0] != '\0')
        fifth = snprintf(context.correlation_id,
                         sizeof(context.correlation_id), "%s", correlation_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (first < 0 || second < 0 || third < 0 || fourth < 0 || fifth < 0 ||
        (size_t)first >= sizeof(context.group_id) ||
        (size_t)second >= sizeof(context.source_application_id) ||
        (size_t)third >= sizeof(context.context_type) ||
        (size_t)fourth >= sizeof(context.subject_id) ||
        (correlation_id != NULL &&
         (size_t)fifth >= sizeof(context.correlation_id)))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    status = umi_application_context_hub_publish(links->hub, &context);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) links->revision += 1U;
    return status;
}
