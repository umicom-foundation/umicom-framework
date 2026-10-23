/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_attestation_rejections.c
 * PURPOSE: Reject signature tampering, changed checkpoint/record bytes, missing
 *          trust anchors and occupied staging, preserving caller outputs.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "training_attestation_fixture.h"

#define CHECK(test) do { if (!(test)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #test); \
    return 1; } } while (0)

int main(void)
{
    UmiAttestationFixture fixture;
    UmiAiTrainingPublicationEvidence evidence, saved;
    unsigned char signatureRecord[UMI_AI_TRAINING_ATTESTATION_RECORD_BYTES];
    unsigned char changed[UMI_AI_TRAINING_ATTESTATION_RECORD_BYTES + 1U];
    FILE *file;
    CHECK(UmiTestMakeFixture("rejections", &fixture));
    memset(&evidence, 0x71, sizeof(evidence));
    saved = evidence;
    CHECK(UmiAiTrainingAttestationSignNewFile(NULL, fixture.privatePem) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingAttestationVerifyAndInspectFile(NULL,
          fixture.trustedPem, &evidence) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingAttestationVerifyAndInspectFile(fixture.base,
          fixture.trustedPem, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingPublicationSaveNewFile(fixture.base) == UMI_STATUS_OK);
    CHECK(UmiTestWriteBytes(fixture.staging, "UNFINISHED", 10U));
    CHECK(UmiAiTrainingAttestationSignNewFile(fixture.base,
          fixture.privatePem) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(remove(fixture.staging) == 0);
    CHECK(UmiAiTrainingAttestationSignNewFile(fixture.base,
          fixture.privatePem) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAttestationVerifyAndInspectFile(fixture.base,
          fixture.wrongPem, &evidence) != UMI_STATUS_OK);
    CHECK(memcmp(&evidence, &saved, sizeof(evidence)) == 0);

    file = fopen(fixture.signature, "rb");
    CHECK(file != NULL);
    CHECK(fread(signatureRecord, 1U, sizeof(signatureRecord), file) ==
          sizeof(signatureRecord));
    CHECK(fclose(file) == 0);
    memcpy(changed, signatureRecord, sizeof(signatureRecord));
    /* A single hex-digit change produces a well-formed but invalid proof. */
    changed[sizeof(signatureRecord) - 2U] = changed[sizeof(signatureRecord) - 2U] ==
        (unsigned char)'0' ? (unsigned char)'1' : (unsigned char)'0';
    CHECK(UmiTestWriteBytes(fixture.signature, changed, sizeof(signatureRecord)));
    CHECK(UmiAiTrainingAttestationVerifyAndInspectFile(fixture.base,
          fixture.trustedPem, &evidence) == UMI_STATUS_PARSE_ERROR);
    CHECK(memcmp(&evidence, &saved, sizeof(evidence)) == 0);
    CHECK(UmiTestWriteBytes(fixture.signature, signatureRecord,
                            sizeof(signatureRecord)));
    /* Fail closed on noncanonical text, including CRLF and trailing bytes. */
    memcpy(changed, signatureRecord, sizeof(signatureRecord));
    changed[0] = (unsigned char)'u';
    CHECK(UmiTestWriteBytes(fixture.signature, changed, sizeof(signatureRecord)));
    CHECK(UmiAiTrainingAttestationVerifyFile(fixture.base,
          fixture.trustedPem) == UMI_STATUS_PARSE_ERROR);
    CHECK(UmiTestWriteBytes(fixture.signature, signatureRecord,
                            sizeof(signatureRecord)));
    CHECK(UmiTestWriteBytes(fixture.signature, signatureRecord,
                            sizeof(signatureRecord) - 1U));
    CHECK(UmiAiTrainingAttestationVerifyFile(fixture.base,
          fixture.trustedPem) == UMI_STATUS_PARSE_ERROR);
    CHECK(UmiTestWriteBytes(fixture.signature, signatureRecord,
                            sizeof(signatureRecord)));
    memcpy(changed, signatureRecord, sizeof(signatureRecord));
    changed[sizeof(signatureRecord)] = (unsigned char)'\n';
    CHECK(UmiTestWriteBytes(fixture.signature, changed, sizeof(changed)));
    CHECK(UmiAiTrainingAttestationVerifyFile(fixture.base,
          fixture.trustedPem) != UMI_STATUS_OK);
    CHECK(UmiTestWriteBytes(fixture.signature, signatureRecord,
                            sizeof(signatureRecord)));
    CHECK(UmiAiTrainingAttestationVerifyFile(fixture.base,
          fixture.trustedPem) == UMI_STATUS_OK);
    /* Signature still matches a publication whose underlying source file
     * was corrupted: independent Framework publication rehash MUST reject. */
    CHECK(UmiTestWriteBytes(fixture.source, "tampered source\n", 16U));
    CHECK(UmiAiTrainingAttestationVerifyAndInspectFile(fixture.base,
          fixture.trustedPem, &evidence) == UMI_STATUS_PARSE_ERROR);
    CHECK(memcmp(&evidence, &saved, sizeof(evidence)) == 0);
    CHECK(UmiTestWriteBytes(fixture.source, "dataset digest\n", 15U));
    CHECK(UmiAiTrainingAttestationVerifyFile(fixture.base,
          fixture.trustedPem) == UMI_STATUS_OK);
    /* A rewritten marker is not authenticated even when a caller could
     * recompute ordinary unsigned file hashes. */
    CHECK(UmiTestWriteBytes(fixture.publication, "not the signed marker\n", 22U));
    CHECK(UmiAiTrainingAttestationVerifyFile(fixture.base,
          fixture.trustedPem) != UMI_STATUS_OK);
    CHECK(remove(fixture.trustedPem) == 0);
    CHECK(UmiAiTrainingAttestationVerifyFile(fixture.base,
          fixture.trustedPem) != UMI_STATUS_OK);
    UmiTestDeleteFixture(&fixture);
    puts("Native Ed25519 publication: signature, source, marker and trust rejections PASS");
    return 0;
}
