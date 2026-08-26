/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/tree_projection.h
 *
 * PURPOSE:
 *   Store the flattened visible projection of a hierarchical tree.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_TREE_PROJECTION_H
#define UMICOM_UI_ENTERPRISE_TREE_PROJECTION_H

#include "umicom/ui/enterprise/tree_node.h"
typedef struct UmiUiEntTreeProjectionRow { char node_id[UMI_UI_ENT_ID_CAPACITY]; int32_t depth; } UmiUiEntTreeProjectionRow;
typedef struct UmiUiEntTreeProjection { UmiUiEntTreeProjectionRow rows[UMI_UI_ENT_MAX_CACHE]; size_t count; uint64_t revision; } UmiUiEntTreeProjection;
void umi_ui_ent_tree_projection_init(UmiUiEntTreeProjection *p);
UmiStatus umi_ui_ent_tree_projection_append(UmiUiEntTreeProjection *p,const UmiUiEntTreeNode *node);

#endif
