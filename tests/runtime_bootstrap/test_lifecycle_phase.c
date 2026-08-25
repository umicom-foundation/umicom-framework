/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_lifecycle_phase.c
 *
 * PURPOSE:
 *   Focused regression coverage for Define stable ordering and text for service bootstrap and shutdown phases.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/lifecycle_phase.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    CHECK(strcmp(umi_bootstrap_lifecycle_phase_text(UMI_BOOTSTRAP_PHASE_START),"start")==0);
    CHECK(umi_bootstrap_lifecycle_phase_rank(UMI_BOOTSTRAP_PHASE_READY) >
          umi_bootstrap_lifecycle_phase_rank(UMI_BOOTSTRAP_PHASE_CREATE));
    return 0;
}
