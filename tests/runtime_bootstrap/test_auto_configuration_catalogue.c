/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_auto_configuration_catalogue.c
 *
 * PURPOSE:
 *   Implement the test auto configuration catalogue behavior for
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
 * File: tests/runtime_bootstrap/test_auto_configuration_catalogue.c
 *
 * PURPOSE:
 *   Focused regression coverage for Maintain a deterministic catalogue of auto-configuration definitions.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/auto_configuration.h"
#include "umicom/runtime/bootstrap/auto_configuration_catalogue.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiBootstrapAutoConfiguration c; UmiBootstrapAutoConfigurationCatalogue cat;
    umi_bootstrap_auto_configuration_catalogue_init(&cat);
    CHECK(umi_bootstrap_auto_configuration_init(&c,"autoconfig.data",10,UMI_BOOTSTRAP_CONDITION_ALL)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_auto_configuration_catalogue_add(&cat,&c)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_auto_configuration_catalogue_find(&cat,"autoconfig.data")!=NULL);
    return 0;
}
