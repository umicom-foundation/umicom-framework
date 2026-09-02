/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/tree_table.c
 *
 * PURPOSE:
 *   Implement tree-table composition over the canonical column catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/tree_table.h"
#include <string.h>
/*
 * Initialise ui ent tree table from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_ent_tree_table_init(UmiUiEntTreeTable *t,const char *id){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!t||!umi_ui_ent_id_valid(id))return UMI_STATUS_INVALID_ARGUMENT;memset(t,0,sizeof *t);umi_ui_ent_column_catalogue_init(&t->columns);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_copy_text(t->tree_column_id,sizeof t->tree_column_id,id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;t->show_connectors=1;return UMI_STATUS_OK;}
/* Check that ui ent tree table satisfies its contract before another service relies on it. */
int umi_ui_ent_tree_table_validate(const UmiUiEntTreeTable *t){return t&&umi_ui_ent_id_valid(t->tree_column_id);}
