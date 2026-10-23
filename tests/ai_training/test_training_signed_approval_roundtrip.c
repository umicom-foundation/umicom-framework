/*-----------------------------------------------------------------------------
 * Umicom Framework | Native AI regression
 * File: tests/ai_training/test_training_signed_approval_roundtrip.c
 * PURPOSE: Real independent Ed25519 approval verification and strict release.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "training_signed_approval_fixture.h"
#include <stdio.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; \
} } while (0)

int main(void)
{
    UmiSignedApprovalFixture fixture;
    UmiAiTrainingReleaseApproval approved;
    char signer[UMI_SHA256_HEX_CAPACITY];
    char output[UMI_AI_TRAINING_SIGNED_APPROVAL_JSON_CAPACITY];
    char repeated[UMI_AI_TRAINING_SIGNED_APPROVAL_JSON_CAPACITY];
    const char *paths[1];
    size_t written = 0U, repeatedWritten = 0U;
    CHECK(UmiTestPrepareSignedApproval("signed-roundtrip", &fixture));
    paths[0] = fixture.publication.base;
    CHECK(UmiAiTrainingApprovalAttestationSignNewFile(
        fixture.approvalPath, fixture.approvalPrivatePem) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingApprovalAttestationSignNewFile(
        fixture.approvalPath, fixture.approvalPrivatePem) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(UmiAiTrainingApprovalAttestationVerifyFile(
        fixture.approvalPath, fixture.approvalPublicPem, &approved,
        signer) == UMI_STATUS_OK);
    CHECK(strcmp(signer, fixture.approvalKeySha256) == 0);
    CHECK(approved.publicationCount == 1U);
    CHECK(approved.evaluatedAtUnixSeconds == 150U);
    CHECK(UmiAiTrainingSignedReleaseApprovalVerifyJson(
        paths, 1U, fixture.policyPath, fixture.approvalPath,
        fixture.approvalPublicPem, output, sizeof(output), &written) == UMI_STATUS_OK);
    CHECK(written == strlen(output));
    CHECK(strstr(output, "\"approval_record_signed\":true") != NULL);
    CHECK(strstr(output, "\"exact_ordered_batch_pinned\":true") != NULL);
    CHECK(strstr(output, "\"approval_record_signed\":true},\"approval_signature\":{") != NULL);
    CHECK(strstr(output, fixture.approvalKeySha256) != NULL);
    CHECK(strstr(output, fixture.publication.base) == NULL);
    CHECK(output[0] == '{' && output[written - 1U] == '}');
    CHECK(UmiAiTrainingSignedReleaseApprovalVerifyJson(
        paths, 1U, fixture.policyPath, fixture.approvalPath,
        fixture.approvalPublicPem, repeated, sizeof(repeated),
        &repeatedWritten) == UMI_STATUS_OK);
    CHECK(repeatedWritten == written && memcmp(output, repeated, written + 1U) == 0);
    UmiTestDeleteSignedApproval(&fixture);
    puts("independently signed release approval roundtrip: PASS");
    return 0;
}
