/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_content_type.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the content type Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/content_type.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    CHECK(umi_fabric_content_type_classify("application/json; charset=utf-8")==UMI_FABRIC_CONTENT_JSON); CHECK(umi_fabric_content_type_compatible(UMI_FABRIC_CONTENT_JSON,UMI_FABRIC_CONTENT_JSON));
    return 0;
}
