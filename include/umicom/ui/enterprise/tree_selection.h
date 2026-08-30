/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/tree_selection.h
 *
 * PURPOSE:
 *   Maintain selected tree-node identities independently of visible projection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_TREE_SELECTION_H
#define UMICOM_UI_ENTERPRISE_TREE_SELECTION_H

#include "umicom/ui/enterprise/types.h"
typedef struct UmiUiEntTreeSelection { UmiUiEntSelectionMode mode; char ids[UMI_UI_ENT_MAX_SELECTIONS][UMI_UI_ENT_ID_CAPACITY]; size_t count; } UmiUiEntTreeSelection;
void umi_ui_ent_tree_selection_init(UmiUiEntTreeSelection *s,UmiUiEntSelectionMode mode);
UmiStatus umi_ui_ent_tree_selection_add(UmiUiEntTreeSelection *s,const char *id);
int umi_ui_ent_tree_selection_contains(const UmiUiEntTreeSelection *s,const char *id);

#endif
