/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_configuration_property.c
 *
 * PURPOSE:
 *   Implement the test configuration property behavior for
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
 * File: tests/runtime_bootstrap/test_configuration_property.c
 *
 * PURPOSE:
 *   Focused regression coverage for Create bounded configuration properties with source precedence.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/configuration_property.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapConfigurationProperty p;
    CHECK(umi_bootstrap_configuration_property_init(&p,"app.mode","prod",10,false)==UMI_STATUS_OK);
    CHECK(strcmp(p.value,"prod")==0 && p.precedence==10);
    return 0;
}
