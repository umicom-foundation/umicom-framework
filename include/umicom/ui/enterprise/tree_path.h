/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/tree_path.h
 *
 * PURPOSE:
 *   Represent a bounded ancestor path for tree navigation and breadcrumbs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_TREE_PATH_H
#define UMICOM_UI_ENTERPRISE_TREE_PATH_H

#include "umicom/ui/enterprise/types.h"
typedef struct UmiUiEntTreePath { char ids[UMI_UI_ENT_MAX_PATH][UMI_UI_ENT_ID_CAPACITY]; size_t count; } UmiUiEntTreePath;
void umi_ui_ent_tree_path_init(UmiUiEntTreePath *p);
UmiStatus umi_ui_ent_tree_path_append(UmiUiEntTreePath *p,const char *node_id);
UmiStatus umi_ui_ent_tree_path_parent(UmiUiEntTreePath *p);

#endif
