/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_element_tree.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web element tree.
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
#include "umicom/frontend/native_web/element_tree.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiNativeWebElementTree t; UmiNativeWebSemanticElement a,b; umi_native_web_element_tree_init(&t); CHECK(umi_native_web_semantic_element_init(&a,"root","main") == UMI_STATUS_OK); CHECK(umi_native_web_element_tree_upsert(&t,&a) == UMI_STATUS_OK); CHECK(umi_native_web_semantic_element_init(&b,"child","section") == UMI_STATUS_OK); CHECK(umi_native_web_copy_text(b.parent_id,sizeof(b.parent_id),"root") == UMI_STATUS_OK); CHECK(umi_native_web_element_tree_upsert(&t,&b) == UMI_STATUS_OK); CHECK(t.count==2U); CHECK(umi_native_web_element_tree_remove(&t,"root") == UMI_STATUS_BUSY);
    return 0;
}
