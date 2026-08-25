/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_starter_dependency.c
 *
 * PURPOSE:
 *   Focused regression coverage for Declare explicit dependencies between reusable application starters.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/starter_dependency.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapStarterDependency d;
    CHECK(umi_bootstrap_starter_dependency_init(&d,"starter.web","starter.core",true)==UMI_STATUS_OK);
    CHECK(d.required);
    return 0;
}
