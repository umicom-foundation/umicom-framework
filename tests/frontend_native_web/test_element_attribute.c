/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_native_web/test_element_attribute.c
 *
 * PURPOSE:
 *   Focused regression coverage for native-web element attribute.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "umicom/frontend/native_web/element_attribute.h"
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s at %s:%d\n", #expr, __FILE__, __LINE__); return 1; } } while (0)
int main(void)
{
    UmiNativeWebSemanticElement e; CHECK(umi_native_web_semantic_element_init(&e,"x","div") == UMI_STATUS_OK); CHECK(umi_native_web_element_attribute_set(&e,"aria-label","Panel") == UMI_STATUS_OK); CHECK(strcmp(umi_native_web_element_attribute_get(&e,"aria-label"),"Panel") == 0); CHECK(umi_native_web_element_attribute_remove(&e,"aria-label") == UMI_STATUS_OK);
    return 0;
}
