/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/tree_projection.h
 *
 * PURPOSE:
 *   Store the flattened visible projection of a hierarchical tree.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_TREE_PROJECTION_H
#define UMICOM_UI_ENTERPRISE_TREE_PROJECTION_H

#include "umicom/ui/enterprise/tree_node.h"
/**
 * Represent the ui ent tree projection row data shared with callers of this public
 * contract.
 */
typedef struct UmiUiEntTreeProjectionRow { char node_id[UMI_UI_ENT_ID_CAPACITY]; int32_t depth; } UmiUiEntTreeProjectionRow;
/**
 * Represent the ui ent tree projection data shared with callers of this public contract.
 */
typedef struct UmiUiEntTreeProjection { UmiUiEntTreeProjectionRow rows[UMI_UI_ENT_MAX_CACHE]; size_t count; uint64_t revision; } UmiUiEntTreeProjection;
/**
 * Initialise ui ent tree projection from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_ent_tree_projection_init(UmiUiEntTreeProjection *p);
/**
 * Add ui ent tree projection only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ui_ent_tree_projection_append(UmiUiEntTreeProjection *p,const UmiUiEntTreeNode *node);

#endif
