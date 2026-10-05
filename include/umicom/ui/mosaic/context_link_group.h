/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/context_link_group.h
 *
 * PURPOSE:
 *   Define toolkit-neutral context link group contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_CONTEXT_LINK_GROUP_H
#define UMICOM_UI_MOSAIC_CONTEXT_LINK_GROUP_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ui mosaic context link group data shared with callers of this public
 * contract.
 */
typedef struct UmiUiMosaicContextLinkGroup {
    char group_id[UMI_UI_MOSAIC_ID_CAPACITY];
    char context_type[UMI_UI_MOSAIC_ID_CAPACITY];
    char member_id[UMI_UI_MOSAIC_ID_CAPACITY];
    uint32_t colour_index;
    bool bidirectional;
} UmiUiMosaicContextLinkGroup;

/* Initializes one typed cross-panel context-link contract. */
void umi_ui_mosaic_context_link_group_init(UmiUiMosaicContextLinkGroup *value);
/* Configures group, context type and member identity. */
UmiStatus umi_ui_mosaic_context_link_group_set(UmiUiMosaicContextLinkGroup *value, const char *group_id, const char *context_type, const char *member_id);
/* Validates typed link identities before routing context. */
UmiStatus umi_ui_mosaic_context_link_group_validate(const UmiUiMosaicContextLinkGroup *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_mosaic_context_link_group_archive_encode(const UmiUiMosaicContextLinkGroup *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_mosaic_context_link_group_archive_decode(const void *bytes, size_t byte_count,
    UmiUiMosaicContextLinkGroup *value);

#ifdef __cplusplus
}
#endif
#endif
