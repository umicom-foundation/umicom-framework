/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/release_inventory_consumer/policy_consumer.c
 * PURPOSE:
 *   Use the installed policy service without source-tree dependencies.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: examples/release_inventory_consumer/policy_consumer.c
 * Purpose: Use the installed policy service without source-tree dependencies.
 *---------------------------------------------------------------------------*/
#include <umicom/distribution/runtime/test_policy.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    const char configuredText[] = "UMICOM-RELEASE-INVENTORY\t1\ncontext\tcmake"
        "\t0123456789abcdef0123456789abcdef\t2f737263\t2f6275696c64\t4465627567\n"
        "test\t6c6573736f6e\t\tregistered\t\n";
    const char observedText[] = "UMICOM-RELEASE-INVENTORY\t1\ncontext\tctest"
        "\t0123456789abcdef0123456789abcdef\t2f737263\t2f6275696c64\t4465627567\n"
        "test\t6c6573736f6e\t\tregistered\t\n";
    UmiReleaseInventory *configured = NULL, *observed = NULL;
    UmiReleaseTestPolicy *draft = NULL, *reviewed = NULL;
    char *draftText = NULL, *reviewedText = NULL; size_t draftSize = 0U, reviewedSize = 0U;
    UmiReleaseTestPolicyAssessment assessment = {0};
    int result = 1;
    if (UmiReleaseInventoryParse(configuredText, strlen(configuredText), &configured) != UMI_STATUS_OK) goto finish;
    if (UmiReleaseInventoryParse(observedText, strlen(observedText), &observed) != UMI_STATUS_OK) goto finish;
    if (UmiReleaseTestPolicyDraft(configured, &draftText, &draftSize) != UMI_STATUS_OK) goto finish;
    if (UmiReleaseTestPolicyParse(draftText, draftSize, &draft) != UMI_STATUS_OK) goto finish;
    if (UmiReleaseTestPolicyAssess(configured, observed, draft, NULL, NULL, &assessment) != UMI_STATUS_OK ||
        assessment.readyForExecution || assessment.unassignedTests != 1U) goto finish;
    /* This is a decision about a synthetic lesson, never a product approval. */
    if (UmiReleaseTestPolicyEdit(draft, "lesson", UMI_RELEASE_TEST_REQUIRED, "Example reviewer",
        "The lesson must exercise the installed public API", &reviewedText, &reviewedSize) != UMI_STATUS_OK) goto finish;
    if (UmiReleaseTestPolicyParse(reviewedText, reviewedSize, &reviewed) != UMI_STATUS_OK) goto finish;
    if (UmiReleaseTestPolicyAssess(configured, observed, reviewed, NULL, NULL, &assessment) != UMI_STATUS_OK ||
        !assessment.readyForExecution) goto finish;
    (void)puts("Synthetic lesson policy is ready for execution. No product release is approved.");
    result = 0;
finish:
    UmiReleaseTestPolicyDestroy(reviewed); UmiReleaseTestPolicyDestroy(draft);
    UmiReleaseTestPolicyTextDestroy(reviewedText); UmiReleaseTestPolicyTextDestroy(draftText);
    UmiReleaseInventoryDestroy(observed); UmiReleaseInventoryDestroy(configured);
    return result;
}
