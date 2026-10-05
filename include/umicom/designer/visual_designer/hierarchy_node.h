/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/hierarchy_node.h
 *
 * PURPOSE:
 *   Represent one node in the designer object hierarchy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_HIERARCHY_NODE_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_HIERARCHY_NODE_H
#include "umicom/designer/visual_designer/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer hierarchy node data shared with callers of this public contract.
 */
typedef struct UmiRadHierarchyNode {
    char node_id[UMI_RAD_ID_CAPACITY];
    char parent_id[UMI_RAD_ID_CAPACITY];
    int32_t order;
    bool expanded;
} UmiRadHierarchyNode;
/**
 * Initialise visual designer hierarchy node from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_hierarchy_node_init(UmiRadHierarchyNode *item);
/**
 * Check that visual designer hierarchy node satisfies its contract before another service relies on
 * it.
 */
int umi_rad_hierarchy_node_is_valid(const UmiRadHierarchyNode *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_rad_hierarchy_node_archive_encode(const UmiRadHierarchyNode *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_rad_hierarchy_node_archive_decode(const void *bytes, size_t byte_count,
    UmiRadHierarchyNode *value);

#ifdef __cplusplus
}
#endif
#endif
