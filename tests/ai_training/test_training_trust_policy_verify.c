/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_trust_policy_verify.c
 * PURPOSE: Prove key rollover, operator revocation, validity windows, file
 *          integrity and failure-atomic publication under reviewed policy.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "training_attestation_fixture.h"
#include "umicom/ai/training/training_trust_policy.h"
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#if defined(_WIN32)
#include <direct.h>
#define UMI_TRUST_TEST_MKDIR(path) _mkdir(path)
#define UMI_TRUST_TEST_RMDIR(path) _rmdir(path)
#else
#include <sys/stat.h>
#include <unistd.h>
#define UMI_TRUST_TEST_MKDIR(path) mkdir(path, 0700)
#define UMI_TRUST_TEST_RMDIR(path) rmdir(path)
#endif

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    return 1; } } while (0)

static int GetFingerprint(const char *keyFile, char hex[65])
{
    BIO *file = BIO_new_file(keyFile, "rb");
    EVP_PKEY *publicKey;
    unsigned char raw[32];
    size_t size = sizeof(raw);
    UmiStatus status;
    if (file == NULL) return 0;
    publicKey = PEM_read_bio_PUBKEY(file, NULL, NULL, NULL);
    BIO_free(file);
    if (publicKey == NULL) return 0;
    if (EVP_PKEY_get_raw_public_key(publicKey, raw, &size) != 1 ||
        size != sizeof(raw)) { EVP_PKEY_free(publicKey); return 0; }
    EVP_PKEY_free(publicKey);
    status = UmiSha256Buffer(raw, sizeof(raw), hex);
    return status == UMI_STATUS_OK;
}

static int SetPolicy(const char *path, const char *fingerprint,
                     const char *keyFilename, uint64_t from, uint64_t until,
                     const char *state)
{
    char text[512];
    int n = snprintf(text, sizeof(text),
        "UMICOM-AI-TRUST-1\nkey=%s,%s,%llu,%llu,%s\n",
        fingerprint, keyFilename, (unsigned long long)from,
        (unsigned long long)until, state);
    if (n <= 0 || (size_t)n >= sizeof(text)) return 0;
    return UmiTestWriteBytes(path, text, (size_t)n);
}

static int DenyWithoutMutation(const char *checkpoint, const char *policyFile,
                               uint64_t now)
{
    UmiAiTrainingTrustDecision result, preserved;
    memset(&result, 0x7b, sizeof(result));
    preserved = result;
    CHECK(UmiAiTrainingAttestationVerifyWithTrustPolicy(
          checkpoint, policyFile, now, &result) != UMI_STATUS_OK);
    CHECK(memcmp(&result, &preserved, sizeof(result)) == 0);
    return 0;
}

