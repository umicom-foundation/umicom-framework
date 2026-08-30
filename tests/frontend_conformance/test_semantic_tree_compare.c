/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_semantic_tree_compare.c
 *
 * PURPOSE:
 *   Focused regression coverage for tree-shape, role and ordering comparison independent of native widget classes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/semantic_tree_compare.h"
#include "umicom/frontend/conformance/semantic_tree.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcSemanticTree a,b; UmiFcSemanticTreeComparison c; umi_fc_semantic_tree_init(&a); umi_fc_semantic_tree_init(&b); CHECK(umi_fc_semantic_tree_add(&a,"r","workbench",-1,0,false)==UMI_STATUS_OK); CHECK(umi_fc_semantic_tree_add(&b,"x","workbench",-1,0,false)==UMI_STATUS_OK); CHECK(umi_fc_semantic_tree_compare_run(&a,&b,&c)==UMI_STATUS_OK); CHECK(c.total_score==1.0);
    return 0;
}
