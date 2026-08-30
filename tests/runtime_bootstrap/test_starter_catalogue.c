/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_starter_catalogue.c
 *
 * PURPOSE:
 *   Implement the test starter catalogue behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_starter_catalogue.c
 *
 * PURPOSE:
 *   Focused regression coverage for Maintain Framework-owned application starter definitions.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/starter_descriptor.h"
#include "umicom/runtime/bootstrap/starter_catalogue.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapStarterDescriptor s; UmiBootstrapStarterCatalogue c;
    umi_bootstrap_starter_catalogue_init(&c);
    CHECK(umi_bootstrap_starter_descriptor_init(&s,"starter.web","feature.web",5)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_starter_catalogue_add(&c,&s)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_starter_catalogue_find(&c,"starter.web")!=NULL);
    return 0;
}
