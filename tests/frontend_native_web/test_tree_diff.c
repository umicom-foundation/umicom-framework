/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_tree_diff.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web tree diff.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "umicom/frontend/native_web/tree_diff.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
int main(void)
{
    static UmiNativeWebElementTree a,b; UmiNativeWebSemanticElement e; UmiNativeWebPatchBatch p; umi_native_web_element_tree_init(&a); umi_native_web_element_tree_init(&b); CHECK(umi_native_web_semantic_element_init(&e,"x","div")==UMI_STATUS_OK); strcpy(e.text,"new"); CHECK(umi_native_web_element_tree_upsert(&b,&e)==UMI_STATUS_OK); CHECK(umi_native_web_tree_diff(&a,&b,&p)==UMI_STATUS_OK); CHECK(p.count==1U); CHECK(p.items[0].kind==UMI_NATIVE_WEB_PATCH_INSERT_ELEMENT);
    return 0;
}
