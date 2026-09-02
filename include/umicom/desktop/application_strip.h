/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/application_strip.h
 *
 * PURPOSE:
 *   Project the application runtime catalogue into a toolkit-neutral taskbar
 *   strip. The strip owns ordering and presentation state; GTK4, Qt, Wt and
 *   future adapters only render copied items and send semantic actions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_APPLICATION_STRIP_H
#define UMICOM_DESKTOP_APPLICATION_STRIP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "umicom/application/launcher.h"
#include "umicom/application/runtime_catalogue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named desktop application strip action values accepted by this public contract.
 */
typedef enum UmiDesktopApplicationStripAction {
    UMI_DESKTOP_APPLICATION_STRIP_LAUNCH_OR_ACTIVATE = 1,
    UMI_DESKTOP_APPLICATION_STRIP_TOGGLE_PIN = 2,
    UMI_DESKTOP_APPLICATION_STRIP_STOP = 3,
    UMI_DESKTOP_APPLICATION_STRIP_RESTART = 4
} UmiDesktopApplicationStripAction;

/**
 * Represent the desktop application strip item data shared with callers of this public
 * contract.
 */
typedef struct UmiDesktopApplicationStripItem {
    char application_id[UMI_APPLICATION_RUNTIME_ID_CAPACITY];
    char display_name[UMI_APPLICATION_RUNTIME_NAME_CAPACITY];
    char icon_resource_id[UMI_APPLICATION_RUNTIME_ID_CAPACITY];
    char icon_name[UMI_APPLICATION_RUNTIME_ID_CAPACITY];
    char default_layout_id[UMI_APPLICATION_RUNTIME_ID_CAPACITY];
    char taskbar_group[UMI_APPLICATION_RUNTIME_ID_CAPACITY];
    UmiApplicationRuntimeState state;
    UmiApplicationEntryKind entry_kind;
    bool pinned;
    bool running;
    bool active;
    bool attention;
    bool enabled;
    uint64_t revision;
} UmiDesktopApplicationStripItem;

/**
 * Represent the desktop application strip snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDesktopApplicationStripSnapshot {
    size_t item_count;
    size_t pinned_count;
    size_t running_count;
    size_t attention_count;
    char active_application_id[UMI_APPLICATION_RUNTIME_ID_CAPACITY];
    uint64_t revision;
} UmiDesktopApplicationStripSnapshot;

/**
 * Represent the desktop application strip data shared with callers of this public
 * contract.
 */
typedef struct UmiDesktopApplicationStrip UmiDesktopApplicationStrip;

/**
 * Initialise desktop application strip from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_desktop_application_strip_create(
    UmiApplicationRuntimeCatalogue *catalogue,
    UmiDesktopApplicationStrip **out_strip);
/**
 * Release or reset state held by desktop application strip so the same storage can be
 * reused safely.
 */
void umi_desktop_application_strip_destroy(
    UmiDesktopApplicationStrip *strip);

/**
 * Provide the desktop application strip refresh operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_application_strip_refresh(
    UmiDesktopApplicationStrip *strip);
/**
 * Find desktop application strip while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_desktop_application_strip_at(
    const UmiDesktopApplicationStrip *strip,
    size_t index,
    UmiDesktopApplicationStripItem *out_item);
/**
 * Find desktop application strip while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_desktop_application_strip_find(
    const UmiDesktopApplicationStrip *strip,
    const char *application_id,
    UmiDesktopApplicationStripItem *out_item);
/**
 * Provide the desktop application strip request operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_application_strip_request(
    UmiDesktopApplicationStrip *strip,
    UmiApplicationLauncher *launcher,
    const char *application_id,
    UmiDesktopApplicationStripAction action);
/**
 * Provide the desktop application strip snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_application_strip_snapshot(
    const UmiDesktopApplicationStrip *strip,
    UmiDesktopApplicationStripSnapshot *out_snapshot);

/**
 * Provide the desktop application strip action text operation used by this module and its
 * client applications.
 */
const char *umi_desktop_application_strip_action_text(
    UmiDesktopApplicationStripAction action);

#ifdef __cplusplus
}
#endif

#endif
