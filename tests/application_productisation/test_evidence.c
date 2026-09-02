/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation/test_evidence.c
 *
 * PURPOSE:
 *   Verify evidence validation, replacement, frontend aggregation and
 *   rejection of undeclared product assets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/application/productisation/evidence.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiProductisationEvidenceLedger ledger;
    const UmiProductisationEvidence *entry;
    umi_productisation_evidence_ledger_init(&ledger);
    assert(umi_productisation_evidence_record(
        &ledger, "org.umicom.studio", UMI_PRODUCTISATION_ASSET_PANEL,
        "editor", "gtk", UMI_PRODUCTISATION_EVIDENCE_TESTED,
        "studio.editor.gtk") == UMI_STATUS_OK);
    assert(umi_productisation_evidence_record(
        &ledger, "org.umicom.studio", UMI_PRODUCTISATION_ASSET_PANEL,
        "editor", "gtk", UMI_PRODUCTISATION_EVIDENCE_ACCEPTED,
        "studio.editor.acceptance") == UMI_STATUS_OK);
    assert(ledger.count == 1U);
    entry = umi_productisation_evidence_find(
        &ledger, "org.umicom.studio", UMI_PRODUCTISATION_ASSET_PANEL,
        "editor", "gtk");
    assert(entry != NULL &&
           entry->state == UMI_PRODUCTISATION_EVIDENCE_ACCEPTED);
    assert(umi_productisation_evidence_effective_state(
        &ledger, "org.umicom.studio", UMI_PRODUCTISATION_ASSET_PANEL,
        "editor", UMI_PRODUCTISATION_EVIDENCE_DECLARED) ==
        UMI_PRODUCTISATION_EVIDENCE_ACCEPTED);
    assert(umi_productisation_evidence_record(
        &ledger, "org.umicom.studio", UMI_PRODUCTISATION_ASSET_PANEL,
        "not-a-panel", "gtk", UMI_PRODUCTISATION_EVIDENCE_TESTED,
        "invalid") == UMI_STATUS_NOT_FOUND);
    {
        char oversized[UMI_PRODUCTISATION_REFERENCE_CAPACITY + 1U];
        size_t index;
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index + 1U < sizeof(oversized); ++index)
            oversized[index] = 'x';
        oversized[sizeof(oversized) - 1U] = '\0';
        assert(umi_productisation_evidence_record(
            &ledger, "org.umicom.studio",
            UMI_PRODUCTISATION_ASSET_PANEL, "problems", "gtk",
            UMI_PRODUCTISATION_EVIDENCE_TESTED, oversized) ==
            UMI_STATUS_CAPACITY_EXCEEDED);
        assert(ledger.count == 1U);
    }
    return 0;
}
