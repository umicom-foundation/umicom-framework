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
/*
 * Initialise ui ent tree projection from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_ent_tree_projection_init(UmiUiEntTreeProjection *p){/* Apply this branch only when its contract condition is satisfied. */ if(p)memset(p,0,sizeof *p);}
/*
 * Add ui ent tree projection only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ui_ent_tree_projection_append(UmiUiEntTreeProjection *p,const UmiUiEntTreeNode *n){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!p||!umi_ui_ent_tree_node_validate(n))return UMI_STATUS_INVALID_ARGUMENT;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(p->count>=UMI_UI_ENT_MAX_CACHE)return UMI_STATUS_CAPACITY_EXCEEDED;umi_ui_ent_copy_text(p->rows[p->count].node_id,sizeof p->rows[p->count].node_id,n->node_id);p->rows[p->count].depth=n->depth;p->count++;p->revision++;return UMI_STATUS_OK;}
