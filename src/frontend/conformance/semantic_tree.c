/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/semantic_tree.c
 *
 * PURPOSE:
 *   bounded semantic UI tree representation used for cross-renderer golden tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/semantic_tree.h"

/*
 * Initialise fc semantic tree from caller-provided values so later operations receive a
 * known state.
 */
void umi_fc_semantic_tree_init(UmiFcSemanticTree *tree){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(tree!=NULL)*tree=(UmiFcSemanticTree){0};}
/* Add fc semantic tree only after its inputs and available capacity have been checked. */
UmiStatus umi_fc_semantic_tree_add(UmiFcSemanticTree *tree,const char *id,const char *role,int32_t parent,int32_t order,bool interactive){UmiFcSemanticNode *n;UmiStatus st;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(tree==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(tree->count>=UMI_FC_MAX_TREE_NODES)return UMI_STATUS_CAPACITY_EXCEEDED;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(parent>=(int32_t)tree->count)return UMI_STATUS_INVALID_STATE;n=&tree->nodes[tree->count];*n=(UmiFcSemanticNode){0};st=umi_fc_copy_text(n->id,sizeof(n->id),id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st;st=umi_fc_copy_text(n->role,sizeof(n->role),role);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st;n->parent=parent;n->order=order;n->interactive=interactive;tree->count++;return UMI_STATUS_OK;}
/*
 * Provide the fc semantic tree shape hash operation used by this module and its client
 * applications.
 */
uint64_t umi_fc_semantic_tree_shape_hash(const UmiFcSemanticTree *tree){size_t i;uint64_t h=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(tree==NULL)return 0U;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<tree->count;++i){h=umi_fc_hash_combine(h,umi_fc_hash_text(tree->nodes[i].role));h=umi_fc_hash_combine(h,(uint64_t)(uint32_t)(tree->nodes[i].parent+1));h=umi_fc_hash_combine(h,(uint64_t)(uint32_t)tree->nodes[i].order);}return h;}
