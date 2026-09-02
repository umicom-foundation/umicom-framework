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
/**
 * Represent the ui ent tree path data shared with callers of this public contract.
 */
typedef struct UmiUiEntTreePath { char ids[UMI_UI_ENT_MAX_PATH][UMI_UI_ENT_ID_CAPACITY]; size_t count; } UmiUiEntTreePath;
/**
 * Initialise ui ent tree path from caller-provided values so later operations receive a
 * known state.
 */
void umi_ui_ent_tree_path_init(UmiUiEntTreePath *p);
/**
 * Add ui ent tree path only after its inputs and available capacity have been checked.
 */
UmiStatus umi_ui_ent_tree_path_append(UmiUiEntTreePath *p,const char *node_id);
/**
 * Provide the ui ent tree path parent operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_ent_tree_path_parent(UmiUiEntTreePath *p);

#endif
