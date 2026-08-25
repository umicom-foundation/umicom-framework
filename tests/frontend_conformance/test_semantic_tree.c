/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_semantic_tree.c
 *
 * PURPOSE:
 *   Focused regression coverage for bounded semantic UI tree representation used for cross-renderer golden tests.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/semantic_tree.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcSemanticTree t; umi_fc_semantic_tree_init(&t); CHECK(umi_fc_semantic_tree_add(&t,"root","workbench",-1,0,false)==UMI_STATUS_OK); CHECK(umi_fc_semantic_tree_add(&t,"editor","editor",0,0,true)==UMI_STATUS_OK); CHECK(umi_fc_semantic_tree_shape_hash(&t)!=0U);
    return 0;
}
