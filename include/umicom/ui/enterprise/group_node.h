/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/group_node.h
 *
 * PURPOSE:
 *   Represent one grouped range projected by an enterprise grid.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_GROUP_NODE_H
#define UMICOM_UI_ENTERPRISE_GROUP_NODE_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/ui/enterprise/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui ent group node data shared with callers of this public contract.
 */
typedef struct UmiUiEntGroupNode {
    char group_id[UMI_UI_ENT_ID_CAPACITY];
    char label[UMI_UI_ENT_TEXT_CAPACITY];
    size_t first_row;
    size_t row_count;
    int32_t depth;
    int expanded;
} UmiUiEntGroupNode;
/**
 * Initialise ui ent group node from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_ent_group_node_init(UmiUiEntGroupNode *value);
/**
 * Check that ui ent group node satisfies its contract before another service relies on it.
 */
int umi_ui_ent_group_node_validate(const UmiUiEntGroupNode *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_ent_group_node_archive_encode(const UmiUiEntGroupNode *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_ent_group_node_archive_decode(const void *bytes, size_t byte_count,
    UmiUiEntGroupNode *value);

#ifdef __cplusplus
}
#endif

#endif
