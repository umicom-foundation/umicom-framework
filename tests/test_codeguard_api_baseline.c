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
#include "value_archive/construction_cases.h"

#include "umicom/codeguard/api_baseline.h"

/* Verify additions, removals and signature changes retain distinct evidence. */

/* A rejected replacement must preserve the previously accepted api baseline value. */
static int UmiCodeGuardApiBaselineConstructionCases(void)
{
    UmiCodeGuardApiBaseline *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_codeguard_api_baseline_init(value, "framework", 1U, 0U) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->name) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_codeguard_api_baseline_init(value, oversized, 1U, 0U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_codeguard_api_baseline_init(value, value->name, 1U, 0U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->name, "framework") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void)
{
    if (UmiCodeGuardApiBaselineConstructionCases() != 0) return 1;

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
