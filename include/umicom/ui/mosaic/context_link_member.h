/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/context_link_member.h
 *
 * PURPOSE:
 *   Define toolkit-neutral context link member contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_CONTEXT_LINK_MEMBER_H
#define UMICOM_UI_MOSAIC_CONTEXT_LINK_MEMBER_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ui mosaic context link member data shared with callers of this public
 * contract.
 */
typedef struct UmiUiMosaicContextLinkMember {
    char group_id[UMI_UI_MOSAIC_ID_CAPACITY];
    char context_type[UMI_UI_MOSAIC_ID_CAPACITY];
    char member_id[UMI_UI_MOSAIC_ID_CAPACITY];
    uint32_t colour_index;
    bool bidirectional;
} UmiUiMosaicContextLinkMember;

/* Initializes one typed cross-panel context-link contract. */
void umi_ui_mosaic_context_link_member_init(UmiUiMosaicContextLinkMember *value);
/* Configures group, context type and member identity. */
UmiStatus umi_ui_mosaic_context_link_member_set(UmiUiMosaicContextLinkMember *value, const char *group_id, const char *context_type, const char *member_id);
/* Validates typed link identities before routing context. */
UmiStatus umi_ui_mosaic_context_link_member_validate(const UmiUiMosaicContextLinkMember *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_mosaic_context_link_member_archive_encode(const UmiUiMosaicContextLinkMember *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_mosaic_context_link_member_archive_decode(const void *bytes, size_t byte_count,
    UmiUiMosaicContextLinkMember *value);

#ifdef __cplusplus
}
#endif
#endif
