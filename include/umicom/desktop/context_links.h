/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/context_links.h
 *
 * PURPOSE:
 *   Publish the public context links contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_CONTEXT_LINKS_H
#define UMICOM_DESKTOP_CONTEXT_LINKS_H

#include "umicom/application/context_hub.h"
#include "umicom/ui/window_group.h"

typedef struct UmiDesktopContextLinks {
    UmiUiWindowGroupStore groups;
    UmiApplicationContextHub *hub;
    uint64_t revision;
} UmiDesktopContextLinks;

void umi_desktop_context_links_init(
    UmiDesktopContextLinks *links,
    UmiApplicationContextHub *hub);
UmiStatus umi_desktop_context_links_define(
    UmiDesktopContextLinks *links,
    const char *group_id,
    const char *colour_token,
    UmiUiWindowContextKind context_kind);
UmiStatus umi_desktop_context_links_join(
    UmiDesktopContextLinks *links,
    const char *group_id,
    const char *window_id,
    UmiUiWindowGroupRole role);
UmiStatus umi_desktop_context_links_leave(
    UmiDesktopContextLinks *links,
    const char *group_id,
    const char *window_id);
size_t umi_desktop_context_links_route(
    const UmiDesktopContextLinks *links,
    const char *group_id,
    const char *source_window_id,
    const char **out_window_ids,
    size_t capacity);
UmiStatus umi_desktop_context_links_publish(
    UmiDesktopContextLinks *links,
    const char *group_id,
    const char *source_application_id,
    const char *context_type,
    const char *subject_id,
    const char *correlation_id);

#endif
