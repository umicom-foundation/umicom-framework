/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/tree_projection.c
 *
 * PURPOSE:
 *   Implement bounded visible tree projections.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/tree_projection.h"
#include <string.h>
void umi_ui_ent_tree_projection_init(UmiUiEntTreeProjection *p){if(p)memset(p,0,sizeof *p);}
UmiStatus umi_ui_ent_tree_projection_append(UmiUiEntTreeProjection *p,const UmiUiEntTreeNode *n){if(!p||!umi_ui_ent_tree_node_validate(n))return UMI_STATUS_INVALID_ARGUMENT;if(p->count>=UMI_UI_ENT_MAX_CACHE)return UMI_STATUS_CAPACITY_EXCEEDED;umi_ui_ent_copy_text(p->rows[p->count].node_id,sizeof p->rows[p->count].node_id,n->node_id);p->rows[p->count].depth=n->depth;p->count++;p->revision++;return UMI_STATUS_OK;}
