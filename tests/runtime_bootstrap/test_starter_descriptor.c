/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_starter_descriptor.c
 *
 * PURPOSE:
 *   Implement the test starter descriptor behavior for
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
 * File: tests/runtime_bootstrap/test_starter_descriptor.c
 *
 * PURPOSE:
 *   Focused regression coverage for Describe reusable application starters built from feature packs and auto-configurations.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/starter_descriptor.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiBootstrapStarterDescriptor s;
    CHECK(umi_bootstrap_starter_descriptor_init(&s,"starter.web","feature.web",5)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_starter_descriptor_add_auto_configuration(&s,"autoconfig.web")==UMI_STATUS_OK);
    CHECK(s.auto_configuration_count==1U);
    return 0;
}
