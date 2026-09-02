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

/**
 * Represent the desktop context links data shared with callers of this public contract.
 */
typedef struct UmiDesktopContextLinks {
    UmiUiWindowGroupStore groups;
    UmiApplicationContextHub *hub;
    uint64_t revision;
} UmiDesktopContextLinks;

/**
 * Initialise desktop context links from caller-provided values so later operations receive
 * a known state.
 */
void umi_desktop_context_links_init(
    UmiDesktopContextLinks *links,
    UmiApplicationContextHub *hub);
/**
 * Provide the desktop context links define operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_context_links_define(
    UmiDesktopContextLinks *links,
    const char *group_id,
    const char *colour_token,
    UmiUiWindowContextKind context_kind);
/**
 * Provide the desktop context links join operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_context_links_join(
    UmiDesktopContextLinks *links,
    const char *group_id,
    const char *window_id,
    UmiUiWindowGroupRole role);
/**
 * Provide the desktop context links leave operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_context_links_leave(
    UmiDesktopContextLinks *links,
    const char *group_id,
    const char *window_id);
/**
 * Provide the desktop context links route operation used by this module and its client
 * applications.
 */
size_t umi_desktop_context_links_route(
    const UmiDesktopContextLinks *links,
    const char *group_id,
    const char *source_window_id,
    const char **out_window_ids,
    size_t capacity);
/**
 * Provide the desktop context links publish operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_context_links_publish(
    UmiDesktopContextLinks *links,
    const char *group_id,
    const char *source_application_id,
    const char *context_type,
    const char *subject_id,
    const char *correlation_id);

#endif
