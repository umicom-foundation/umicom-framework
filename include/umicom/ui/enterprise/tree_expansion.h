/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/tree_expansion.h
 *
 * PURPOSE:
 *   Track expanded tree-node identities separately from tree data.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_TREE_EXPANSION_H
#define UMICOM_UI_ENTERPRISE_TREE_EXPANSION_H

#include "umicom/ui/enterprise/types.h"
typedef struct UmiUiEntTreeExpansion { char ids[UMI_UI_ENT_MAX_CACHE][UMI_UI_ENT_ID_CAPACITY]; size_t count; } UmiUiEntTreeExpansion;
void umi_ui_ent_tree_expansion_init(UmiUiEntTreeExpansion *e);
UmiStatus umi_ui_ent_tree_expansion_set(UmiUiEntTreeExpansion *e,const char *id,int expanded);
int umi_ui_ent_tree_expansion_is_expanded(const UmiUiEntTreeExpansion *e,const char *id);

#endif
