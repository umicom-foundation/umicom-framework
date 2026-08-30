/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_codeguard_api_baseline_v2.c
 *
 * PURPOSE:
 *   Implement the test codeguard api baseline v2 behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | API baseline v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "umicom/codeguard/api_baseline.h"
int main(void)
{
    UmiCodeGuardApiBaseline expected;
    UmiCodeGuardApiBaseline current;
    UmiCodeGuardApiComparison comparison;
    assert(umi_codeguard_api_baseline_init(&expected,"framework",1U,0U) == UMI_STATUS_OK);
    assert(umi_codeguard_api_baseline_init(&current,"framework",1U,1U) == UMI_STATUS_OK);
    assert(umi_codeguard_api_baseline_add(&expected,"umi_open",11U,false) == UMI_STATUS_OK);
    assert(umi_codeguard_api_baseline_add(&expected,"umi_old",12U,true) == UMI_STATUS_OK);
    assert(umi_codeguard_api_baseline_add(&current,"umi_open",99U,false) == UMI_STATUS_OK);
    assert(umi_codeguard_api_baseline_add(&current,"umi_new",13U,false) == UMI_STATUS_OK);
    assert(umi_codeguard_api_baseline_compare(&expected,&current,&comparison) == UMI_STATUS_OK);
    assert(comparison.signature_changes == 1U && comparison.removals == 1U && comparison.additions == 1U);
    assert(umi_codeguard_api_comparison_breaking(&comparison));
    return 0;
}
