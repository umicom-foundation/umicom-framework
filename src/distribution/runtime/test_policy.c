/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/test_policy.c
 * PURPOSE:
 *   Apply explicit per-test decisions without hiding population drift.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: src/distribution/runtime/test_policy.c
 * Purpose: Apply explicit per-test decisions without hiding population drift.
 *---------------------------------------------------------------------------*/
#include "test_policy_internal.h"
#include <string.h>

static const UmiReleaseInventoryRecord *FindTest(const UmiReleaseInventory *inventory, const char *name)
{
    size_t first = 0U, last = UmiReleaseInventoryCount(inventory);
    while (first < last) {
        size_t middle = first + (last - first) / 2U;
        const UmiReleaseInventoryRecord *row = UmiReleaseInventoryAt(inventory, middle);
        /* Inventory sorts its test kind after all other records. */
        if (row->kind != UMI_RELEASE_INVENTORY_TEST) { first = middle + 1U; continue; }
        int order = strcmp(row->identity, name);
        if (order == 0) return row;
        if (order < 0) first = middle + 1U; else last = middle;
    }
    return NULL;
}
typedef struct Reporter { UmiReleaseTestPolicyFindingFn finding; void *context; } Reporter;
static void Report(const Reporter *reporter, UmiReleaseTestPolicyFinding finding, const char *name)
{ if (reporter->finding != NULL) reporter->finding(reporter->context, finding, name); }
static void Difference(void *context, UmiReleaseInventoryDifference difference, const char *name)
{
    const Reporter *reporter = context;
    Report(reporter, difference == UMI_RELEASE_INVENTORY_TEST_MISSING ?
        UMI_RELEASE_POLICY_MISSING_REGISTRATION : UMI_RELEASE_POLICY_ADDED_REGISTRATION, name);
}
UmiStatus UmiReleaseTestPolicyAssess(const UmiReleaseInventory *configured,
    const UmiReleaseInventory *observed, const UmiReleaseTestPolicy *policy,
    UmiReleaseTestPolicyFindingFn finding, void *context, UmiReleaseTestPolicyAssessment *outAssessment)
{
    if (configured == NULL || observed == NULL || policy == NULL || outAssessment == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(policy->generation, UmiReleaseInventoryGeneration(configured)) != 0 ||
        strcmp(policy->sourceRoot, UmiReleaseInventorySourceRoot(configured)) != 0 ||
        strcmp(policy->buildRoot, UmiReleaseInventoryBuildRoot(configured)) != 0 ||
        strcmp(policy->configuration, UmiReleaseInventoryConfiguration(configured)) != 0) return UMI_STATUS_INVALID_STATE;
    /* Validate before publishing callbacks or output. The existing comparator
     * remains authoritative for exact population identity and context. */
    UmiReleaseInventoryComparison comparison = {0};
    UmiStatus status = UmiReleaseInventoryCompareTests(configured, observed, NULL, NULL, &comparison);
    if (status != UMI_STATUS_OK) return status;
    Reporter reporter = { finding, context };
    UmiReleaseTestPolicyAssessment result = {0};
    result.expectedTests = comparison.expectedTests; result.policyRules = policy->count;
    result.missingRegistrations = comparison.missingTests; result.addedRegistrations = comparison.addedTests;
    (void)UmiReleaseInventoryCompareTests(configured, observed, Difference, &reporter, &comparison);
    for (size_t i = 0U; i < UmiReleaseInventoryCount(configured); ++i) {
        const UmiReleaseInventoryRecord *expected = UmiReleaseInventoryAt(configured, i);
        if (expected->kind != UMI_RELEASE_INVENTORY_TEST) continue;
        const UmiReleaseTestRule *rule = UmiReleaseTestPolicyFind(policy, expected->identity);
        if (rule == NULL) {
            ++result.missingRules; Report(&reporter, UMI_RELEASE_POLICY_MISSING_RULE, expected->identity); continue;
        }
        if (rule->requirement == UMI_RELEASE_TEST_UNASSIGNED) {
            ++result.unassignedTests; Report(&reporter, UMI_RELEASE_POLICY_UNASSIGNED, rule->name); continue;
        }
        const UmiReleaseInventoryRecord *actual = FindTest(observed, rule->name);
        bool unavailable = actual == NULL || strcmp(actual->state, "registered") != 0;
        if (rule->requirement == UMI_RELEASE_TEST_REQUIRED) {
            ++result.requiredTests;
            if (unavailable) { ++result.requiredUnavailable; Report(&reporter, UMI_RELEASE_POLICY_REQUIRED_UNAVAILABLE, rule->name); }
        } else {
            ++result.optionalTests;
            if (unavailable) { ++result.optionalUnavailable; Report(&reporter, UMI_RELEASE_POLICY_OPTIONAL_UNAVAILABLE, rule->name); }
        }
    }
    for (size_t i = 0U; i < policy->count; ++i) {
        if (FindTest(configured, policy->rules[i].name) == NULL) {
            ++result.orphanRules; Report(&reporter, UMI_RELEASE_POLICY_ORPHAN_RULE, policy->rules[i].name);
        }
    }
    result.readyForExecution = result.requiredTests != 0U && comparison.namesMatch &&
        result.unassignedTests == 0U && result.missingRules == 0U && result.orphanRules == 0U && result.requiredUnavailable == 0U;
    *outAssessment = result; return UMI_STATUS_OK;
}
