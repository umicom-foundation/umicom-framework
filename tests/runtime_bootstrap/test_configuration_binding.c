/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_configuration_binding.c
 *
 * PURPOSE:
 *   Implement the test configuration binding behavior for
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
 * File: tests/runtime_bootstrap/test_configuration_binding.c
 *
 * PURPOSE:
 *   Focused regression coverage for Convert validated textual configuration into primitive C values.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/configuration_binding.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    bool b=false; int64_t n=0;
    CHECK(umi_bootstrap_configuration_bind_bool("true",&b)==UMI_STATUS_OK && b);
    CHECK(umi_bootstrap_configuration_bind_i64("123",&n)==UMI_STATUS_OK && n==123);
    CHECK(umi_bootstrap_configuration_bind_i64("x",&n)==UMI_STATUS_PARSE_ERROR);
    return 0;
}
