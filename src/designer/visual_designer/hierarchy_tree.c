/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/hierarchy_tree.c
 *
 * PURPOSE:
 *   Maintain a bounded component hierarchy with deterministic lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/hierarchy_tree.h"
#include <string.h>
/*
 * Initialise visual designer hierarchy tree from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_hierarchy_tree_init(UmiRadHierarchyTree *t){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(t,0,sizeof *t);return UMI_STATUS_OK;}
/*
 * Find visual designer hierarchy tree while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiRadHierarchyNode *umi_rad_hierarchy_tree_find(const UmiRadHierarchyTree *t,const char *id){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t==NULL||id==NULL)return NULL;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<t->count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(t->nodes[i].node_id,id)==0)return &t->nodes[i];return NULL;}
/* Add visual designer hierarchy tree only after its inputs and available capacity have been checked. */
UmiStatus umi_rad_hierarchy_tree_add(UmiRadHierarchyTree *t,const UmiRadHierarchyNode *n){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t==NULL||n==NULL||!umi_rad_hierarchy_node_is_valid(n))return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t->count>=UMI_RAD_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_rad_hierarchy_tree_find(t,n->node_id)!=NULL)return UMI_STATUS_ALREADY_EXISTS;t->nodes[t->count++]=*n;t->revision++;return UMI_STATUS_OK;}
