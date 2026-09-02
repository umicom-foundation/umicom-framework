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
#ifdef __cplusplus
}
#endif

#endif
