/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_snapshot_compare.c
 *
 * PURPOSE:
 *   Focused regression coverage for semantic render snapshot comparison with tolerant volatile-field handling.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/snapshot_compare.h"
#include "umicom/frontend/conformance/render_snapshot.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcRenderSnapshot a={"x",10U,100U,5U,9U,1U},b={"x",10U,100U,5U,9U,2U}; UmiFcSnapshotComparison c; CHECK(umi_fc_snapshot_compare_run(&a,&b,&c)==UMI_STATUS_OK); CHECK(c.total_score==1.0);
    return 0;
}
