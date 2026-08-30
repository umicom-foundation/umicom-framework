/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_bootstrap_stage.c
 *
 * PURPOSE:
 *   Implement the test bootstrap stage behavior for
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
 * File: tests/runtime_bootstrap/test_bootstrap_stage.c
 *
 * PURPOSE:
 *   Focused regression coverage for Track validated bootstrap-stage state transitions.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/bootstrap_stage.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapStage s;
    CHECK(umi_bootstrap_stage_init(&s,"bootstrap.resolve",20)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_stage_transition(&s,UMI_BOOTSTRAP_STAGE_READY)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_stage_transition(&s,UMI_BOOTSTRAP_STAGE_RUNNING)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_stage_transition(&s,UMI_BOOTSTRAP_STAGE_COMPLETE)==UMI_STATUS_OK);
    return 0;
}
