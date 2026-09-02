/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_configuration_source.c
 *
 * PURPOSE:
 *   Implement the test configuration source behavior for
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
 * File: tests/runtime_bootstrap/test_configuration_source.c
 *
 * PURPOSE:
 *   Focused regression coverage for Describe trusted configuration sources and their precedence.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/configuration_source.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiBootstrapConfigurationSource s;
    CHECK(umi_bootstrap_configuration_source_init(&s,"env",20,true)==UMI_STATUS_OK);
    CHECK(s.trusted && s.precedence==20);
    return 0;
}
