/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_layout_snapshot.c
 *
 * PURPOSE:
 *   Focused regression coverage for normalised layout snapshot state and deterministic layout fingerprints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../value_archive/revision_cases.h"
#include "umicom/frontend/conformance/layout_snapshot.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFcLayoutSnapshot s; umi_fc_layout_snapshot_init(&s,"code"); CHECK(umi_fc_layout_snapshot_append(&s,0.5)==UMI_STATUS_OK);
    UMI_TEST_REVISION_EXHAUSTION(UmiFcLayoutSnapshot, &s,
        (umi_fc_layout_snapshot_append(&s,0.5)));
 CHECK(umi_fc_layout_snapshot_fingerprint(&s)!=0U);
    return 0;
}
