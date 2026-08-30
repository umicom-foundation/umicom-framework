/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_production/test_launch_stage.c
 *
 * PURPOSE:
 *   Implement the test launch stage behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework application production test | launch_stage | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "test_fixture.h"
int main(void) {
    assert(strcmp(umi_application_production_launch_stage_text(UMI_APPLICATION_PRODUCTION_STAGE_START), "start") == 0);
    assert(strcmp(umi_application_production_state_text(UMI_APPLICATION_PRODUCTION_READY), "ready") == 0);
    assert(strcmp(umi_application_production_gate_text(UMI_APPLICATION_PRODUCTION_GATE_MANIFEST), "manifest") == 0);
    return 0;
}

