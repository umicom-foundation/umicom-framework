/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_codeguard_documentation_inventory.c
 *
 * PURPOSE:
 *   Verify workspace documentation totals remain bounded, merge safely and
 *   report truthful function, decision and per-file percentages.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/codeguard/documentation_inventory.h"

/* Build a caller-owned report fixture with internally consistent counters. */
static UmiCodeGuardDocumentationReport report_fixture(
    bool passes,
    size_t functions,
    size_t documented_functions,
    size_t decisions,
    size_t explained_decisions)
{
    UmiCodeGuardDocumentationReport report;
    (void)memset(&report, 0, sizeof(report));
    report.structure_size = (uint32_t)sizeof(report);
    report.line_count = 20U;
    report.function_count = functions;
    report.documented_function_count = documented_functions;
    report.decision_count = decisions;
    report.explained_decision_count = explained_decisions;
    report.has_standard_file_header = true;
    report.passes = passes;
    return report;
}

/* Exercise record, merge, percentage and conservative pass behaviour. */
int main(void)
{
    UmiCodeGuardDocumentationInventory inventory;
    UmiCodeGuardDocumentationInventory second;
    UmiCodeGuardDocumentationReport passing = report_fixture(
        true, 4U, 4U, 2U, 2U);
    UmiCodeGuardDocumentationReport incomplete = report_fixture(
        false, 4U, 2U, 2U, 1U);
    umi_codeguard_documentation_inventory_init(&inventory);
    umi_codeguard_documentation_inventory_init(&second);
    /* One complete file produces full aggregate coverage. */
    assert(umi_codeguard_documentation_inventory_record(
        &inventory, &passing) == UMI_STATUS_OK);
    assert(umi_codeguard_documentation_inventory_passes(&inventory));
    /* A second incomplete file reduces each aggregate percentage truthfully. */
    assert(umi_codeguard_documentation_inventory_record(
        &second, &incomplete) == UMI_STATUS_OK);
    assert(umi_codeguard_documentation_inventory_merge(
        &inventory, &second) == UMI_STATUS_OK);
    assert(inventory.file_count == 2U);
    assert(umi_codeguard_documentation_inventory_file_percent(&inventory) == 50U);
    assert(umi_codeguard_documentation_inventory_function_percent(&inventory) == 75U);
    assert(umi_codeguard_documentation_inventory_decision_percent(&inventory) == 75U);
    assert(!umi_codeguard_documentation_inventory_passes(&inventory));
    /* Self-merge is rejected because it is normally an accidental double count. */
    assert(umi_codeguard_documentation_inventory_merge(
        &inventory, &inventory) == UMI_STATUS_INVALID_ARGUMENT);
    /* Corrupted subset totals are detected before an application renders them. */
    inventory.passing_file_count = inventory.file_count + 1U;
    assert(umi_codeguard_documentation_inventory_validate(
        &inventory) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
