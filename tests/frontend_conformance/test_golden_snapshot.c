/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_golden_snapshot.c
 *
 * PURPOSE:
 *   Focused regression coverage for versioned golden semantic snapshot metadata and acceptance rules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/golden_snapshot.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFcGoldenSnapshot g; CHECK(umi_fc_golden_snapshot_make("code",1U,123U,0.95,&g)==UMI_STATUS_OK); CHECK(umi_fc_golden_snapshot_accepts(&g,123U,0.0)); CHECK(!umi_fc_golden_snapshot_accepts(&g,999U,0.8));
    return 0;
}
