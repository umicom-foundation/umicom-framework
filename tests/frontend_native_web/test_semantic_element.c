/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_semantic_element.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web semantic element.
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
#include "umicom/frontend/native_web/semantic_element.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
int main(void)
{
    UmiNativeWebSemanticElement e; CHECK(umi_native_web_semantic_element_init(&e,"root","main") == UMI_STATUS_OK); CHECK(umi_native_web_semantic_element_validate(&e) == UMI_STATUS_OK);
    return 0;
}
