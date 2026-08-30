/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_release_evidence.c
 *
 * PURPOSE:
 *   Verify supply-chain evidence required by stable product releases.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/delivery/delivery.h"

int main(void)
{
    UmiReleaseEvidence evidence;
    umi_release_evidence_init(&evidence);
    assert(umi_release_evidence_set(
               &evidence, "tests", UMI_EVIDENCE_PASS) == UMI_STATUS_OK);
    assert(umi_release_evidence_set(
               &evidence, "checksums", UMI_EVIDENCE_PASS) == UMI_STATUS_OK);
    assert(umi_release_evidence_set(
               &evidence, "licences", UMI_EVIDENCE_PASS) == UMI_STATUS_OK);
    assert(umi_release_evidence_set(
               &evidence, "sbom", UMI_EVIDENCE_PASS) == UMI_STATUS_OK);
    assert(umi_release_evidence_set(
               &evidence, "provenance", UMI_EVIDENCE_PASS) == UMI_STATUS_OK);
    assert(!umi_release_evidence_ready(&evidence, UMI_RELEASE_STABLE));
    assert(umi_release_evidence_set(
               &evidence, "signatures", UMI_EVIDENCE_PASS) == UMI_STATUS_OK);
    assert(umi_release_evidence_ready(&evidence, UMI_RELEASE_STABLE));
    return 0;
}
