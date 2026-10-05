/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/workspace_switch.h
 *
 * PURPOSE:
 *   Define toolkit-neutral workspace switch contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_WORKSPACE_SWITCH_H
#define UMICOM_UI_MOSAIC_WORKSPACE_SWITCH_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ui mosaic workspace switch data shared with callers of this public
 * contract.
 */
typedef struct UmiUiMosaicWorkspaceSwitch {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char name[UMI_UI_MOSAIC_TEXT_CAPACITY];
    char layout_id[UMI_UI_MOSAIC_ID_CAPACITY];
    UmiUiMosaicApplication application;
    bool available;
} UmiUiMosaicWorkspaceSwitch;

/* Initializes a task/workspace descriptor without toolkit state. */
void umi_ui_mosaic_workspace_switch_init(UmiUiMosaicWorkspaceSwitch *value);
/* Configures identity, default layout and application ownership. */
UmiStatus umi_ui_mosaic_workspace_switch_set(UmiUiMosaicWorkspaceSwitch *value, const char *id, const char *name, const char *layout_id, UmiUiMosaicApplication application);
/* Validates that a selectable perspective/workspace has a valid layout. */
UmiStatus umi_ui_mosaic_workspace_switch_validate(const UmiUiMosaicWorkspaceSwitch *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_mosaic_workspace_switch_archive_encode(const UmiUiMosaicWorkspaceSwitch *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_mosaic_workspace_switch_archive_decode(const void *bytes, size_t byte_count,
    UmiUiMosaicWorkspaceSwitch *value);

#ifdef __cplusplus
}
#endif
#endif
