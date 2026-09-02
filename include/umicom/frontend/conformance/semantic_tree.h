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

/**
 * Represent the fc semantic node data shared with callers of this public contract.
 */
typedef struct UmiFcSemanticNode { char id[UMI_FC_ID_CAPACITY]; char role[UMI_FC_ID_CAPACITY]; int32_t parent; int32_t order; bool interactive; } UmiFcSemanticNode;
/**
 * Represent the fc semantic tree data shared with callers of this public contract.
 */
typedef struct UmiFcSemanticTree { UmiFcSemanticNode nodes[UMI_FC_MAX_TREE_NODES]; size_t count; } UmiFcSemanticTree;
/**
 * Initialise fc semantic tree from caller-provided values so later operations receive a
 * known state.
 */
void umi_fc_semantic_tree_init(UmiFcSemanticTree *tree);
/**
 * Add fc semantic tree only after its inputs and available capacity have been checked.
 */
UmiStatus umi_fc_semantic_tree_add(UmiFcSemanticTree *tree,const char *id,const char *role,int32_t parent,int32_t order,bool interactive);
/**
 * Provide the fc semantic tree shape hash operation used by this module and its client
 * applications.
 */
uint64_t umi_fc_semantic_tree_shape_hash(const UmiFcSemanticTree *tree);

#ifdef __cplusplus
}
#endif
#endif