int main(void)
{
    UmiAttestationFixture old, replacement;
    UmiAiTrainingTrustDecision approved;
    char oldKeyId[65], newKeyId[65], oldPolicy[260], newPolicy[260];
    char bundle[1500];
    int n;
    CHECK(UmiTestMakeFixture("trust-old", &old));
    CHECK(UmiTestMakeFixture("trust-new", &replacement));
    CHECK(GetFingerprint(old.trustedPem, oldKeyId));
    CHECK(GetFingerprint(replacement.trustedPem, newKeyId));
    /* The public onboarding API must reuse the exact raw-key digest chosen
     * by the existing canonical signer and verifier. */
    {
        char observed[UMI_SHA256_HEX_CAPACITY] = {0};
        CHECK(UmiAiTrainingAttestationPublicKeyFingerprint(
              old.trustedPem, observed) == UMI_STATUS_OK);
        CHECK(strcmp(observed, oldKeyId) == 0);
        memcpy(observed, "KEEP", 5U);
        CHECK(UmiAiTrainingAttestationPublicKeyFingerprint(
              "absent-ed25519-key.pem", observed) != UMI_STATUS_OK);
        CHECK(strcmp(observed, "KEEP") == 0);
    }
    CHECK(strcmp(oldKeyId, newKeyId) != 0);
    n = snprintf(oldPolicy, sizeof(oldPolicy), "%s.trust", old.base);
    CHECK(n > 0 && (size_t)n < sizeof(oldPolicy));
    n = snprintf(newPolicy, sizeof(newPolicy), "%s.trust", replacement.base);
    CHECK(n > 0 && (size_t)n < sizeof(newPolicy));
    CHECK(UmiAiTrainingPublicationSaveNewFile(old.base) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAttestationSignNewFile(
          old.base, old.privatePem) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingPublicationSaveNewFile(replacement.base) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAttestationSignNewFile(
          replacement.base, replacement.privatePem) == UMI_STATUS_OK);
    /* A policy file is operator-controlled: the signature cannot provide it
     * or register its signing key as a trusted authority. */
    CHECK(SetPolicy(oldPolicy, oldKeyId, old.trustedPem,
                    100U, 200U, "active"));
    CHECK(UmiAiTrainingAttestationVerifyWithTrustPolicy(
          old.base, oldPolicy, 100U, &approved) == UMI_STATUS_OK);
    CHECK(approved.evaluatedAtUnixSeconds == 100U);
    CHECK(strcmp(approved.verifiedPublicKeySha256, oldKeyId) == 0);
    CHECK(approved.publication.checkpoint.byteCount == 17U);
    CHECK(UmiAiTrainingAttestationVerifyWithTrustPolicy(
          old.base, oldPolicy, 199U, &approved) == UMI_STATUS_OK);
    CHECK(DenyWithoutMutation(old.base, oldPolicy, 99U) == 0);
    CHECK(DenyWithoutMutation(old.base, oldPolicy, 200U) == 0);
    CHECK(SetPolicy(oldPolicy, oldKeyId, old.trustedPem,
                    100U, 200U, "revoked"));
    CHECK(DenyWithoutMutation(old.base, oldPolicy, 150U) == 0);
    CHECK(DenyWithoutMutation(old.base, oldPolicy, 101U) == 0);
    /* Key replacement cannot succeed by changing a policy fingerprint but
     * retaining another publisher's PEM: crypto verifies key and signature. */
    CHECK(SetPolicy(oldPolicy, newKeyId, replacement.trustedPem,
                    0U, 300U, "active"));
    CHECK(DenyWithoutMutation(old.base, oldPolicy, 150U) == 0);
    CHECK(SetPolicy(oldPolicy, oldKeyId, replacement.trustedPem,
                    0U, 300U, "active"));
    CHECK(DenyWithoutMutation(old.base, oldPolicy, 150U) == 0);
    /* A new publisher may be authorised while the previous signer remains
     * explicitly revoked. An earlier signature does not become valid again. */
    n = snprintf(bundle, sizeof(bundle),
        "UMICOM-AI-TRUST-1\n"
        "key=%s,%s,0,300,revoked\n"
        "key=%s,%s,100,300,active\n",
        oldKeyId, old.trustedPem, newKeyId, replacement.trustedPem);
    CHECK(n > 0 && (size_t)n < sizeof(bundle));
    CHECK(UmiTestWriteBytes(newPolicy, bundle, (size_t)n));
    CHECK(DenyWithoutMutation(old.base, newPolicy, 150U) == 0);
    CHECK(UmiAiTrainingAttestationVerifyWithTrustPolicy(
          replacement.base, newPolicy, 150U, &approved) == UMI_STATUS_OK);
    CHECK(strcmp(approved.verifiedPublicKeySha256, newKeyId) == 0);
    /* Resolve the PEM relative to the policy's own directory. A verified
     * checkpoint in the current working directory must not force the key
     * lookup to use that directory instead. */
    {
        char directory[224], policyInside[290], keyInside[290];
        unsigned char pemBytes[4096];
        size_t count;
        FILE *input;
        n = snprintf(directory, sizeof(directory), "%s-trust-directory", replacement.base);
        CHECK(n > 0 && (size_t)n < sizeof(directory));
        CHECK(UMI_TRUST_TEST_MKDIR(directory) == 0);
        n = snprintf(keyInside, sizeof(keyInside), "%s/approved.pem", directory);
        CHECK(n > 0 && (size_t)n < sizeof(keyInside));
        n = snprintf(policyInside, sizeof(policyInside), "%s/publishers.trust", directory);
        CHECK(n > 0 && (size_t)n < sizeof(policyInside));
        input = fopen(replacement.trustedPem, "rb");
        CHECK(input != NULL);
        count = fread(pemBytes, 1U, sizeof(pemBytes), input);
        CHECK(count > 0U && count < sizeof(pemBytes));
        CHECK(fclose(input) == 0);
        CHECK(UmiTestWriteBytes(keyInside, pemBytes, count));
        CHECK(SetPolicy(policyInside, newKeyId, "approved.pem",
                        100U, 300U, "active"));
        CHECK(UmiAiTrainingAttestationVerifyWithTrustPolicy(
              replacement.base, policyInside, 150U, &approved) == UMI_STATUS_OK);
        CHECK(remove(keyInside) == 0);
        CHECK(remove(policyInside) == 0);
        CHECK(UMI_TRUST_TEST_RMDIR(directory) == 0);
    }
    /* Even a valid signature cannot mask changed checkpoint companions. */
    CHECK(UmiTestWriteBytes(replacement.source, "different source lock\n", 22U));
    CHECK(DenyWithoutMutation(replacement.base, newPolicy, 150U) == 0);
    (void)remove(oldPolicy);
    (void)remove(newPolicy);
    UmiTestDeleteFixture(&old);
    UmiTestDeleteFixture(&replacement);
    puts("Offline publisher-key policy: rotation, revocation and crypto verification PASS");
    return 0;
}
