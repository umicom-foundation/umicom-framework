/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_element_index.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web element index.
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
#include "umicom/frontend/native_web/element_index.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiNativeWebElementTree t; UmiNativeWebSemanticElement e; UmiNativeWebElementIndex idx; size_t pos=99U; umi_native_web_element_tree_init(&t); CHECK(umi_native_web_semantic_element_init(&e,"root","main")==UMI_STATUS_OK); CHECK(umi_native_web_element_tree_upsert(&t,&e)==UMI_STATUS_OK); CHECK(umi_native_web_element_index_build(&idx,&t)==UMI_STATUS_OK); CHECK(umi_native_web_element_index_find(&idx,"root",&pos)==UMI_STATUS_OK); CHECK(pos==0U);
    return 0;
}
