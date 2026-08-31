/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_codeguard_api_baseline.c
 *
 * PURPOSE:
 *   Verify public API baselines distinguish additions, removals and signature
 *   changes while retaining bounded, reviewable compatibility evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/codeguard/api_baseline.h"

/* Verify additions, removals and signature changes retain distinct evidence. */
int main(void)
{
    UmiCodeGuardApiBaseline expected;
    UmiCodeGuardApiBaseline current;
    UmiCodeGuardApiComparison comparison;
    /* Expected describes the published contract; current describes the candidate. */
    assert(umi_codeguard_api_baseline_init(
        &expected, "framework", 1U, 0U) == UMI_STATUS_OK);
    assert(umi_codeguard_api_baseline_init(
        &current, "framework", 1U, 1U) == UMI_STATUS_OK);
    /* The fixture changes one signature, retires one deprecated symbol and adds one. */
    assert(umi_codeguard_api_baseline_add(
        &expected, "umi_open", 11U, false) == UMI_STATUS_OK);
    assert(umi_codeguard_api_baseline_add(
        &expected, "umi_old", 12U, true) == UMI_STATUS_OK);
    assert(umi_codeguard_api_baseline_add(
        &current, "umi_open", 99U, false) == UMI_STATUS_OK);
    assert(umi_codeguard_api_baseline_add(
        &current, "umi_new", 13U, false) == UMI_STATUS_OK);
    /* Comparison must retain all three facts rather than stopping at the first one. */
    assert(umi_codeguard_api_baseline_compare(
        &expected, &current, &comparison) == UMI_STATUS_OK);
    assert(comparison.signature_changes == 1U);
    assert(comparison.removals == 1U);
    assert(comparison.additions == 1U);
    /* A changed signature remains breaking even when other changes are compatible. */
    assert(umi_codeguard_api_comparison_breaking(&comparison));
    return 0;
}
