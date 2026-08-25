/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_optional_binding.c
 *
 * PURPOSE:
 *   Focused regression coverage for Resolve optional dependency presence and fallback decisions explicitly.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/optional_binding.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    int fallback=1, actual=2;
    CHECK(umi_bootstrap_optional_binding_choose(&actual,&fallback)==&actual);
    CHECK(umi_bootstrap_optional_binding_choose(NULL,&fallback)==&fallback);
    return 0;
}
