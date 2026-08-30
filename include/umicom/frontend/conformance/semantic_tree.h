/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/semantic_tree.h
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
#ifndef UMICOM_FRONTEND_CONFORMANCE_SEMANTIC_TREE_H
#define UMICOM_FRONTEND_CONFORMANCE_SEMANTIC_TREE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFcSemanticNode { char id[UMI_FC_ID_CAPACITY]; char role[UMI_FC_ID_CAPACITY]; int32_t parent; int32_t order; bool interactive; } UmiFcSemanticNode;
typedef struct UmiFcSemanticTree { UmiFcSemanticNode nodes[UMI_FC_MAX_TREE_NODES]; size_t count; } UmiFcSemanticTree;
void umi_fc_semantic_tree_init(UmiFcSemanticTree *tree);
UmiStatus umi_fc_semantic_tree_add(UmiFcSemanticTree *tree,const char *id,const char *role,int32_t parent,int32_t order,bool interactive);
uint64_t umi_fc_semantic_tree_shape_hash(const UmiFcSemanticTree *tree);

#ifdef __cplusplus
}
#endif
#endif
