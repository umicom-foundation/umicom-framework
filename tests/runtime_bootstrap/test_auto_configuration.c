/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_auto_configuration.c
 *
 * PURPOSE:
 *   Focused regression coverage for Describe conditional Framework-owned auto-configuration contributions.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/auto_configuration.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapAutoConfiguration c;
    CHECK(umi_bootstrap_auto_configuration_init(&c,"autoconfig.data",10,UMI_BOOTSTRAP_CONDITION_ALL)==UMI_STATUS_OK);
    CHECK(c.enabled && c.priority==10);
    return 0;
}
