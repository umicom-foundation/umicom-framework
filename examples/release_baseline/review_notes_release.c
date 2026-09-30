/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/release_baseline/review_notes_release.c
 * PURPOSE:
 *   Show why a Linux test receipt cannot complete a Windows Notes release.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: examples/release_baseline/review_notes_release.c
 * Purpose: Show why a Linux test receipt cannot complete a Windows Notes release.
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/evidence.h"
#include <stdio.h>
#include <string.h>

int main(void)
{
    /* These fictional receipts explain policy. They are not real test evidence
     * for Notes or any Framework product and must never enter the release book. */
    const char contractText[] =
        "UMICOM-RELEASE-CONTRACT\t1\ncandidate\tnotes-practice\n"
        "source\t1111111111111111111111111111111111111111\n"
        "require\tsign\tsignature\tpackage\tRelease\treview\tFW-18\tInspect publisher identity\n"
        "require\tbytes\tchecksum\tpackage\tRelease\tanalysis\tFW-18\tCheck the installed package bytes\n"
        "require\tabi\tcompatibility\tlinux-x86_64\tRelease\tnative\tFW-03\tBuild an independent Notes client\n"
        "require\tunit\ttests\tlinux-x86_64\tRelease\tnative\tFW-05\tRun Notes worker tests\n"
        "require\twindow\tfrontend\twindows11-x86_64\tRelease\tinstalled\tFW-10\tUse both Notes windows after installation\n";
    const char evidenceText[] =
        "UMICOM-RELEASE-EVIDENCE\t1\n"
        "result\tunit\tnotes-practice\t1111111111111111111111111111111111111111\tlinux-x86_64\tRelease\tnative\tpassed\t8\t8\t0\t0\t0\tasserted\t"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\tpractice/worker.txt\n";
    UmiReleaseContract *contract = NULL;
    UmiReleaseEvidence *evidence = NULL;
    UmiReleaseEvidenceAssessment assessment;
    UmiStatus status = UmiReleaseContractParse(contractText,strlen(contractText),&contract);
    if (status == UMI_STATUS_OK) status = UmiReleaseEvidenceParse(evidenceText,strlen(evidenceText),&evidence);
    if (status == UMI_STATUS_OK) status = UmiReleaseEvidenceAssess(contract,evidence,&assessment);
    bool correct = status == UMI_STATUS_OK && assessment.reportedComplete == 1U && assessment.missing == 4U && !assessment.readyForOwnerReview;
    if (correct) {
        (void)puts("One Linux test receipt is present; four required release records are missing.");
        (void)puts("The Windows Notes journey is not a Linux test result. Release remains blocked.");
        (void)puts("Practice complete. Memory only; no test, window, artefact or repository was opened.");
    }
    UmiReleaseEvidenceDestroy(evidence); UmiReleaseContractDestroy(contract);
    return correct && ferror(stdout) == 0 ? 0 : 1;
}
