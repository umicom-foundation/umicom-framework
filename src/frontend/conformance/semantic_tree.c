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

void umi_fc_semantic_tree_init(UmiFcSemanticTree *tree){if(tree!=NULL)*tree=(UmiFcSemanticTree){0};}
UmiStatus umi_fc_semantic_tree_add(UmiFcSemanticTree *tree,const char *id,const char *role,int32_t parent,int32_t order,bool interactive){UmiFcSemanticNode *n;UmiStatus st;if(tree==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(tree->count>=UMI_FC_MAX_TREE_NODES)return UMI_STATUS_CAPACITY_EXCEEDED;if(parent>=(int32_t)tree->count)return UMI_STATUS_INVALID_STATE;n=&tree->nodes[tree->count];*n=(UmiFcSemanticNode){0};st=umi_fc_copy_text(n->id,sizeof(n->id),id);if(st!=UMI_STATUS_OK)return st;st=umi_fc_copy_text(n->role,sizeof(n->role),role);if(st!=UMI_STATUS_OK)return st;n->parent=parent;n->order=order;n->interactive=interactive;tree->count++;return UMI_STATUS_OK;}
uint64_t umi_fc_semantic_tree_shape_hash(const UmiFcSemanticTree *tree){size_t i;uint64_t h=0U;if(tree==NULL)return 0U;for(i=0U;i<tree->count;++i){h=umi_fc_hash_combine(h,umi_fc_hash_text(tree->nodes[i].role));h=umi_fc_hash_combine(h,(uint64_t)(uint32_t)(tree->nodes[i].parent+1));h=umi_fc_hash_combine(h,(uint64_t)(uint32_t)tree->nodes[i].order);}return h;}
