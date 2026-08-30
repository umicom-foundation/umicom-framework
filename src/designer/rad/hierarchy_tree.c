/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/hierarchy_tree.c
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
#include "umicom/designer/rad/hierarchy_tree.h"
#include <string.h>
UmiStatus umi_rad_hierarchy_tree_init(UmiRadHierarchyTree *t){if(t==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(t,0,sizeof *t);return UMI_STATUS_OK;}
const UmiRadHierarchyNode *umi_rad_hierarchy_tree_find(const UmiRadHierarchyTree *t,const char *id){size_t i;if(t==NULL||id==NULL)return NULL;for(i=0U;i<t->count;i++)if(strcmp(t->nodes[i].node_id,id)==0)return &t->nodes[i];return NULL;}
UmiStatus umi_rad_hierarchy_tree_add(UmiRadHierarchyTree *t,const UmiRadHierarchyNode *n){if(t==NULL||n==NULL||!umi_rad_hierarchy_node_is_valid(n))return UMI_STATUS_INVALID_ARGUMENT;if(t->count>=UMI_RAD_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;if(umi_rad_hierarchy_tree_find(t,n->node_id)!=NULL)return UMI_STATUS_ALREADY_EXISTS;t->nodes[t->count++]=*n;t->revision++;return UMI_STATUS_OK;}
