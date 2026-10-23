/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/training_signed_approval_fixture.h
 * PURPOSE: Disposable, independently keyed signed-release-approval fixtures.
 * SECURITY: Test-only ephemeral Ed25519 private keys; never commit PEM data.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_SIGNED_APPROVAL_FIXTURE_H
#define UMICOM_TEST_SIGNED_APPROVAL_FIXTURE_H

#include "training_attestation_fixture.h"
#include "umicom/ai/training/training_approval_attestation.h"
#include <openssl/evp.h>
#include <stdio.h>
#include <string.h>

typedef struct UmiSignedApprovalFixture {
    UmiAttestationFixture publication;
    char policyPath[256];
    char approvalPath[256];
    char signedPath[288];
    char stagingPath[300];
    char approvalPrivatePem[256];
    char approvalPublicPem[256];
    char approvalText[256];
    size_t approvalTextLength;
    char approvalKeySha256[UMI_SHA256_HEX_CAPACITY];
} UmiSignedApprovalFixture;

static int UmiTestPrepareSignedApproval(const char *name,
                                        UmiSignedApprovalFixture *fixture)
{
    EVP_PKEY *approver = NULL;
    char publisherKey[UMI_SHA256_HEX_CAPACITY];
    char policy[1024];
    char batch[UMI_AI_TRAINING_TRUST_BATCH_JSON_CAPACITY];
    const char *paths[1];
    const char *hash;
    size_t size = 0U;
    int n;
    if (name == NULL || fixture == NULL) return 0;
    memset(fixture, 0, sizeof(*fixture));
    if (!UmiTestMakeFixture(name, &fixture->publication)) return 0;
#define SIGNED_PATH(field, suffix) do { \
    n = snprintf(fixture->field, sizeof(fixture->field), \
                 "%s%s", fixture->publication.base, suffix); \
    if (n <= 0 || (size_t)n >= sizeof(fixture->field)) return 0; \
} while (0)
    SIGNED_PATH(policyPath, ".trust");
    SIGNED_PATH(approvalPath, ".release-approval");
    SIGNED_PATH(approvalPrivatePem, ".approver-private.pem");
    SIGNED_PATH(approvalPublicPem, ".approver-public.pem");
    n = snprintf(fixture->signedPath, sizeof(fixture->signedPath), "%s%s",
                 fixture->approvalPath, UMI_AI_TRAINING_APPROVAL_SIGNATURE_SUFFIX);
    if (n <= 0 || (size_t)n >= sizeof(fixture->signedPath)) return 0;
    n = snprintf(fixture->stagingPath, sizeof(fixture->stagingPath),
                 "%s.partial", fixture->signedPath);
    if (n <= 0 || (size_t)n >= sizeof(fixture->stagingPath)) return 0;
#undef SIGNED_PATH
    if (UmiAiTrainingPublicationSaveNewFile(fixture->publication.base) != UMI_STATUS_OK ||
        UmiAiTrainingAttestationSignNewFile(
            fixture->publication.base, fixture->publication.privatePem) != UMI_STATUS_OK ||
        UmiAiTrainingAttestationPublicKeyFingerprint(
            fixture->publication.trustedPem, publisherKey) != UMI_STATUS_OK)
        return 0;
    n = snprintf(policy, sizeof(policy),
                 "UMICOM-AI-TRUST-1\nkey=%s,%s,100,200,active\n",
                 publisherKey, fixture->publication.trustedPem);
    if (n <= 0 || (size_t)n >= sizeof(policy) ||
        !UmiTestWriteBytes(fixture->policyPath, policy, (size_t)n))
        return 0;
    paths[0] = fixture->publication.base;
    if (UmiAiTrainingTrustBatchVerifyJson(
            paths, 1U, fixture->policyPath, UINT64_C(150),
            batch, sizeof(batch), &size) != UMI_STATUS_OK)
        return 0;
    hash = strstr(batch, "\"batch_sha256\":\"");
    if (hash == NULL) return 0;
    hash += sizeof("\"batch_sha256\":\"") - 1U;
    if (strlen(hash) < 65U || hash[64] != '"') return 0;
    n = snprintf(fixture->approvalText, sizeof(fixture->approvalText),
                 "UMICOM-AI-RELEASE-APPROVAL-1\n"
                 "publication_count=1\nevaluated_at_unix_seconds=150\n"
                 "batch_sha256=%.*s\n", 64, hash);
    if (n <= 0 || (size_t)n >= sizeof(fixture->approvalText) ||
        !UmiTestWriteBytes(fixture->approvalPath,
                           fixture->approvalText, (size_t)n))
        return 0;
    fixture->approvalTextLength = (size_t)n;
    if (!UmiTestNewKey(&approver)) return 0;
    if (!UmiTestWriteKey(fixture->approvalPrivatePem, approver, 1) ||
        !UmiTestWriteKey(fixture->approvalPublicPem, approver, 0)) {
        EVP_PKEY_free(approver);
        return 0;
    }
    EVP_PKEY_free(approver);
    if (UmiAiTrainingAttestationPublicKeyFingerprint(
            fixture->approvalPublicPem,
            fixture->approvalKeySha256) != UMI_STATUS_OK)
        return 0;
    /* Fail if test keys are accidentally identical. */
    return strcmp(publisherKey, fixture->approvalKeySha256) != 0;
}

static void UmiTestDeleteSignedApproval(const UmiSignedApprovalFixture *fixture)
{
    if (fixture == NULL) return;
    (void)remove(fixture->policyPath);
    (void)remove(fixture->approvalPath);
    (void)remove(fixture->signedPath);
    (void)remove(fixture->stagingPath);
    (void)remove(fixture->approvalPrivatePem);
    (void)remove(fixture->approvalPublicPem);
    UmiTestDeleteFixture(&fixture->publication);
}

#endif /* UMICOM_TEST_SIGNED_APPROVAL_FIXTURE_H */
