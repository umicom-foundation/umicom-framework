/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/training_attestation_fixture.h
 * PURPOSE: Create disposable four-file test evidence and ephemeral Ed25519
 *          keys, exclusively for native attestation regressions.
 * SECURITY: No test private key is committed; every key is generated in memory
 *           and its temporary file is removed after the test.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TEST_TRAINING_ATTESTATION_FIXTURE_H
#define UMICOM_AI_TEST_TRAINING_ATTESTATION_FIXTURE_H

#include "umicom/ai/training/training_attestation.h"
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <stdio.h>
#include <string.h>
#if defined(_WIN32)
#include <process.h>
#define UMI_TEST_ATTESTATION_PID _getpid
#else
#include <unistd.h>
#define UMI_TEST_ATTESTATION_PID getpid
#endif

typedef struct UmiAttestationFixture {
    char base[192];
    char clip[224];
    char run[224];
    char source[224];
    char publication[224];
    char signature[256];
    char staging[256];
    char privatePem[224];
    char trustedPem[224];
    char wrongPem[224];
} UmiAttestationFixture;

static int UmiTestWriteBytes(const char *path, const void *bytes, size_t length)
{
    FILE *output = fopen(path, "wb");
    int result;
    if (output == NULL) return 0;
    result = fwrite(bytes, 1U, length, output) == length;
    if (fclose(output) != 0) result = 0;
    return result;
}

static int UmiTestWriteKey(const char *path, const EVP_PKEY *key, int privateKey)
{
    BIO *output = BIO_new_file(path, "wb");
    int okay;
    if (output == NULL) return 0;
    if (privateKey != 0)
        okay = PEM_write_bio_PrivateKey(output, (EVP_PKEY *)key,
                                       NULL, NULL, 0, NULL, NULL);
    else
        okay = PEM_write_bio_PUBKEY(output, (EVP_PKEY *)key);
    if (BIO_free(output) != 1) okay = 0;
    return okay == 1;
}

static int UmiTestNewKey(EVP_PKEY **outKey)
{
    EVP_PKEY_CTX *context = EVP_PKEY_CTX_new_id(EVP_PKEY_ED25519, NULL);
    EVP_PKEY *key = NULL;
    int ok;
    if (context == NULL) return 0;
    ok = EVP_PKEY_keygen_init(context) == 1 &&
         EVP_PKEY_keygen(context, &key) == 1;
    EVP_PKEY_CTX_free(context);
    if (!ok) { EVP_PKEY_free(key); return 0; }
    *outKey = key;
    return 1;
}

static int UmiTestMakeFixture(const char *name, UmiAttestationFixture *fixture)
{
    EVP_PKEY *primary = NULL, *different = NULL;
    char prefix[150];
    int n;
    if (fixture == NULL || name == NULL) return 0;
    memset(fixture, 0, sizeof(*fixture));
    n = snprintf(prefix, sizeof(prefix), "umi-attestation-%s-%ld",
                 name, (long)UMI_TEST_ATTESTATION_PID());
    if (n < 0 || (size_t)n >= sizeof(prefix)) return 0;
#define UMI_FIXTURE_PATH(member, suffix) do { \
    n = snprintf(fixture->member, sizeof(fixture->member), "%s%s", prefix, suffix); \
    if (n < 0 || (size_t)n >= sizeof(fixture->member)) return 0; \
} while (0)
    UMI_FIXTURE_PATH(base, ".umi");
    n = snprintf(fixture->clip, sizeof(fixture->clip), "%s.clip-policy", fixture->base);
    if (n < 0 || (size_t)n >= sizeof(fixture->clip)) return 0;
    n = snprintf(fixture->run, sizeof(fixture->run), "%s.training-lock", fixture->base);
    if (n < 0 || (size_t)n >= sizeof(fixture->run)) return 0;
    n = snprintf(fixture->source, sizeof(fixture->source), "%s.source-lock", fixture->base);
    if (n < 0 || (size_t)n >= sizeof(fixture->source)) return 0;
    n = snprintf(fixture->publication, sizeof(fixture->publication), "%s.publication", fixture->base);
    if (n < 0 || (size_t)n >= sizeof(fixture->publication)) return 0;
    n = snprintf(fixture->signature, sizeof(fixture->signature), "%s%s",
                 fixture->base, UMI_AI_TRAINING_ATTESTATION_SUFFIX);
    if (n < 0 || (size_t)n >= sizeof(fixture->signature)) return 0;
    n = snprintf(fixture->staging, sizeof(fixture->staging), "%s.partial", fixture->signature);
    if (n < 0 || (size_t)n >= sizeof(fixture->staging)) return 0;
    UMI_FIXTURE_PATH(privatePem, ".private-test.pem");
    UMI_FIXTURE_PATH(trustedPem, ".public-test.pem");
    UMI_FIXTURE_PATH(wrongPem, ".wrong-public-test.pem");
#undef UMI_FIXTURE_PATH
    if (!UmiTestWriteBytes(fixture->base, "checkpoint bytes\n", 17U) ||
        !UmiTestWriteBytes(fixture->clip, "clip settings\n", 14U) ||
        !UmiTestWriteBytes(fixture->run, "training settings\n", 18U) ||
        !UmiTestWriteBytes(fixture->source, "dataset digest\n", 15U)) return 0;
    if (!UmiTestNewKey(&primary) || !UmiTestNewKey(&different)) goto failed;
    if (!UmiTestWriteKey(fixture->privatePem, primary, 1) ||
        !UmiTestWriteKey(fixture->trustedPem, primary, 0) ||
        !UmiTestWriteKey(fixture->wrongPem, different, 0)) goto failed;
    EVP_PKEY_free(primary);
    EVP_PKEY_free(different);
    return 1;
failed:
    EVP_PKEY_free(primary);
    EVP_PKEY_free(different);
    return 0;
}

static void UmiTestDeleteFixture(const UmiAttestationFixture *fixture)
{
    if (fixture == NULL) return;
    (void)remove(fixture->base);
    (void)remove(fixture->clip);
    (void)remove(fixture->run);
    (void)remove(fixture->source);
    (void)remove(fixture->publication);
    (void)remove(fixture->signature);
    (void)remove(fixture->staging);
    (void)remove(fixture->privatePem);
    (void)remove(fixture->trustedPem);
    (void)remove(fixture->wrongPem);
}

#endif /* UMICOM_AI_TEST_TRAINING_ATTESTATION_FIXTURE_H */
