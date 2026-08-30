/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_recovery_bridge.c
 *
 * PURPOSE:
 *   Verify explicit recovery recommendations, recovered-document marking and
 *   saved/current choices.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"


int main(void)
{
    UmiWorkbenchLayoutDocument saved;
    UmiWorkbenchLayoutDocument current;
    UmiWorkbenchLayoutDocument recovered;
    UmiWorkbenchLayoutDocument result;
    UmiWorkbenchDesignerRecoveryProposal proposal;
    TEST_REQUIRE_STATUS(test_make_document(&saved));
    TEST_REQUIRE_STATUS(umi_workbench_layout_document_copy(&current, &saved));
    TEST_REQUIRE_STATUS(umi_workbench_layout_document_copy(&recovered, &saved));
    TEST_REQUIRE_STATUS(umi_workbench_layout_document_set_metadata(
        &recovered, "Recovered Layout", "development", "Recovered autosave"));
    recovered.version.revision = current.version.revision + 3U;
    TEST_REQUIRE_STATUS(umi_workbench_designer_recovery_proposal_build(
        &proposal, &saved, &current, &recovered));
    TEST_REQUIRE(proposal.recovery_available);
    TEST_REQUIRE(proposal.recommended_choice ==
                 UMI_WORKBENCH_DESIGNER_RECOVERY_USE_RECOVERED);
    TEST_REQUIRE_STATUS(umi_workbench_designer_recovery_apply(
        &proposal, proposal.recommended_choice, &saved, &current,
        &recovered, &result));
    TEST_REQUIRE((result.flags & UMI_WORKBENCH_LAYOUT_DOCUMENT_RECOVERED) != 0U);
    return 0;
}
