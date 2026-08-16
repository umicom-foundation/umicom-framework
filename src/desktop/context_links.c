/*-----------------------------------------------------------------------------
 * Umicom Framework | Desktop context-link bridge
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/context_links.h"

#include <stdio.h>
#include <string.h>

void umi_desktop_context_links_init(
    UmiDesktopContextLinks *links,
    UmiApplicationContextHub *hub)
{
    if (links == NULL) return;
    (void)memset(links, 0, sizeof(*links));
    links->hub = hub;
}

UmiStatus umi_desktop_context_links_define(
    UmiDesktopContextLinks *links,
    const char *group_id,
    const char *colour_token,
    UmiUiWindowContextKind context_kind)
{
    UmiStatus status;
    if (links == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_window_group_define(
        &links->groups, group_id, colour_token, context_kind);
    if (status == UMI_STATUS_OK) links->revision += 1U;
    return status;
}

UmiStatus umi_desktop_context_links_join(
    UmiDesktopContextLinks *links,
    const char *group_id,
    const char *window_id,
    UmiUiWindowGroupRole role)
{
    UmiStatus status;
    if (links == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_window_group_join(
        &links->groups, group_id, window_id, role);
    if (status == UMI_STATUS_OK) links->revision += 1U;
    return status;
}

UmiStatus umi_desktop_context_links_leave(
    UmiDesktopContextLinks *links,
    const char *group_id,
    const char *window_id)
{
    UmiStatus status;
    if (links == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_window_group_leave(&links->groups, group_id, window_id);
    if (status == UMI_STATUS_OK) links->revision += 1U;
    return status;
}

size_t umi_desktop_context_links_route(
    const UmiDesktopContextLinks *links,
    const char *group_id,
    const char *source_window_id,
    const char **out_window_ids,
    size_t capacity)
{
    if (links == NULL) return 0U;
    return umi_ui_window_group_route(
        &links->groups, group_id, source_window_id, out_window_ids, capacity);
}

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
    if (links == NULL || links->hub == NULL || group_id == NULL ||
        source_application_id == NULL || context_type == NULL ||
        subject_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
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
    if (correlation_id != NULL && correlation_id[0] != '\0')
        fifth = snprintf(context.correlation_id,
                         sizeof(context.correlation_id), "%s", correlation_id);
    if (first < 0 || second < 0 || third < 0 || fourth < 0 || fifth < 0 ||
        (size_t)first >= sizeof(context.group_id) ||
        (size_t)second >= sizeof(context.source_application_id) ||
        (size_t)third >= sizeof(context.context_type) ||
        (size_t)fourth >= sizeof(context.subject_id) ||
        (correlation_id != NULL &&
         (size_t)fifth >= sizeof(context.correlation_id)))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    status = umi_application_context_hub_publish(links->hub, &context);
    if (status == UMI_STATUS_OK) links->revision += 1U;
    return status;
}
