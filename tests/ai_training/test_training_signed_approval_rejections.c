/*-----------------------------------------------------------------------------
 * Umicom Framework | Native AI regression
 * File: tests/ai_training/test_training_signed_approval_rejections.c
 * PURPOSE: Fail-closed, no-clobber, tamper and wrong-key regression tests.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation | MIT
 *---------------------------------------------------------------------------
 * LICENCE: MIT
 */
#include "training_signed_approval_fixture.h"
#include <stdio.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; \
} } while (0)

static int CheckRejectedWithoutOutput(
    UmiSignedApprovalFixture *fixture, const char *key,
    const char *const paths[1], size_t count)
{
    char json[UMI_AI_TRAINING_SIGNED_APPROVAL_JSON_CAPACITY];
    size_t written = 907U;
    memset(json, '#', sizeof(json));
    CHECK(UmiAiTrainingSignedReleaseApprovalVerifyJson(
        paths, count, fixture->policyPath, fixture->approvalPath,
        key, json, sizeof(json), &written) != UMI_STATUS_OK);
    CHECK(written == 907U);
    for (size_t i = 0U; i < sizeof(json); ++i) CHECK(json[i] == '#');
    return 0;
}

int main(void)
{
    UmiSignedApprovalFixture fixture;
    UmiAiTrainingReleaseApproval saved, after;
    char signer[UMI_SHA256_HEX_CAPACITY];
    char preservedSigner[UMI_SHA256_HEX_CAPACITY];
    const char *paths[1];
    char tiny[4] = {'S','A','F','E'};
    size_t untouched = 456U;

    CHECK(UmiTestPrepareSignedApproval("signed-rejections", &fixture));
    paths[0] = fixture.publication.base;
    /* Missing detached signature must NEVER imply unsigned approval. */
    CHECK(CheckRejectedWithoutOutput(&fixture, fixture.approvalPublicPem,
                                    paths, 1U) == 0);
    CHECK(UmiAiTrainingApprovalAttestationSignNewFile(
        fixture.approvalPath, fixture.approvalPrivatePem) == UMI_STATUS_OK);
    CHECK(CheckRejectedWithoutOutput(&fixture, fixture.publication.trustedPem,
                                    paths, 1U) == 0); /* wrong approval key */
    CHECK(CheckRejectedWithoutOutput(&fixture, fixture.approvalPublicPem,
                                    paths, 0U) == 0); /* wrong batch size */

    CHECK(UmiAiTrainingApprovalAttestationVerifyFile(
        fixture.approvalPath, fixture.approvalPublicPem, &saved,
        preservedSigner) == UMI_STATUS_OK);
    memcpy(signer, preservedSigner, sizeof(signer));
    after = saved;
    /* Parser-valid change to SHA-256 is also signature-invalid. */
    {
        char changed[256];
        const char *digest = strstr(fixture.approvalText, "batch_sha256=");
        size_t offset;
        CHECK(digest != NULL);
        offset = (size_t)(digest - fixture.approvalText) +
                 sizeof("batch_sha256=") - 1U;
        memcpy(changed, fixture.approvalText, fixture.approvalTextLength);
        changed[offset] = changed[offset] == '0' ? '1' : '0';
        CHECK(UmiTestWriteBytes(fixture.approvalPath, changed,
                                 fixture.approvalTextLength));
        CHECK(UmiAiTrainingApprovalAttestationVerifyFile(
            fixture.approvalPath, fixture.approvalPublicPem,
            &after, signer) != UMI_STATUS_OK);
        CHECK(memcmp(&saved, &after, sizeof(saved)) == 0);
        CHECK(memcmp(preservedSigner, signer, sizeof(signer)) == 0);
        CHECK(CheckRejectedWithoutOutput(&fixture,
             fixture.approvalPublicPem, paths, 1U) == 0);
        CHECK(UmiTestWriteBytes(fixture.approvalPath,
              fixture.approvalText, fixture.approvalTextLength));
    }
    /* A broken signature record must fail rather than selecting a key. */
    CHECK(UmiTestWriteBytes(fixture.signedPath, "invalid\n", 8U));
    CHECK(CheckRejectedWithoutOutput(&fixture, fixture.approvalPublicPem,
                                    paths, 1U) == 0);
    CHECK(UmiAiTrainingApprovalAttestationVerifyFile(
        fixture.approvalPath, fixture.approvalPublicPem,
        &after, signer) != UMI_STATUS_OK);
    CHECK(memcmp(&saved, &after, sizeof(saved)) == 0);
    CHECK(memcmp(preservedSigner, signer, sizeof(signer)) == 0);
    CHECK(remove(fixture.signedPath) == 0);
    CHECK(UmiAiTrainingApprovalAttestationSignNewFile(
        fixture.approvalPath, fixture.approvalPrivatePem) == UMI_STATUS_OK);

    /* Even a correctly signed approval is not a substitute for fresh
     * publisher verification or adequate caller output capacity. */
    CHECK(UmiAiTrainingSignedReleaseApprovalVerifyJson(
        paths, 1U, fixture.policyPath, fixture.approvalPath,
        fixture.approvalPublicPem, tiny, sizeof(tiny), &untouched) ==
        UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(untouched == 456U && memcmp(tiny, "SAFE", 4U) == 0);
    CHECK(UmiTestWriteBytes(fixture.publication.source,
                            "modified source\n", 16U));
    CHECK(CheckRejectedWithoutOutput(&fixture, fixture.approvalPublicPem,
                                    paths, 1U) == 0);
    UmiTestDeleteSignedApproval(&fixture);
    puts("signed approval: missing/wrong key, tamper, no-clobber PASS");
    return 0;
}
