/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/group_node.c
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
#include "umicom/ui/enterprise/group_node.h"
#include <string.h>
/*
 * Initialise ui ent group node from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_ent_group_node_init(UmiUiEntGroupNode *value){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!value)return UMI_STATUS_INVALID_ARGUMENT;memset(value,0,sizeof *value);value->group_id[0]='\0';value->label[0]='\0';value->first_row=0;value->row_count=0;value->depth=0;value->expanded=0;return UMI_STATUS_OK;}
/* Check that ui ent group node satisfies its contract before another service relies on it. */
int umi_ui_ent_group_node_validate(const UmiUiEntGroupNode *value){return value!=NULL&&umi_ui_ent_id_valid(value->group_id)&&value->row_count>0U;}
