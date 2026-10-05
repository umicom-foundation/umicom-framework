/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/workspace_catalogue.h
 *
 * PURPOSE:
 *   Define toolkit-neutral workspace catalogue contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_WORKSPACE_CATALOGUE_H
#define UMICOM_UI_MOSAIC_WORKSPACE_CATALOGUE_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ui mosaic workspace catalogue data shared with callers of this public
 * contract.
 */
typedef struct UmiUiMosaicWorkspaceCatalogue {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char name[UMI_UI_MOSAIC_TEXT_CAPACITY];
    char layout_id[UMI_UI_MOSAIC_ID_CAPACITY];
    UmiUiMosaicApplication application;
    bool available;
} UmiUiMosaicWorkspaceCatalogue;

/* Initializes a task/workspace descriptor without toolkit state. */
void umi_ui_mosaic_workspace_catalogue_init(UmiUiMosaicWorkspaceCatalogue *value);
/* Configures identity, default layout and application ownership. */
UmiStatus umi_ui_mosaic_workspace_catalogue_set(UmiUiMosaicWorkspaceCatalogue *value, const char *id, const char *name, const char *layout_id, UmiUiMosaicApplication application);
/* Validates that a selectable perspective/workspace has a valid layout. */
UmiStatus umi_ui_mosaic_workspace_catalogue_validate(const UmiUiMosaicWorkspaceCatalogue *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_mosaic_workspace_catalogue_archive_encode(const UmiUiMosaicWorkspaceCatalogue *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_mosaic_workspace_catalogue_archive_decode(const void *bytes, size_t byte_count,
    UmiUiMosaicWorkspaceCatalogue *value);

#ifdef __cplusplus
}
#endif
#endif
