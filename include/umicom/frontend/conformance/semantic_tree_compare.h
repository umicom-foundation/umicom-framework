/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/semantic_tree_compare.h
 *
 * PURPOSE:
 *   tree-shape, role and ordering comparison independent of native widget classes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_SEMANTIC_TREE_COMPARE_H
#define UMICOM_FRONTEND_CONFORMANCE_SEMANTIC_TREE_COMPARE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"
#include "umicom/frontend/conformance/semantic_tree.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFcSemanticTreeComparison { double count_score; double shape_score; double interactive_score; double total_score; } UmiFcSemanticTreeComparison;
UmiStatus umi_fc_semantic_tree_compare_run(const UmiFcSemanticTree *expected,const UmiFcSemanticTree *actual,UmiFcSemanticTreeComparison *out_comparison);

#ifdef __cplusplus
}
#endif
#endif
