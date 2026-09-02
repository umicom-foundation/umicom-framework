/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/tree_node.c
 *
 * PURPOSE:
 *   Describe a lazily materialised node in an enterprise tree.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/tree_node.h"
#include <string.h>
/*
 * Initialise ui ent tree node from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_ent_tree_node_init(UmiUiEntTreeNode *value){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!value)return UMI_STATUS_INVALID_ARGUMENT;memset(value,0,sizeof *value);value->node_id[0]='\0';value->parent_id[0]='\0';value->label[0]='\0';value->depth=0;value->has_children=0;value->children_loaded=0;value->revision=0;return UMI_STATUS_OK;}
/* Check that ui ent tree node satisfies its contract before another service relies on it. */
int umi_ui_ent_tree_node_validate(const UmiUiEntTreeNode *value){return value!=NULL&&umi_ui_ent_id_valid(value->node_id)&&value->depth>=0;}
