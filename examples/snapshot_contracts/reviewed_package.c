/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/snapshot_contracts/reviewed_package.c
 * PURPOSE: Publish related package-description fields as one reviewed value.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/sdk_runtime/package_evidence.h"
#include <stdio.h>
#include <string.h>

int main(void)
{
    /* This is a state-edit lesson, not a package validator. Descriptive fields
     * are prepared here without claiming that an external package was tested. */
    UmiSdkRuntimePackageEvidence live;
    umi_sdk_runtime_package_evidence_init(&live, "notes-package");
    uint64_t reviewed_revision = live.revision;
    UmiSdkRuntimePackageEvidence proposal = live;
    if (umi_sdk_runtime_package_evidence_set_path(&proposal, "share/notes") != UMI_STATUS_OK) return 1;
    if (umi_sdk_runtime_package_evidence_set_detail(&proposal, "Awaiting package inspection") != UMI_STATUS_OK) return 1;
    if (umi_sdk_runtime_package_evidence_replace_if_current(
            &live, reviewed_revision, &proposal) != UMI_STATUS_OK) return 1;
    if (live.revision != reviewed_revision + 1U) return 1;
    if (strcmp(live.path, "share/notes") != 0 || strcmp(live.detail, "Awaiting package inspection") != 0) return 1;
    puts("Related description fields were published with one new revision.");

    /* A later owner change takes priority over this older prepared value.
     * The unchanged proposal can still be shown to explain the conflict. */
    reviewed_revision = live.revision;
    proposal = live;
    if (umi_sdk_runtime_package_evidence_set_detail(&proposal, "Earlier proposed description") != UMI_STATUS_OK) return 1;
    if (umi_sdk_runtime_package_evidence_set_detail(&live, "New owner description") != UMI_STATUS_OK) return 1;
    if (umi_sdk_runtime_package_evidence_replace_if_current(
            &live, reviewed_revision, &proposal) != UMI_STATUS_INVALID_STATE) return 1;
    if (strcmp(live.detail, "New owner description") != 0) return 1;
    puts("The stale description was refused; the newer owner value remains.");
    return 0;
}
